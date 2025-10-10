#include "OSCManager.h"
#include "../core/settingsParser.h"

OSC::OSC() {}

char* OSC::get_uuid() {
    char* str2 = new char[41];
    uuid.generate();
    sprintf(str2, "uuid=%s", uuid.toCharArray());
    return str2;
}

// Specialization for handling uint8_t arrays
void OSC::addToOSCMessage(OSCMessage& msg, uint8_t* array, int length) {
    for (int i = 0; i < length; i++) {
        msg.add((int32_t)array[i]);
    }
}

void OSC::addToOSCMessage(OSCMessage& msg) {
    // Fin de récursion template
}


void OSC::sendSubscribe(IPAddress ip, uint16_t port) {
    DEBUG_OSC_LOGLN("Sending Ableset Subscribe");
    OSCMessage oscMessage("/subscribe");
    oscMessage.add("auto");
    oscMessage.add((int32_t)showcontrolLocalPort);
    oscMessage.add("show control");
    oscMessage.add(get_uuid());
    showcontrolUdp.beginPacket(ip, port);
    oscMessage.send(showcontrolUdp);
    showcontrolUdp.endPacket();
}

// void OSC::sendOSCSubscribe(IPAddress ip, uint16_t port) {
//     DEBUG_OSC_LOGLN("Sending ShowControl Subscribe");
//     OSCMessage oscMessage("/showcontrol/subscribe");
//     oscMessage.add((int32_t)showcontrolLocalPort);
//     oscMessage.add(get_uuid());
//     showcontrolUdp.beginPacket(ip, port);
//     oscMessage.send(showcontrolUdp);
//     showcontrolUdp.endPacket();
// }

void OSC::getValues(){
    if (serviceManager.serviceCounts[0] > 0){
      DEBUG_OSC_LOGLN("Requesting current values from Ableset");
      osc.sendOSC("/getValues");
      return;
    }
    DEBUG_OSC_LOGLN("Requesting current values from ShowControl");
    osc.sendOSC("/getValues");
}

void OSC::sendSysex(uint8_t* sysex_msg, int _length) {
    OSCMessage oscMessage("/showcontrol/sysex");
    addToOSCMessage(oscMessage, sysex_msg, _length);
    for (int i = 0; i < serviceManager.totalServiceCount; i++) {
        if (serviceManager.discoveredServices[i].serviceType == SHOWCONTROLSRV) {
            showcontrolUdp.beginPacket(serviceManager.discoveredServices[i].ip, serviceManager.discoveredServices[i].port);
            oscMessage.send(showcontrolUdp);  // Send the OSC message over UDP
            showcontrolUdp.endPacket();
        }
    }
}

void OSC::sendRawOSC(OSCMessage& oscMessage) {
    for (int i = 0; i < serviceManager.totalServiceCount; i++) {
        showcontrolUdp.beginPacket(serviceManager.discoveredServices[i].ip, serviceManager.discoveredServices[i].port);
        oscMessage.send(showcontrolUdp);
        showcontrolUdp.endPacket();
    }
}

OSC osc;
