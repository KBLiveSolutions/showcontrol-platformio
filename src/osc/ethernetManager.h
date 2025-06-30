#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <EthernetBonjour.h>
#include "../config/consts.h"
#include "../core/settingsParser.h"

extern EthernetUDP showcontrolUdp;

// extern bool oscDebug;
// extern bool ethernetDebug;

// extern IPAddress remoteIp;
// extern bool setupEthernetDone;

// extern int showcontrolPacketSize;
// extern char showcontrolPacketRcvd[UDP_TX_PACKET_MAX_SIZE];
// extern int totalServiceCount;
// extern bool useDHCP;

// extern int serviceCounts[2]; // Index 0: Ableset, Index 1: ShowControl

// extern byte mac[6];

enum ServiceType { ABLESETSRV, SHOWCONTROLSRV };

struct ServiceInfo {
  IPAddress ip;
  uint16_t port;
  unsigned long lastSeen;
  bool subscribed = false; // Indicates if the service is subscribed
  ServiceType serviceType;
};


// extern unsigned long lastCheckTime;
// extern uint8_t numberOfDots;

#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <EthernetBonjour.h>
#include "../config/consts.h"
#include "../config/jsonManager.h"

#define MAX_SERVICES 10

extern bool oscDebug;
extern bool ethernetDebug;
extern int32_t showcontrolLocalPort, showcontrolAppInPort, ablesetInPort;
extern IPAddress     localIP;
extern IPAddress     myDns;
extern IPAddress     gateway;
extern IPAddress     subnet;
extern IPAddress     broadcast;
extern IPAddress     manualIP;
extern IPAddress     manualDNS;

class EthernetManager {
  public:



    // ServiceInfo discoveredServices[];
    ServiceInfo discoveredServices[MAX_SERVICES];
    int totalServiceCount = 0;
    int serviceCounts[2] = {0, 0}; // Index 0: Ableset, Index 1: ShowControl
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

  private:
    byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xBF };
    IPAddress remoteIp = broadcast;
    bool setupEthernetDone = false;
    char showcontrolPacketRcvd[UDP_TX_PACKET_MAX_SIZE];
    unsigned long lastCheckTime = 0;
    uint8_t numberOfDots = 0;

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
};

 void serviceFoundCallback(const char* type, MDNSServiceProtocol proto, const char* name, const uint8_t ipAddr[4], unsigned short port, const char* txtContent);
 
// Instance globale
extern EthernetUDP showcontrolUdp; 
extern EthernetManager ethernet;