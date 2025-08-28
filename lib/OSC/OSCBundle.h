#ifndef OSC_BUNDLE_H
#define OSC_BUNDLE_H

#include "OSCMessage.h"

class OSCBundle {
private:
    String m_address;

public:
    OSCBundle() {}
    OSCBundle(const char* addr) : m_address(addr) {}
    
    // Bundle management
    OSCBundle& add(const char* addr) { return *this; }
    
    // Template send method for compatibility
    template<typename T>
    void send(T& sender) {
        // Stub implementation - does nothing
    }
    
    // Size
    int size() { return 0; }
};

#endif
