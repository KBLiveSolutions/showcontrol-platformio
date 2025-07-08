#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <EthernetBonjour.h>
#include "../config/consts.h"
#include "../core/settingsParser.h"
#include "../config/jsonManager.h"

extern EthernetUDP showcontrolUdp;

#define UDP_TX_PACKET_MAX_SIZE 120
#define MAX_SERVICES 10

enum ServiceType { ABLESETSRV, SHOWCONTROLSRV };

struct ServiceInfo {
  IPAddress ip;
  uint16_t port;
  unsigned long lastSeen;
  bool subscribed = false; // Indicates if the service is subscribed
  ServiceType serviceType;
};

extern int32_t showcontrolLocalPort, showcontrolAppInPort, ablesetInPort;
extern IPAddress     localIP;
extern IPAddress     myDns;
extern IPAddress     gateway;
extern IPAddress     subnet;
extern IPAddress     broadcast;
extern IPAddress     manualIP;
extern IPAddress     manualDNS;

class EthernetManager {
  private:
    byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xBF };
    IPAddress remoteIp = broadcast;
    bool setupEthernetDone = false;
    char showcontrolPacketRcvd[UDP_TX_PACKET_MAX_SIZE];  // Utilise la constante
    unsigned long lastCheckTime = 0;
    uint8_t numberOfDots = 0;

    // Queue optimisée avec la constante centralisée
    static const uint8_t UDP_QUEUE_SIZE = 32;
    
    struct UDPPacket {
      char data[UDP_TX_PACKET_MAX_SIZE];  // Utilise la constante
      uint16_t size;
    };
    
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
    void checkForDisconnectedServices();
    void updateLastSeen(ServiceType type, IPAddress remoteIP);
    void discoverOSCServer();
    void showLookingForServer();
    
    bool enqueuePacket(const char* data, uint16_t size);
    bool dequeuePacket(UDPPacket& packet);

  public:

    UDPPacket udpQueue[UDP_QUEUE_SIZE];
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
    
    uint8_t getQueueCount() { return queueCount; }
    bool isQueueFull() { return queueCount >= UDP_QUEUE_SIZE; }
};

 void serviceFoundCallback(const char* type, MDNSServiceProtocol proto, const char* name, const uint8_t ipAddr[4], unsigned short port, const char* txtContent);
 
// Instance globale
extern EthernetUDP showcontrolUdp; 
extern EthernetManager ethernet;