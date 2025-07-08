#include "ethernetManager.h"
#include "OSCManager.h"
#include "OSCReceive.h"
#include "../display/colors.h"
#include "../config/jsonManager.h"
#include "../display/pages/globalPage.h"
#include "../display/pages/splashPage.h"
#include "../core/settingsParser.h"
#include "../core/utils.h"

// Variables globales
EthernetUDP showcontrolUdp;
EthernetManager ethernet;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

IPAddress     localIP(192, 168, 2, 10);
IPAddress     manualIP(192, 168, 2, 10);
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
  if (queueCount >= UDP_QUEUE_SIZE || data == nullptr || size == 0 || size > 75) {
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
  // PROTECTION simple contre la réentrance
  if (processingUDP) {
    return;
  }
  processingUDP = true;
  
  // PHASE 1 : Réception RAPIDE - Mettre tous les paquets en queue
  int packetsReceived = 0;
  const int MAX_RECEIVE_PER_LOOP = 25;  // Plus de paquets
  
  while (packetsReceived < MAX_RECEIVE_PER_LOOP) {
    showcontrolPacketSize = showcontrolUdp.parsePacket();
    
    if (showcontrolPacketSize > 0 && totalServiceCount > 0) {
      // PROTECTION : Rejeter les paquets trop grands
      if (showcontrolPacketSize > 75) {
        showcontrolUdp.flush();
        packetsReceived++;
        continue;
      }
      
      // PROTECTION : Queue presque pleine
      if (queueCount >= UDP_QUEUE_SIZE - 1) {
        showcontrolUdp.flush();
        break;
      }
      
      // Buffer temporaire
      char tempBuffer[80];
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
  
  // PHASE 2 : Traitement OSC RAPIDE - Traiter PLUSIEURS messages par boucle
  const unsigned long OSC_BUDGET_MS = settings.isRunning ? 35 : 10;  // Budget généreux
  const int MAX_PROCESS_PER_LOOP = settings.isRunning ? 32 : 16;     // Traiter beaucoup de messages
  unsigned long oscProcessingStart = millis();
  int packetsProcessed = 0;
  
  UDPPacket packet;
  while (packetsProcessed < MAX_PROCESS_PER_LOOP && 
         (millis() - oscProcessingStart) < OSC_BUDGET_MS &&
         dequeuePacket(packet)) {
    
    // TRAITEMENT DIRECT sans validation
    if (packet.size > 0 && packet.size <= 75) {
      receiveOSCMsg(packet.data, packet.size);
    }
    packetsProcessed++;
  }
  
  // DIAGNOSTIC minimal
  if (debugOn && (packetsReceived > 10 || packetsProcessed > 10)) {
    DEBUG_LOG("R:");
    DEBUG_LOG(packetsReceived);
    DEBUG_LOG(" P:");
    DEBUG_LOG(packetsProcessed);
    DEBUG_LOG(" Q:");
    DEBUG_LOG(queueCount);
    DEBUG_LOG("/");
    DEBUG_LOGLN(UDP_QUEUE_SIZE);
  }
  
  // Service discovery seulement si queue pas trop pleine
  if (!settings.isRunning && queueCount < UDP_QUEUE_SIZE / 2) {
    EthernetBonjour.run();
  }
  
  // Maintenance réseau moins fréquente
  static unsigned long lastNetworkCheck = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - lastNetworkCheck >= 750) {  // 750ms au lieu de 500ms
    lastNetworkCheck = currentMillis;
    if (Ethernet.linkStatus() != LinkOFF) {
      checkEthernetConnection();
      if (activePage == SPLASH_PAGE) showLookingForServer();
    } else {
      globalPage.showEthSprite(settings.MIDIConnected, -1);
    }
  }
  
  processingUDP = false;
}

// Par exemple :
int EthernetManager::getServiceTypeIndex(ServiceType type) {
  return (type == ABLESETSRV) ? 0 : 1;
}

bool EthernetManager::isServiceKnown(ServiceType type, const IPAddress& ip, uint16_t port) {
  DEBUG_LOG("====>>> Service ");
  const unsigned long currentTime = millis(); // const pour optimisation
  
  for (int i = 0; i < totalServiceCount; i++) {
    if (discoveredServices[i].serviceType == type &&
        discoveredServices[i].ip == ip &&
        discoveredServices[i].port == port) {
      discoveredServices[i].lastSeen = currentTime;
      DEBUG_LOGLN("Known");
      if(activePage == SPLASH_PAGE) settings.getItStarted();
      return true;
    }
  }
  DEBUG_LOGLN("NOT Known");
  return false;
}

// Add a new service
void EthernetManager::addService(ServiceType type, const IPAddress& ip, uint16_t port) {
  int index = getServiceTypeIndex(type);
  if (serviceCounts[index] < MAX_SERVICES) {
    unsigned long currentTime = millis(); // Une seule lecture de millis()

    discoveredServices[totalServiceCount].ip = ip;
    discoveredServices[totalServiceCount].port = port;
    discoveredServices[totalServiceCount].lastSeen = currentTime;
    discoveredServices[totalServiceCount].serviceType = type;
    discoveredServices[totalServiceCount].subscribed = false;
    serviceCounts[index]++;
    totalServiceCount++;

    if (type == ABLESETSRV) {
      DEBUG_LOG_VALUE("====>>> Ableset  connected port : ", port);
      // showSpriteHPadding("Ableset connected", defaultTxtColor, ethernetStatusSprite, -228);
      sendOSCAblesetSubscribe();
    } 
    else {
      DEBUG_LOG_VALUE("====>>> ShowControl connected port : ", port);
      // showSpriteHPadding("ShowControl connected", defaultTxtColor, ethernetStatusSprite, -228);
      sendOSCShowControl("/showcontrol/getValues");
    }
    // activePage.showPage();
    // globalPage.updateEthSprite(getActiveServiceCount());
  }
}

// Function to handle new service discovery
void EthernetManager::initServer(ServiceType type, const IPAddress& ip, uint16_t port) {
    DEBUG_LOG("New service discovered (");
  if (!isServiceKnown(type, ip, port)) {
    addService(type, ip, port);
  }
}

// Helper to remove disconnected services
void EthernetManager::removeService(int index, ServiceType type) {
  int serviceTypeIndex = getServiceTypeIndex(type);
  for (int j = index; j < totalServiceCount - 1; j++) {
    discoveredServices[j] = discoveredServices[j + 1];
  }
  serviceCounts[serviceTypeIndex]--; // Reduce service count
  totalServiceCount = serviceCounts[0] + serviceCounts[1];
  // globalPage.updateEthSprite(getActiveServiceCount());
}

// Check for disconnected services
void EthernetManager::checkForDisconnectedServices() {
  unsigned long currentMillis = millis();
  for (int i = 0; i < totalServiceCount; i++) {
    if (currentMillis - discoveredServices[i].lastSeen > 10000) {
      DEBUG_LOG("Service disconnected: ");
      DEBUG_LOG(discoveredServices[i].ip);
      DEBUG_LOG(":");
      DEBUG_LOGLN(discoveredServices[i].port);

      removeService(i, discoveredServices[i].serviceType);
      i--; // Adjust index after removal
    }
  }
}

void EthernetManager::updateLastSeen(ServiceType type, IPAddress remoteIP) {
  unsigned long currentTime = millis();
  for (int i = 0; i < totalServiceCount; i++) {
    if (discoveredServices[i].serviceType == type && 
        discoveredServices[i].ip == remoteIP) {
      discoveredServices[i].lastSeen = currentTime;
      return; // Exit the function early
    }
  }
}

void EthernetManager::showIP(int *ip) {
  DEBUG_LOGLN("Printing ip ");
  static char buf[16];
  sprintf(buf, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  if(activePage == SPLASH_PAGE) splashPage.showSplashSub1Sprite(buf, defaultTxtColor);
  globalPage.showEthSprite(settings.MIDIConnected, 0);
  DEBUG_LOGLN(buf);
  DEBUG_LOGLN("IP printed");
}

void EthernetManager::setIp(int *ip) {
  IPAddress ipAdr(ip[0], ip[1], ip[2], ip[3]);
  manualIP = ipAdr;
  showIP(ip);
  jsonManager.writeManualIP(ip);
  Ethernet.setLocalIP(manualIP);
}

void EthernetManager::setPort(int32_t port) {
  showcontrolLocalPort = port;
  jsonManager.writeOption("port", port);
}

void EthernetManager::setDHCP(int data){
  DEBUG_LOG_VALUE("DHCP: ", data);
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
  DEBUG_LOGLN("Discovering OSC server");
  EthernetBonjour.startDiscoveringService("_osc", MDNSServiceUDP, 0);
}

void EthernetManager::showLookingForServer() {
  // discoverOSCServer();
  char text[30] = "Looking for Ableset";  // Buffer to hold the text
  if (numberOfDots == 1) strcat(text, ".");
  else if (numberOfDots == 2) strcat(text, "..");
  else if (numberOfDots == 3) strcat(text, "...");
  splashPage.showSpriteHPadding(text, _LightGray, -100);
  numberOfDots++;
  if (numberOfDots > 3) numberOfDots = 0;
}

// Fonction utilitaire pour calculer le nombre de services actifs
int EthernetManager::getActiveServiceCount() {
  return (serviceCounts[0] > 0) ? serviceCounts[0] : 
         (serviceCounts[1] > 0) ? serviceCounts[1] : 0;
}

void EthernetManager::setup() {
// IPAddress     localIP(jsonManager.getIPNum(0), jsonManager.getIPNum(1), jsonManager.getIPNum(2), jsonManager.getIPNum(3));
IPAddress manualIP(jsonManager.getIPNum(0), jsonManager.getIPNum(1), jsonManager.getIPNum(2), jsonManager.getIPNum(3));
  DEBUG_LOGLN("ETHERNET SETUP");
    // delay(1000);
  globalPage.showEthSprite(settings.MIDIConnected, -1);
  Ethernet.init(17); 
  if (Ethernet.linkStatus() == LinkOFF) {
    DEBUG_LOGLN("LinkOff");
  } 
  else {
    DEBUG_LOGLN("Link ON");
    if(useDHCP) {
      DEBUG_LOGLN("Start DHCP");
      if (Ethernet.begin(mac) == 0) {
        DEBUG_LOGLN("DHCP Failed");
        Ethernet.begin(mac, manualIP, myDns, gateway, subnet);
      }
      else {
        DEBUG_LOGLN("DHCP Success");
      }
    }
    else {
      DEBUG_LOGLN("Manual IP");
      Ethernet.setLocalIP(manualIP);
      Ethernet.begin(mac, manualIP, myDns, gateway, subnet);
    }
    localIP = Ethernet.localIP();
    int buf[4] = {localIP[0], localIP[1], localIP[2], localIP[3]};
    // showcontrolUdp.begin(showcontrolLocalPort);
    if (showcontrolUdp.begin(showcontrolLocalPort)) {
  DEBUG_LOG("UDP started successfully on port : ");
  DEBUG_LOGLN(showcontrolLocalPort);
} else {
  DEBUG_LOGLN("UDP start failed");
}
    setupEthernetDone = true;
    showIP(buf);
  }
  EthernetBonjour.begin("showcontrolDevice");
  EthernetBonjour.setServiceFoundCallback(serviceFoundCallback);
  discoverOSCServer();
}

void EthernetManager::pingServices(){
  sendOSCShowControl("/showcontrol/ping");
  // for(int index=0; index<2; index++){
  //   // if (index==0) sendOSCAbleset("/getHeartbeat");
  //   if (index==1) sendOSCShowControl("/showcontrol/ping");
  // }
}

void EthernetManager::checkEthernetConnection(){  
  if (Ethernet.linkStatus() == LinkON) {
    globalPage.showEthSprite(settings.MIDIConnected, serviceCounts[0]);
    if (!setupEthernetDone) {
      setup();
    }
  } 
  else {
    DEBUG_LOGLN("LinkOff");
      // showSprite("Link Off", defaultTxtColor, sub2Sprite);
      setupEthernetDone = false;  // Flag the setup to rerun when cable is connected
  }
}

void serviceFoundCallback(const char* type, MDNSServiceProtocol /*proto*/, const char* name, const uint8_t ipAddr[4], unsigned short port, const char* txtContent)
{ 
  // if (ipAddr[0] == 0 && ipAddr[1] == 0 && ipAddr[2] == 0 && ipAddr[3] == 0) {
  //   ethernet.setup(); // Reinitialize Ethernet if IP is zero
  //   return; // Skip if IP address is zero
  // }

  if (NULL == name) {
  } 
  else {
    IPAddress ip(ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
    DEBUG_LOG("Service found: ");
    DEBUG_LOG(name);
    DEBUG_LOG(" at IP: ");
    DEBUG_LOG(ip);
    DEBUG_LOG(" on port: ");
    DEBUG_LOG(port); 
    if(port > 100 && ipAddr[0] == localIP[0] && ipAddr[1] == localIP[1] && ipAddr[2] == localIP[2]){
      // Vérification directe du nom du service
      if(strcmp(name, "showcontrol") == 0) {
        ethernet.initServer(SHOWCONTROLSRV, ip, port);
      }
      
      if (txtContent) {
        char buf[256];
        char len = *txtContent++; 
        int i=0;
        const char ablesetString[] = "server=ableset";  // const pour optimisation
        DEBUG_LOG("txtContent: ");
        DEBUG_LOG(txtContent);
        DEBUG_LOG(" len: ");
        DEBUG_LOGLN(len);
        while (len) {
          i = 0;
          while (len--)
            buf[i++] = *txtContent++;
          buf[i] = '\0';
          len = *txtContent++;
          
          if(strcmp(buf, ablesetString) == 0) {
            ethernet.initServer(ABLESETSRV, ip, port);
          }
        }
      }
    }
  }
}