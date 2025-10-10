#include "ethernetManager.h"
#include "OSCManager.h"
#include "OSCReceive.h"
#include "../display/colors.h"
#include "../config/jsonManager.h"
#include "../display/pages/globalPage.h"
#include "../display/pages/splashPage.h"
#include "../core/settingsParser.h"
#include "../core/utils.h"
#include "serviceManager.h"

// Variables globales
EthernetUDP showcontrolUdp;
EthernetManager ethernet;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

IPAddress     manualIP(192, 101, 200, 71);
IPAddress     myDns(192, 168, 1, 1);
IPAddress     gateway(192, 168, 1, 1);
IPAddress     subnet(255, 255, 0, 0);
IPAddress     broadcast(255, 255, 255, 255);
IPAddress     manualDNS(192, 168, 1, 1);
int32_t       showcontrolLocalPort = jsonManager.getPort();
int32_t       showcontrolAppInPort = 40051;
int32_t       ablesetInPort = 39050;


bool EthernetManager::dequeuePacket(UDPPacket& packet) {
  if (queueCount == 0) {
    return false;
  }
  
  if (queueTail >= UDP_QUEUE_SIZE) {
    queueTail = 0;
    queueCount = 0;
    return false;
  }
  
  // COPIE RAPIDE
  memcpy(packet.data, udpQueue[queueTail].data, udpQueue[queueTail].size);
  packet.size = udpQueue[queueTail].size;
  
  queueTail = (queueTail + 1) % UDP_QUEUE_SIZE;
  queueCount--;
  
  return true;
}

bool EthernetManager::enqueuePacket(const char* data, uint16_t size) {
  if (queueCount >= UDP_QUEUE_SIZE || data == nullptr || size == 0 || size > UDP_TX_PACKET_MAX_SIZE) {
    return false;
  }
  
  if (queueHead >= UDP_QUEUE_SIZE) {
    queueHead = 0;
    queueCount = 0;
    return false;
  }
  
  // COPIE RAPIDE
  memcpy(udpQueue[queueHead].data, data, size);
  udpQueue[queueHead].size = size;
  
  queueHead = (queueHead + 1) % UDP_QUEUE_SIZE;
  queueCount++;
  
  return true;
}

void EthernetManager::read() {
  if (processingUDP) {
    return;
  }
  processingUDP = true;
  
  int packetsReceived = 0;
  
  while (packetsReceived < MAX_RECEIVE_PER_LOOP) {
    showcontrolPacketSize = showcontrolUdp.parsePacket();
    
    if (showcontrolPacketSize > 0) {
      if (showcontrolPacketSize > UDP_TX_PACKET_MAX_SIZE) {
        showcontrolUdp.flush();
        packetsReceived++;
        continue;
      }
      if (queueCount >= UDP_QUEUE_SIZE - 1) {
        showcontrolUdp.flush();
        break;
      }
      char tempBuffer[UDP_TX_PACKET_MAX_SIZE];
      int bytesRead = showcontrolUdp.read(tempBuffer, showcontrolPacketSize);
      if (bytesRead == showcontrolPacketSize && bytesRead > 0) {
        if (enqueuePacket(tempBuffer, bytesRead)) {
          packetsReceived++;
        } else {
          break;
        }
      } else {
        showcontrolUdp.flush();
        break;
      }
    } else {
      break;
    }
  }
  
  const unsigned long OSC_BUDGET_MS = settings.isRunning ? OSC_BUDGET_MS_RUNNING : OSC_BUDGET_MS_IDLE;
  const int MAX_PROCESS_PER_LOOP = settings.isRunning ? MAX_PROCESS_PER_LOOP_RUNNING : MAX_PROCESS_PER_LOOP_IDLE;
  unsigned long oscProcessingStart = millis();
  int packetsProcessed = 0;
  
  UDPPacket packet;
  while (packetsProcessed < MAX_PROCESS_PER_LOOP && 
         (millis() - oscProcessingStart) < OSC_BUDGET_MS &&
         dequeuePacket(packet)) {
    if (packet.size > 0 && packet.size <= UDP_TX_PACKET_MAX_SIZE) {
      receiveOSCMsg(packet.data, packet.size);
    }
    packetsProcessed++;
  }
  
  // if (packetsReceived > 10 || packetsProcessed > 10) {
  //   DEBUG_ETHERNET_LOG("R:");
  //   DEBUG_ETHERNET_LOG(packetsReceived);
  //   DEBUG_ETHERNET_LOG(" P:");
  //   DEBUG_ETHERNET_LOG(packetsProcessed);
  //   DEBUG_ETHERNET_LOG(" Q:");
  //   DEBUG_ETHERNET_LOG(queueCount);
  //   DEBUG_ETHERNET_LOG("/");
  //   DEBUG_ETHERNET_LOGLN(UDP_QUEUE_SIZE);
  // }
  
  EthernetBonjour.run();

  static unsigned long lastNetworkCheck = 0;
  static unsigned long lastServiceCheck = 0;
  static unsigned long lastServiceDebug = 0;
  static unsigned long lastPingServices = 0;
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastServiceCheck >= SERVICE_CHECK_INTERVAL) {
    lastServiceCheck = currentMillis;
    serviceManager.checkForDisconnectedServices();
  }
  
  if (currentMillis - lastNetworkCheck >= NETWORK_CHECK_INTERVAL) {
    lastNetworkCheck = currentMillis;
    if (Ethernet.linkStatus() == LinkOFF) {
      globalPage.showEthSprite(settings.MIDIConnected, -1);
    } 
    else {
      checkEthernetConnection();
      if (activePage == &SPLASH_PAGE) showLookingForServer();
    }
  }
  
  // if (currentMillis - lastServiceDebug >= 8000) {
  //   lastServiceDebug = currentMillis;
  //   if (serviceManager.totalServiceCount > 0) {
  //     DEBUG_ETHERNET_LOG("Active services: ");
  //     DEBUG_ETHERNET_LOG(serviceManager.totalServiceCount);
  //     DEBUG_ETHERNET_LOG(" (Ableset: ");
  //     DEBUG_ETHERNET_LOG(serviceManager.serviceCounts[0]);
  //     DEBUG_ETHERNET_LOG(", ShowControl: ");
  //     DEBUG_ETHERNET_LOG(serviceManager.serviceCounts[1]);
  //     DEBUG_ETHERNET_LOGLN(")");
  //   }
  // }
  
  processingUDP = false;
}

