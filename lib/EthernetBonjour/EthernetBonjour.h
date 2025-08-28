#ifndef ETHERNET_BONJOUR_H
#define ETHERNET_BONJOUR_H

#include <Arduino.h>

// MDNS Service Protocol enum
enum MDNSServiceProtocol {
    MDNSServiceTCP,
    MDNSServiceUDP
};

class EthernetBonjourClass {
public:
    // Stub methods for compatibility
    void begin(const char* hostname) { /* stub */ }
    void run() { /* stub */ }
    
    // Service discovery stubs
    void addService(const char* service, MDNSServiceProtocol protocol, int port, const char* txtRecord = nullptr) { /* stub */ }
    void removeService(const char* service, MDNSServiceProtocol protocol, int port) { /* stub */ }
    
    // Callback management
    template<typename T>
    void setServiceFoundCallback(T callback) { /* stub */ }
    
    template<typename T>  
    void setServiceRemovedCallback(T callback) { /* stub */ }
};

// Global instance
extern EthernetBonjourClass EthernetBonjour;

#endif
