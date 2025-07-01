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

bool ethernetDebug = false;
bool oscDebug = false;

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

// Par exemple :
int EthernetManager::getServiceTypeIndex(ServiceType type) {
  return (type == ABLESETSRV) ? 0 : 1;
}

bool EthernetManager::isServiceKnown(ServiceType type, const IPAddress& ip, uint16_t port) {
  ETHERNET_DEBUG_LOG("====>>> Service ");
  const unsigned long currentTime = millis(); // const pour optimisation
  
  for (int i = 0; i < totalServiceCount; i++) {
    if (discoveredServices[i].serviceType == type &&
        discoveredServices[i].ip == ip &&
        discoveredServices[i].port == port) {
      discoveredServices[i].lastSeen = currentTime;
      ETHERNET_DEBUG_LOGLN("Known");
      if(activePage == SPLASH_PAGE) settings.getItStarted();
      return true;
    }
  }
  ETHERNET_DEBUG_LOGLN("NOT Known");
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
      ETHERNET_DEBUG_LOG_VALUE("====>>> Ableset  connected port : ", port);
      // showSpriteHPadding("Ableset connected", defaultTxtColor, ethernetStatusSprite, -228);
      sendOSCAblesetSubscribe();
    } 
    else {
      ETHERNET_DEBUG_LOG_VALUE("====>>> ShowControl connected port : ", port);
      // showSpriteHPadding("ShowControl connected", defaultTxtColor, ethernetStatusSprite, -228);
      sendOSCShowControl("/showcontrol/getValues");
    }
    // activePage.showPage();
    // globalPage.updateEthSprite(getActiveServiceCount());
  }
}

// Function to handle new service discovery
void EthernetManager::initServer(ServiceType type, const IPAddress& ip, uint16_t port) {
    ETHERNET_DEBUG_LOG("New service discovered (");
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
      if(ethernetDebug) {
        ETHERNET_DEBUG_LOG("Service disconnected: ");
        ETHERNET_DEBUG_LOG(discoveredServices[i].ip);
        ETHERNET_DEBUG_LOG(":");
        ETHERNET_DEBUG_LOGLN(discoveredServices[i].port);
      }

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
  ETHERNET_DEBUG_LOGLN("Printing ip ");
  static char buf[16];
  sprintf(buf, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  if(activePage == SPLASH_PAGE) splashPage.showSplashSub1Sprite(buf, defaultTxtColor);
  globalPage.showEthSprite(settings.MIDIConnected, 0);
  ETHERNET_DEBUG_LOGLN(buf);
  ETHERNET_DEBUG_LOGLN("IP printed");
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
  ETHERNET_DEBUG_LOG_VALUE("DHCP: ", data);
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
    ETHERNET_DEBUG_LOGLN("LinkOff");
  } 
  else {
    ETHERNET_DEBUG_LOGLN("Link ON");
    if(useDHCP) {
      ETHERNET_DEBUG_LOGLN("Start DHCP");
      if (Ethernet.begin(mac) == 0) {
        ETHERNET_DEBUG_LOGLN("DHCP Failed");
        Ethernet.begin(mac, manualIP, myDns, gateway, subnet);
      }
      else {
        ETHERNET_DEBUG_LOGLN("DHCP Success");
      }
    }
    else {
      ETHERNET_DEBUG_LOGLN("Manual IP");
      Ethernet.setLocalIP(manualIP);
      Ethernet.begin(mac, manualIP, myDns, gateway, subnet);
    }
    localIP = Ethernet.localIP();
    int buf[4] = {localIP[0], localIP[1], localIP[2], localIP[3]};
    // showcontrolUdp.begin(showcontrolLocalPort);
    if (showcontrolUdp.begin(showcontrolLocalPort)) {
  ETHERNET_DEBUG_LOG("UDP started successfully on port : ");
  ETHERNET_DEBUG_LOGLN(showcontrolLocalPort);
} else {
  ETHERNET_DEBUG_LOGLN("UDP start failed");
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
    ETHERNET_DEBUG_LOGLN("LinkOff");
      // showSprite("Link Off", defaultTxtColor, sub2Sprite);
      setupEthernetDone = false;  // Flag the setup to rerun when cable is connected
  }
}

void EthernetManager::read() {
  EthernetBonjour.run();
  unsigned long currentMillis = millis();
  // if (currentMillis - lastCheckTime >= 2000) {
  //   lastCheckTime = currentMillis;
  //   checkForDisconnectedServices();
  //   pingServices();
  // }
  showcontrolPacketSize = showcontrolUdp.parsePacket();

  if (totalServiceCount > 0 && showcontrolPacketSize) {
    if(ethernetDebug){
      ETHERNET_DEBUG_LOG("Message received from: ");
      ETHERNET_DEBUG_LOG(showcontrolUdp.remoteIP());
      ETHERNET_DEBUG_LOG(" : ");
      ETHERNET_DEBUG_LOG(showcontrolUdp.remotePort());
      ETHERNET_DEBUG_LOG(" to: ");
      ETHERNET_DEBUG_LOG(manualIP);
      ETHERNET_DEBUG_LOG(" port: ");
      ETHERNET_DEBUG_LOGLN(showcontrolLocalPort);
    }
    receiveOSCMsg();
  } 
  
  if(Ethernet.linkStatus() != LinkOFF){
    static unsigned long lastCheckTime = 0;
    unsigned long currentMillis = millis();
    if(currentMillis - lastCheckTime >= 500) {
      lastCheckTime = currentMillis;
      checkEthernetConnection();
      if(activePage == SPLASH_PAGE) showLookingForServer();
    }
  }
    else globalPage.showEthSprite(settings.MIDIConnected, -1);
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
    ETHERNET_DEBUG_LOG("Service found: ");
    ETHERNET_DEBUG_LOG(name);
    ETHERNET_DEBUG_LOG(" at IP: ");
    ETHERNET_DEBUG_LOG(ip);
    ETHERNET_DEBUG_LOG(" on port: ");
    ETHERNET_DEBUG_LOG(port); 
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
        ETHERNET_DEBUG_LOG("txtContent: ");
        ETHERNET_DEBUG_LOG(txtContent);
        ETHERNET_DEBUG_LOG(" len: ");
        ETHERNET_DEBUG_LOGLN(len);
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
