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
  if (queueCount >= UDP_QUEUE_SIZE || data == nullptr || size == 0 || size > UDP_TX_PACKET_MAX_SIZE) {  // CHANGÉ: 75 -> UDP_TX_PACKET_MAX_SIZE
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
  
  while (packetsReceived < MAX_RECEIVE_PER_LOOP) {
    showcontrolPacketSize = showcontrolUdp.parsePacket();
    
    if (showcontrolPacketSize > 0) {
      // PROTECTION : Rejeter les paquets trop grands
      if (showcontrolPacketSize > UDP_TX_PACKET_MAX_SIZE) {
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
  
  // PHASE 2 : Traitement OSC RAPIDE - Utilisation des constantes globales
  const unsigned long OSC_BUDGET_MS = settings.isRunning ? OSC_BUDGET_MS_RUNNING : OSC_BUDGET_MS_IDLE;
  const int MAX_PROCESS_PER_LOOP = settings.isRunning ? MAX_PROCESS_PER_LOOP_RUNNING : MAX_PROCESS_PER_LOOP_IDLE;
  unsigned long oscProcessingStart = millis();
  int packetsProcessed = 0;
  
  UDPPacket packet;
  while (packetsProcessed < MAX_PROCESS_PER_LOOP && 
         (millis() - oscProcessingStart) < OSC_BUDGET_MS &&
         dequeuePacket(packet)) {
    
    // TRAITEMENT DIRECT sans validation
    if (packet.size > 0 && packet.size <= UDP_TX_PACKET_MAX_SIZE) {
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
  
  EthernetBonjour.run();

  
  // Maintenance réseau moins fréquente - Utilisation de la constante globale
  static unsigned long lastNetworkCheck = 0;
  static unsigned long lastServiceCheck = 0;
  static unsigned long lastServiceDebug = 0;
  static unsigned long lastPingServices = 0;
  unsigned long currentMillis = millis();
  
  // Vérification des services déconnectés plus fréquente
  if (currentMillis - lastServiceCheck >= SERVICE_CHECK_INTERVAL) {
    lastServiceCheck = currentMillis;
    checkForDisconnectedServices();
  }
  
  // Ping des services pour demander un heartbeat
  // if (currentMillis - lastPingServices >= SERVICE_PING_INTERVAL) {
  //   lastPingServices = currentMillis;
  //   pingServices(); // Toujours envoyer des pings, même si aucun service n'est connu
  // }
  
  // Vérification réseau générale
  if (currentMillis - lastNetworkCheck >= NETWORK_CHECK_INTERVAL) {
    lastNetworkCheck = currentMillis;
    if (Ethernet.linkStatus() != LinkOFF) {
      checkEthernetConnection();
      if (activePage == SPLASH_PAGE) showLookingForServer();
    } else {
      globalPage.showEthSprite(settings.MIDIConnected, -1);
    }
  }
  
  // Debug périodique des services (toutes les 8 secondes - RÉDUIT)
  if (debugOn && currentMillis - lastServiceDebug >= 8000) {
    lastServiceDebug = currentMillis;
    if (totalServiceCount > 0) {
      DEBUG_LOG("Active services: ");
      DEBUG_LOG(totalServiceCount);
      DEBUG_LOG(" (Ableset: ");
      DEBUG_LOG(serviceCounts[0]);
      DEBUG_LOG(", ShowControl: ");
      DEBUG_LOG(serviceCounts[1]);
      DEBUG_LOGLN(")");
    }
  }
  
  processingUDP = false;
}

// Par exemple :
int EthernetManager::getServiceTypeIndex(ServiceType type) {
  return (type == ABLESETSRV) ? 0 : 1;
}

bool EthernetManager::isServiceKnown(ServiceType type, const IPAddress& ip, uint16_t port) {
  // DEBUG_LOG("====>>> Service ");
  const unsigned long currentTime = millis(); // const pour optimisation
  
  for (int i = 0; i < totalServiceCount; i++) {
    if (discoveredServices[i].serviceType == type &&
        discoveredServices[i].ip == ip &&
        discoveredServices[i].port == port) {
      discoveredServices[i].lastSeen = currentTime;
      // DEBUG_LOGLN("Known");
      return true;
    }
  }
  // DEBUG_LOGLN("NOT Known");
  return false;
}

// Add a new service
void EthernetManager::addService(ServiceType type, const IPAddress& ip, uint16_t port) {
  DEBUG_LOG("Attempting to add service: ");
  DEBUG_LOG(ip);
  DEBUG_LOG(":");
  DEBUG_LOG(port);
  DEBUG_LOG(" (");
  DEBUG_LOG((type == ABLESETSRV) ? "Ableset" : "ShowControl");
  DEBUG_LOGLN(")");
  
  // Pour Ableset, toujours n'utiliser qu'un service par IP, port 39051
  uint16_t servicePort = (type == ABLESETSRV) ? 39051 : port;
  for (int i = 0; i < totalServiceCount; i++) {
    if (discoveredServices[i].serviceType == type &&
        discoveredServices[i].ip == ip &&
        discoveredServices[i].port == servicePort) {
      DEBUG_LOG("Service already exists: ");
      DEBUG_LOG(ip);
      DEBUG_LOG(":");
      DEBUG_LOG(servicePort);
      DEBUG_LOG(" (");
      DEBUG_LOG((type == ABLESETSRV) ? "Ableset" : "ShowControl");
      DEBUG_LOG(") - updating lastSeen");
      discoveredServices[i].lastSeen = millis();
      DEBUG_LOGLN("");
      return; // Service déjà présent, ne pas ajouter
    }
  }
  int index = getServiceTypeIndex(type);
  if (serviceCounts[index] < MAX_SERVICES && totalServiceCount < MAX_SERVICES * 2) {
    unsigned long currentTime = millis();
    discoveredServices[totalServiceCount].ip = ip;
    discoveredServices[totalServiceCount].port = servicePort;
    discoveredServices[totalServiceCount].lastSeen = currentTime;
    discoveredServices[totalServiceCount].serviceType = type;
    discoveredServices[totalServiceCount].subscribed = false;
    serviceCounts[index]++;
    totalServiceCount++;

    if (type == ABLESETSRV) {
      DEBUG_LOG_VALUE("====>>> Ableset  connected port : ", servicePort);
      sendOSCAblesetSubscribe();
      DEBUG_LOGLN("Ableset service added, waiting for /subscribed confirmation");
    } else {
      DEBUG_LOG_VALUE("====>>> ShowControl connected port : ", port);
      sendOSCShowControl("/showcontrol/getValues");
    }

    globalPage.showEthSprite(settings.MIDIConnected, serviceCounts[0]);
    DEBUG_LOG("Service added: totalServiceCount=");
    DEBUG_LOG(totalServiceCount);
    DEBUG_LOG(", serviceCounts[0]=");
    DEBUG_LOG(serviceCounts[0]);
    DEBUG_LOG(", serviceCounts[1]=");
    DEBUG_LOG(serviceCounts[1]);
    DEBUG_LOGLN("");
    listAllServices();
  } else {
    DEBUG_LOG("Cannot add service - limits reached: serviceCounts[");
    DEBUG_LOG(index);
    DEBUG_LOG("]=");
    DEBUG_LOG(serviceCounts[index]);
    DEBUG_LOG(", totalServiceCount=");
    DEBUG_LOGLN(totalServiceCount);
  }
}

// Function to handle new service discovery
void EthernetManager::initServer(ServiceType type, const IPAddress& ip, uint16_t port) {
    // DEBUG_LOG("New service discovered (");
  if (!isServiceKnown(type, ip, port)) {
    addService(type, ip, port);
  }
}

// Helper to remove disconnected services
void EthernetManager::removeService(int index, ServiceType type) {
  DEBUG_LOG("Removing service at index ");
  DEBUG_LOG(index);
  DEBUG_LOG(" (type: ");
  DEBUG_LOG((type == ABLESETSRV) ? "Ableset" : "ShowControl");
  DEBUG_LOGLN(")");
  
  int serviceTypeIndex = getServiceTypeIndex(type);
  for (int j = index; j < totalServiceCount - 1; j++) {
    discoveredServices[j] = discoveredServices[j + 1];
  }
  serviceCounts[serviceTypeIndex]--; // Reduce service count
  totalServiceCount--; // Décrémenter le nombre total de services
  
  DEBUG_LOG("After removal: totalServiceCount=");
  DEBUG_LOG(totalServiceCount);
  DEBUG_LOG(", serviceCounts[0]=");
  DEBUG_LOG(serviceCounts[0]);
  DEBUG_LOG(", serviceCounts[1]=");
  DEBUG_LOGLN(serviceCounts[1]);
  
  // Mettre à jour l'affichage avec le nouveau nombre de services
  globalPage.showEthSprite(settings.MIDIConnected, serviceCounts[0]);
  DEBUG_LOG("Service count updated to: ");
  DEBUG_LOGLN(serviceCounts[0]);
}

// Check for disconnected services
void EthernetManager::checkForDisconnectedServices() {
  unsigned long currentMillis = millis();
  for (int i = 0; i < totalServiceCount; i++) {
    if (currentMillis - discoveredServices[i].lastSeen > SERVICE_TIMEOUT_MS) {  // CHANGÉ: 10000 -> SERVICE_TIMEOUT_MS
      DEBUG_LOG("Service disconnected: ");
      DEBUG_LOG(discoveredServices[i].ip);
      DEBUG_LOG(":");
      DEBUG_LOG(discoveredServices[i].port);
      DEBUG_LOG(" (");
      DEBUG_LOG((discoveredServices[i].serviceType == ABLESETSRV) ? "Ableset" : "ShowControl");
      DEBUG_LOGLN(")");

      removeService(i, discoveredServices[i].serviceType);
      i--; // Adjust index after removal
      
      // Mettre à jour l'affichage
      globalPage.showEthSprite(settings.MIDIConnected, serviceCounts[0]);
      
      // Debug: lister tous les services après suppression
      listAllServices();
    }
  }
}

void EthernetManager::updateLastSeen(ServiceType type, IPAddress remoteIP) {
  unsigned long currentTime = millis();
  for (int i = 0; i < totalServiceCount; i++) {
    if (discoveredServices[i].serviceType == type &&
        discoveredServices[i].ip == remoteIP &&
        (type != ABLESETSRV || discoveredServices[i].port == 39051)) {
      discoveredServices[i].lastSeen = currentTime;
      return;
    }
  }
}

void EthernetManager::clearAllServices() {
  DEBUG_LOG("Clearing all discovered services (was: ");
  DEBUG_LOG(totalServiceCount);
  DEBUG_LOGLN(" services)");
  
  totalServiceCount = 0;
  serviceCounts[0] = 0;  // ABLESETSRV
  serviceCounts[1] = 0;  // SHOWCONTROLSRV
  
  // Optionnel : réinitialiser le tableau (pas strictement nécessaire)
  memset(discoveredServices, 0, sizeof(discoveredServices));
  
  DEBUG_LOGLN("All services cleared");
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
  IPAddress newIP(ip[0], ip[1], ip[2], ip[3]);
  manualIP = newIP;
  
  // Sauvegarder dans la configuration
  jsonManager.writeManualIP(ip);
  
  // Afficher la nouvelle IP
  showIP(ip);
  
  // Appliquer la nouvelle IP directement si Ethernet est initialisé
  if (setupEthernetDone) {
    DEBUG_LOGLN("Applying new IP to existing Ethernet connection");
    Ethernet.setLocalIP(manualIP);
  
    
    // Redémarrer UDP avec le nouveau port si nécessaire
    showcontrolUdp.stop();
    if (showcontrolUdp.begin(showcontrolLocalPort)) {
      DEBUG_LOG("UDP restarted on new IP: ");
      DEBUG_LOGLN(showcontrolLocalPort);
    }
    
    // Redémarrer EthernetBonjour avec la nouvelle IP
    restartBonjour();
  }
  // Si Ethernet n'est pas encore initialisé, la nouvelle IP sera utilisée au prochain setup()
}

void EthernetManager::setPort(int32_t port) {
  showcontrolLocalPort = port;
  jsonManager.writeOption("port", port);
  
  if (setupEthernetDone) {
    DEBUG_LOGLN("Applying new port to existing Ethernet connection");
    
    // Redémarrer UDP avec le nouveau port
    showcontrolUdp.stop();
    if (showcontrolUdp.begin(showcontrolLocalPort)) {
      DEBUG_LOG("UDP restarted on new port: ");
      DEBUG_LOGLN(showcontrolLocalPort);
    }
    
    // Redémarrer EthernetBonjour avec le nouveau port
    restartBonjour();
  }
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

void EthernetManager::restartBonjour() {
  DEBUG_LOGLN("Restarting EthernetBonjour with new IP");
  
  // Vider la liste des services découverts pour éviter les conflits
  clearAllServices();
  
  // Essayons d'arrêter EthernetBonjour si une méthode stop/end existe
  // Note: toutes les bibliothèques Bonjour n'ont pas forcément une méthode stop
  // mais on peut toujours recommencer avec begin()
  
  // Redémarrer EthernetBonjour avec la nouvelle IP
  EthernetBonjour.begin("showcontrolDevice");
  EthernetBonjour.setServiceFoundCallback(serviceFoundCallback);
  
  // Redémarrer la découverte des services OSC
  discoverOSCServer();
  
  DEBUG_LOGLN("EthernetBonjour restarted successfully");
}

void EthernetManager::showLookingForServer() {
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
    int buf[4] = {manualIP[0], manualIP[1], manualIP[2], manualIP[3]};
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
  // ABLESET NE PEUT PAS ÊTRE PINGÉ - Il envoie seulement des /heartbeat de lui-même
  // Donc on ne fait rien pour Ableset, on attend juste ses /heartbeat
  
  // Ping des services ShowControl connus
  if (serviceCounts[1] > 0) {  // ShowControl services
    DEBUG_LOGLN("Pinging known ShowControl services");
    sendOSCShowControl("/showcontrol/ping");
  } else {
    // Si aucun service ShowControl connu, envoyer un ping global pour découvrir
    DEBUG_LOGLN("No ShowControl services known, sending discovery ping");
    sendOSCShowControl("/showcontrol/ping");
  }
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
    DEBUG_LOG(" (localIP: ");
    DEBUG_LOG(manualIP);
    DEBUG_LOGLN(")");
    
    if(port > 100 && ipAddr[0] == manualIP[0] && ipAddr[1] == manualIP[1] && ipAddr[2] == manualIP[2]){
      DEBUG_LOGLN("IP filter passed, processing service...");
      // Vérification directe du nom du service
      if(strcmp(name, "showcontrol") == 0) {
        DEBUG_LOGLN("Found ShowControl service");
        ethernet.initServer(SHOWCONTROLSRV, ip, port);
      }
      
      if (txtContent) {
        DEBUG_LOGLN("Processing txtContent for Ableset service...");
        char buf[256];
        char len = *txtContent++; 
        int i=0;
        const char ablesetString[] = "server=ableset";  // const pour optimisation
        DEBUG_LOG("txtContent length: ");
        DEBUG_LOGLN(len);
        while (len) {
          i = 0;
          while (len--)
            buf[i++] = *txtContent++;
          buf[i] = '\0';
          len = *txtContent++;
          
          DEBUG_LOG("Found txt record: ");
          DEBUG_LOGLN(buf);
          
          if(strcmp(buf, ablesetString) == 0) {
            DEBUG_LOGLN("Found Ableset service!");
            ethernet.initServer(ABLESETSRV, ip, port);
          }
        }
      }
    } else {
      DEBUG_LOG("IP filter rejected service (port: ");
      DEBUG_LOG(port);
      DEBUG_LOG(", subnet mismatch or port <= 100)");
      DEBUG_LOGLN("");
    }
  }
}

// Debug function to list all known services
void EthernetManager::listAllServices() {
  DEBUG_LOG("=== ALL SERVICES LIST (totalServiceCount=");
  DEBUG_LOG(totalServiceCount);
  DEBUG_LOG(") ===");
  
  if (totalServiceCount == 0) {
    DEBUG_LOGLN(" - NO SERVICES");
    return;
  }
  
  for (int i = 0; i < totalServiceCount; i++) {
    DEBUG_LOG("Service ");
    DEBUG_LOG(i);
    DEBUG_LOG(": Type=");
    DEBUG_LOG((discoveredServices[i].serviceType == ABLESETSRV) ? "Ableset" : "ShowControl");
    DEBUG_LOG(", IP=");
    DEBUG_LOG(discoveredServices[i].ip);
    DEBUG_LOG(", Port=");
    DEBUG_LOG(discoveredServices[i].port);
    DEBUG_LOG(", Subscribed=");
    DEBUG_LOG(discoveredServices[i].subscribed ? "YES" : "NO");
    DEBUG_LOG(", LastSeen=");
    DEBUG_LOG(millis() - discoveredServices[i].lastSeen);
    DEBUG_LOGLN("ms ago");
  }
  
  DEBUG_LOG("Counts: Ableset=");
  DEBUG_LOG(serviceCounts[0]);
  DEBUG_LOG(", ShowControl=");
  DEBUG_LOG(serviceCounts[1]);
  DEBUG_LOG(", Total=");
  DEBUG_LOGLN(totalServiceCount);
  
  // Vérification de cohérence
  int calculatedTotal = serviceCounts[0] + serviceCounts[1];
  if (calculatedTotal != totalServiceCount) {
    DEBUG_LOG("WARNING: Count mismatch! Calculated=");
    DEBUG_LOG(calculatedTotal);
    DEBUG_LOG(", Actual=");
    DEBUG_LOGLN(totalServiceCount);
  }
  
  DEBUG_LOGLN("=== END SERVICES LIST ===");
}

// Force une vérification immédiate des services (utile pour les tests)
void EthernetManager::forceServiceCheck() {
  DEBUG_LOGLN("Forcing immediate service check...");
  checkForDisconnectedServices();
  // pingServices();
  // Mettre à jour l'affichage
  globalPage.showEthSprite(settings.MIDIConnected, serviceCounts[0]);
}