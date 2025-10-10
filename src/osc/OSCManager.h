#pragma once
#include "ethernetManager.h"
#include <OSCBundle.h>
#include "UUID.h"
#include "../core/utils.h"
#include "serviceManager.h"

class OSC {
  public:
    OSC();

    char* get_uuid();

    void addToOSCMessage(OSCMessage& msg, uint8_t* array, int length);
    void addToOSCMessage(OSCMessage& msg);


    template<typename T, typename... Rest>
    void addToOSCMessage(OSCMessage& msg, T firstArg, Rest... rest) {
        msg.add(firstArg);
        addToOSCMessage(msg, rest...);
    }

    template<typename... Args>
    void sendOSC(const char* address, Args... args) {
        OSCMessage oscMessage(address);
        addToOSCMessage(oscMessage, args...);
        for (int i = 0; i < serviceManager.totalServiceCount; i++) {
            if(serviceManager.discoveredServices[i].serviceType == ABLESETSRV) oscMessage.add(get_uuid());
            showcontrolUdp.beginPacket(serviceManager.discoveredServices[i].ip, serviceManager.discoveredServices[i].port);
            oscMessage.send(showcontrolUdp);
            showcontrolUdp.endPacket();
        }
    }

    void sendRawOSC(OSCMessage& oscMessage);
    void sendSubscribe(IPAddress ip, uint16_t port);
    // void sendOSCSubscribe(IPAddress ip, uint16_t port);
    void getValues();
    void sendSysex(uint8_t* sysex_msg, int _length);

    UUID uuid;
};

extern OSC osc;