#pragma once
#include <Arduino.h>
#include <IPAddress.h>
#define MAX_SERVICES 10
#define SERVICE_TIMEOUT_MS 3000         // Timeout pour déconnecter un service (ms) - RÉDUIT: 5000 -> 3000
#define SERVICE_CHECK_INTERVAL 250      // Intervalle de vérification des services (ms) - NOUVEAU
#define SERVICE_PING_INTERVAL 1500      // Intervalle de ping des services (ms) - NOUVEAU

enum ServiceType { ABLESETSRV, SHOWCONTROLSRV };

struct ServiceInfo {
    IPAddress ip;
    uint16_t port;
    unsigned long lastSeen;
    ServiceType serviceType;
    bool subscribed;
};

class ServiceManager {
public:
    ServiceManager();

    void addService(ServiceType type, const IPAddress& ip, uint16_t port);
    void removeService(int index, ServiceType type);
    void clearAllServices();
    void checkForDisconnectedServices();
    void updateLastSeen(ServiceType type, IPAddress remoteIP);
    bool isServiceKnown(ServiceType type, const IPAddress& ip, uint16_t port);
    int getServiceTypeIndex(ServiceType type);
    int getActiveServiceCount();
    void listAllServices();
    void forceServiceCheck();

    int totalServiceCount;
    int serviceCounts[2];
    ServiceInfo discoveredServices[MAX_SERVICES];
};

extern ServiceManager serviceManager;