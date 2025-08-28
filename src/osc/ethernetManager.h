#ifndef ETHERNET_MANAGER_H
#define ETHERNET_MANAGER_H

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <EthernetBonjour.h>
#include "../config/consts.h"
#include "../core/settingsParser.h"
#include "../config/jsonManager.h"

extern EthernetUDP showcontrolUdp;

// NOUVELLES CONSTANTES GLOBALES pour faciliter la configuration
#define UDP_TX_PACKET_MAX_SIZE 1024
#define UDP_QUEUE_SIZE 64               // Taille de la queue UDP
#define OSC_BUDGET_MS_RUNNING 30        // Budget de traitement OSC quand en marche (ms)
#define OSC_BUDGET_MS_IDLE 10           // Budget de traitement OSC quand en attente (ms)
#define MAX_PROCESS_PER_LOOP_RUNNING 32 // Nombre max de paquets à traiter quand en cours
#define MAX_PROCESS_PER_LOOP_IDLE 16    // Nombre max de paquets à traiter quand en attente
#define MAX_RECEIVE_PER_LOOP 64         // Nombre max de paquets à recevoir par boucle
#define NETWORK_CHECK_INTERVAL 500      // Intervalle de vérification réseau (ms) - RÉDUIT: 750 -> 500
#define SERVICE_TIMEOUT_MS 3000         // Timeout pour déconnecter un service (ms) - RÉDUIT: 5000 -> 3000
#define SERVICE_CHECK_INTERVAL 250      // Intervalle de vérification des services (ms) - NOUVEAU
#define SERVICE_PING_INTERVAL 1500      // Intervalle de ping des services (ms) - NOUVEAU
#define MAX_SERVICES 10

extern int32_t showcontrolLocalPort, showcontrolAppInPort, ablesetInPort;
extern IPAddress     myDns;
extern IPAddress     gateway;
extern IPAddress     subnet;
extern IPAddress     broadcast;
extern IPAddress     manualIP;
extern IPAddress     manualDNS;

enum ServiceType { ABLESETSRV, SHOWCONTROLSRV };

struct ServiceInfo {
  IPAddress ip;
  uint16_t port;
  unsigned long lastSeen;
  bool subscribed = false;
  ServiceType serviceType;
};

// Déclaration forward de la fonction callback
void serviceFoundCallback(const char* type, MDNSServiceProtocol proto, const char* name, const uint8_t ipAddr[4], unsigned short port, const char* txtContent);

class EthernetManager {
public:
  struct UDPPacket {
    char data[UDP_TX_PACKET_MAX_SIZE];
    uint16_t size;
  };

  ServiceInfo discoveredServices[MAX_SERVICES];
  int totalServiceCount = 0;
  int serviceCounts[2] = {0, 0};
  int showcontrolPacketSize;
  bool useDHCP = false;
  
  void setIp(int *ip);
  void setPort(int32_t port);
  void setDHCP(int data);
  void setDns(int *ip);
  void setGateway(int *ip);
  void setSubnet(int *ip);
  void setup();
  void read();
  void addService(ServiceType type, const IPAddress& ip, uint16_t port);
  void initServer(ServiceType type, const IPAddress& ip, uint16_t port);
  void updateLastSeen(ServiceType type, IPAddress remoteIP);
  void clearAllServices();
  void restartBonjour();
  void discoverOSCServer();
  void showLookingForServer();
  void checkForDisconnectedServices();
  void forceServiceCheck(); // Force une vérification immédiate des services
  void listAllServices(); // Debug function
  void registerAblesetSeervice();
  
  uint8_t getQueueCount() { return queueCount; }
  bool isQueueFull() { return queueCount >= UDP_QUEUE_SIZE; }

private:
  byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xBF };
  IPAddress remoteIp;
  bool setupEthernetDone = false;
  char showcontrolPacketRcvd[UDP_TX_PACKET_MAX_SIZE];
  unsigned long lastCheckTime = 0;
  uint8_t numberOfDots = 0;

  UDPPacket udpQueue[UDP_QUEUE_SIZE];
  uint8_t queueHead = 0;              
  uint8_t queueTail = 0;              
  uint8_t queueCount = 0;             
  
  volatile bool processingUDP = false;
  
  void checkEthernetConnection();
  void pingServices();
  void showIP(int *ip);
  int getServiceTypeIndex(ServiceType type);
  int getActiveServiceCount();
  bool isServiceKnown(ServiceType type, const IPAddress& ip, uint16_t port);
  void removeService(int index, ServiceType type);
  
  bool enqueuePacket(const char* data, uint16_t size);
  bool dequeuePacket(UDPPacket& packet);
};

extern EthernetManager ethernet;

#endif