void EthernetManager::showIP(int *ip) {
  DEBUG_ETHERNET_LOGLN("Printing ip ");
  static char buf[16];
  sprintf(buf, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  if(activePage == &SPLASH_PAGE) {
    showLookingForServer();
    splashPage.showSplashSub1Sprite(buf, defaultTxtColor);
    globalPage.showEthSprite(settings.MIDIConnected, 0);
  }
  DEBUG_ETHERNET_LOGLN(buf);
  DEBUG_ETHERNET_LOGLN("IP printed");
}

void EthernetManager::setIp(int *ip) {
  IPAddress newIP(ip[0], ip[1], ip[2], ip[3]);
  manualIP = newIP;
  jsonManager.writeManualIP(ip);
  showIP(ip);
  if (setupEthernetDone) {
    DEBUG_ETHERNET_LOGLN("Applying new IP to existing Ethernet connection");
    Ethernet.setLocalIP(manualIP);
    showcontrolUdp.stop();
    if (showcontrolUdp.begin(showcontrolLocalPort)) {
      DEBUG_ETHERNET_LOG("UDP restarted on new IP: ");
      DEBUG_ETHERNET_LOGLN(showcontrolLocalPort);
    }
    restartBonjour();
  }
}

void EthernetManager::setPort(int32_t port) {
  showcontrolLocalPort = port;
  jsonManager.writeOption("port", port);
  if (setupEthernetDone) {
    DEBUG_ETHERNET_LOGLN("Applying new port to existing Ethernet connection");
    showcontrolUdp.stop();
    if (showcontrolUdp.begin(showcontrolLocalPort)) {
      DEBUG_ETHERNET_LOG("UDP restarted on new port: ");
      DEBUG_ETHERNET_LOGLN(showcontrolLocalPort);
    }
    restartBonjour();
  }
}

void EthernetManager::setDHCP(int data){
  DEBUG_ETHERNET_LOG_VALUE("DHCP: ", data);
  useDHCP = data;
  jsonManager.writeOption("useDHCP", data);
}

void EthernetManager::setDns(int *ip) {  
  IPAddress ipAdr(ip[0], ip[1], ip[2], ip[3]);
  myDns = ipAdr;
  Ethernet.setDnsServerIP(myDns);
}

void EthernetManager::setGateway(int *ip) {
  IPAddress ipAdr(ip[0], ip[1], ip[2], ip[3]);
  gateway = ipAdr;
  Ethernet.setGatewayIP(gateway);
}

void EthernetManager::setSubnet(int *ip) {
  IPAddress ipAdr(ip[0], ip[1], ip[2], ip[3]);
  subnet = ipAdr;
  Ethernet.setSubnetMask(subnet);
}

void EthernetManager::discoverOSCServer(){
  DEBUG_ETHERNET_LOGLN("Discovering OSC server");
  EthernetBonjour.startDiscoveringService("_osc", MDNSServiceUDP, 0);
}

void EthernetManager::restartBonjour() {
  DEBUG_ETHERNET_LOGLN("Restarting EthernetBonjour with new IP");
  serviceManager.clearAllServices();
  EthernetBonjour.begin("showcontrolDevice");
  EthernetBonjour.setServiceFoundCallback(serviceFoundCallback);
  discoverOSCServer();
  DEBUG_ETHERNET_LOGLN("EthernetBonjour restarted successfully");
}

void EthernetManager::showLookingForServer() {
  char text[30] = "Looking for Ableset";
  if (numberOfDots == 1) strcat(text, ".");
  else if (numberOfDots == 2) strcat(text, "..");
  else if (numberOfDots == 3) strcat(text, "...");
  splashPage.showSpriteHPadding(text, _LightGray, -100);
  numberOfDots++;
  if (numberOfDots > 3) numberOfDots = 0;
}

void EthernetManager::setup() {
  IPAddress manualIP(jsonManager.getIPNum(0), jsonManager.getIPNum(1), jsonManager.getIPNum(2), jsonManager.getIPNum(3));
  DEBUG_ETHERNET_LOGLN("ETHERNET SETUP");
  globalPage.showEthSprite(settings.MIDIConnected, -1);
  Ethernet.init(17); 
  if (Ethernet.linkStatus() == LinkOFF) {
    DEBUG_ETHERNET_LOGLN("LinkOff");
  } 
  else {
    DEBUG_ETHERNET_LOGLN("Link ON");
    if(useDHCP) {
      DEBUG_ETHERNET_LOGLN("Start DHCP");
      if (Ethernet.begin(mac) == 0) {
        DEBUG_ETHERNET_LOGLN("DHCP Failed");
        Ethernet.begin(mac, manualIP, myDns, gateway, subnet);
      }
      else {
        DEBUG_ETHERNET_LOGLN("DHCP Success");
      }
    }
    else {
      DEBUG_ETHERNET_LOGLN("Manual IP");
      Ethernet.setLocalIP(manualIP);
      Ethernet.begin(mac, manualIP, myDns, gateway, subnet);
    }
    int buf[4] = {manualIP[0], manualIP[1], manualIP[2], manualIP[3]};
    if (showcontrolUdp.begin(showcontrolLocalPort)) {
      DEBUG_ETHERNET_LOG("UDP started successfully on port : ");
      DEBUG_ETHERNET_LOGLN(showcontrolLocalPort);
    } else {
      DEBUG_ETHERNET_LOGLN("UDP start failed");
    }
    setupEthernetDone = true;
    showIP(buf);
  }
  EthernetBonjour.begin("showcontrolDevice");
  EthernetBonjour.setServiceFoundCallback(serviceFoundCallback);
  discoverOSCServer();
}

void EthernetManager::pingServices(){
  if (serviceManager.serviceCounts[1] > 0) {
    DEBUG_ETHERNET_LOGLN("Pinging known ShowControl services");
    osc.sendOSC("/showcontrol/ping");
  } else {
    DEBUG_ETHERNET_LOGLN("No ShowControl services known, sending discovery ping");
    osc.sendOSC("/showcontrol/ping");
  }
}

void EthernetManager::checkEthernetConnection(){  
  if (Ethernet.linkStatus() == LinkON) {
    // globalPage.showEthSprite(settings.MIDIConnected, serviceManager.serviceCounts[0]);
    if (!setupEthernetDone) {
      setup();
    }
  } 
  else {
    DEBUG_ETHERNET_LOGLN("LinkOff");
    setupEthernetDone = false;
  }
}

void serviceFoundCallback(const char* type, MDNSServiceProtocol /*proto*/, const char* name, const uint8_t ipAddr[4], unsigned short port, const char* txtContent)
{ 
  if (NULL == name) {
  } 
  else {
    IPAddress ip(ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
    DEBUG_ETHERNET_LOG("Service found: ");
    DEBUG_ETHERNET_LOG(name);
    DEBUG_ETHERNET_LOG(" at IP: ");
    DEBUG_ETHERNET_LOG(ip);
    DEBUG_ETHERNET_LOG(" on port: ");
    DEBUG_ETHERNET_LOG(port);
    DEBUG_ETHERNET_LOG(" (localIP: ");
    DEBUG_ETHERNET_LOG(manualIP);
    DEBUG_ETHERNET_LOGLN(")");
    
    if(port > 100 && ipAddr[0] == manualIP[0] && ipAddr[1] == manualIP[1] && ipAddr[2] == manualIP[2]){
      DEBUG_ETHERNET_LOGLN("IP filter passed, processing service...");
      if(strcmp(name, "showcontrol") == 0) {
        DEBUG_ETHERNET_LOGLN("Found ShowControl service");
        serviceManager.addService(SHOWCONTROLSRV, ip, port);
      }
      if (txtContent) {
        DEBUG_ETHERNET_LOGLN("Processing txtContent for Ableset service...");
        char buf[256];
        char len = *txtContent++; 
        int i=0;
        const char ablesetString[] = "server=ableset";
        DEBUG_ETHERNET_LOG("txtContent length: ");
        DEBUG_ETHERNET_LOGLN(len);
        while (len) {
          i = 0;
          while (len--)
            buf[i++] = *txtContent++;
          buf[i] = '\0';
          len = *txtContent++;
          DEBUG_ETHERNET_LOG("Found txt record: ");
          DEBUG_ETHERNET_LOGLN(buf);
          if(strcmp(buf, ablesetString) == 0) {
            DEBUG_ETHERNET_LOGLN("Found Ableset service!");
            serviceManager.addService(ABLESETSRV, ip, port);
          }
        }
      }
    } else {
      DEBUG_ETHERNET_LOG("IP filter rejected service (port: ");
      DEBUG_ETHERNET_LOG(port);
      DEBUG_ETHERNET_LOG(", subnet mismatch or port <= 100)");
      DEBUG_ETHERNET_LOGLN("");
    }
  }
}