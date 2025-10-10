#include "serviceManager.h"
#include "../core/settingsParser.h"
#include "OSCManager.h"
#include "../display/pages/globalPage.h"

ServiceManager serviceManager;

ServiceManager::ServiceManager() {
    totalServiceCount = 0;
    serviceCounts[0] = 0;
    serviceCounts[1] = 0;
    memset(discoveredServices, 0, sizeof(discoveredServices));
}

int ServiceManager::getServiceTypeIndex(ServiceType type) {
    return (type == ABLESETSRV) ? 0 : 1;
}

bool ServiceManager::isServiceKnown(ServiceType type, const IPAddress& ip, uint16_t port) {
    const unsigned long currentTime = millis();
    for (int i = 0; i < totalServiceCount; i++) {
        if (discoveredServices[i].serviceType == type &&
            discoveredServices[i].ip == ip &&
            discoveredServices[i].port == port) {
            discoveredServices[i].lastSeen = currentTime;
            return true;
        }
    }
    return false;
}

void ServiceManager::addService(ServiceType type, const IPAddress& ip, uint16_t port) {
    DEBUG_ETHERNET_LOG("Adding service: ");
    DEBUG_ETHERNET_LOG(type == ABLESETSRV ? "Ableset" : "ShowControl");
    DEBUG_ETHERNET_LOG(" at ");
    DEBUG_ETHERNET_LOG(ip);
    DEBUG_ETHERNET_LOG(":");
    DEBUG_ETHERNET_LOGLN(port);
    uint16_t servicePort = (type == ABLESETSRV) ? 39051 : port;
    for (int i = 0; i < totalServiceCount; i++) {
        if (discoveredServices[i].serviceType == type &&
            discoveredServices[i].ip == ip &&
            discoveredServices[i].port == servicePort) {
            discoveredServices[i].lastSeen = millis();
            return;
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
        if (serviceCounts[index] == 1) osc.sendSubscribe(ip, servicePort);
        globalPage.showEthSprite(settings.MIDIConnected, totalServiceCount);
    };
}

void ServiceManager::removeService(int index, ServiceType type) {
    int serviceTypeIndex = getServiceTypeIndex(type);
    for (int j = index; j < totalServiceCount - 1; j++) {
        discoveredServices[j] = discoveredServices[j + 1];
    }
    serviceCounts[serviceTypeIndex]--;
    totalServiceCount--;
}

void ServiceManager::clearAllServices() {
    totalServiceCount = 0;
    serviceCounts[0] = 0;
    serviceCounts[1] = 0;
    memset(discoveredServices, 0, sizeof(discoveredServices));
}

void ServiceManager::checkForDisconnectedServices() {
    int tSC = totalServiceCount;
    unsigned long currentMillis = millis();
    for (int i = 0; i < totalServiceCount; i++) {
        if (currentMillis - discoveredServices[i].lastSeen > SERVICE_TIMEOUT_MS) {
            removeService(i, discoveredServices[i].serviceType);
            i--;
        }
    }
    if(totalServiceCount != tSC) globalPage.showEthSprite(settings.MIDIConnected, totalServiceCount);
}

void ServiceManager::updateLastSeen(ServiceType type, IPAddress remoteIP) {
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

int ServiceManager::getActiveServiceCount() {
    return (serviceCounts[0] > 0) ? serviceCounts[0] : 
           (serviceCounts[1] > 0) ? serviceCounts[1] : 0;
}

void ServiceManager::listAllServices() {
    // Ajoute ici ton code de debug pour afficher tous les services
}

void ServiceManager::forceServiceCheck() {
    checkForDisconnectedServices();
}