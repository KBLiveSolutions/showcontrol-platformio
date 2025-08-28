#ifndef OSC_MESSAGE_H
#define OSC_MESSAGE_H

#include <Arduino.h>
#include <string.h>

class OSCMessage {
private:
    String m_address;
    bool m_hasError;

public:
    OSCMessage() : m_hasError(false) {}
    OSCMessage(const char* addr) : m_address(addr), m_hasError(false) {}
    
    // Basic add methods with explicit types to avoid ambiguity
    OSCMessage& add(int i) { return *this; }
    OSCMessage& add(float f) { return *this; }
    OSCMessage& add(double d) { return *this; }
    OSCMessage& add(const char* s) { return *this; }
    OSCMessage& add(String s) { return *this; }
    
    // Template send method for compatibility
    template<typename T>
    void send(T& sender) {
        // Stub implementation - does nothing
    }
    
    // Address handling
    void getAddress(char* buffer, int offset = 0) { strcpy(buffer, m_address.c_str()); }
    String getAddress() { return m_address; }
    
    // Error handling
    bool hasError() { return m_hasError; }
    int getError() { return m_hasError ? 1 : 0; }
    
    // Message construction
    void fill(uint8_t byte) { /* Stub - normally would parse OSC data */ }
    
    // Routing system - template to handle lambda functions
    template<typename F>
    void route(const char* pattern, F func) {
        if (m_address == pattern) {
            func(*this, 0);
        }
    }
    
    // Argument access
    float getFloat(int position) { return 0.0f; }
    int getInt(int position) { return 0; }
    String getString(int position) { return ""; }
    bool getBoolean(int position) { return false; }
    
    // Size
    int size() { return 0; }
};

#endif
