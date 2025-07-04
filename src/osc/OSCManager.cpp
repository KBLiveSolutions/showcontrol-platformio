#include "OSCManager.h"
#include "../core/settingsParser.h"

UUID uuid;

char* get_uuid() {
  char* str2 = new char[41];
  uuid.generate();
  sprintf(str2, "uuid=%s", uuid.toCharArray());
  return str2;
}

// Specialization for handling uint8_t arrays
void addToOSCMessage(OSCMessage& msg, uint8_t* array, int length) {
  for (int i = 0; i < length; i++) {
    msg.add((int32_t)array[i]);
  }
}

void addToOSCMessage(OSCMessage& msg) {
  // Fin de récursion template
}


void sendOSCAblesetSubscribe() {
  OSCMessage oscMessage("/subscribe");
  oscMessage.add("auto");
  oscMessage.add((int32_t)showcontrolLocalPort);
  oscMessage.add("show control");
  oscMessage.add(true);
  oscMessage.add(get_uuid());
  for (int i = 0; i < ethernet.totalServiceCount; i++) {
    if (ethernet.discoveredServices[i].serviceType == ABLESETSRV) {
      ETHERNET_DEBUG_LOGLN("Send Ableset Subscribe");
      showcontrolUdp.beginPacket(ethernet.discoveredServices[i].ip, ethernet.discoveredServices[i].port);
      oscMessage.send(showcontrolUdp);
      showcontrolUdp.endPacket();
    }
  }
}

void getValuesOSC(){
  sendOSCAbleset("/getValues");
  sendOSCShowControl("/showcontrol/getValues");
}

void sendOSCSysex(uint8_t* sysex_msg, int _length) {
  OSCMessage oscMessage("/showcontrol/sysex");
  addToOSCMessage(oscMessage, sysex_msg, _length);
  for (int i = 0; i < ethernet.totalServiceCount; i++) { // ethernet.serviceCounts[0] corresponds to ABLESETSRV
    if (ethernet.discoveredServices[i].serviceType == SHOWCONTROLSRV) {
      showcontrolUdp.beginPacket(ethernet.discoveredServices[i].ip, ethernet.discoveredServices[i].port);
      oscMessage.send(showcontrolUdp);  // Send the OSC message over UDP
      showcontrolUdp.endPacket();
    }
  }
}
