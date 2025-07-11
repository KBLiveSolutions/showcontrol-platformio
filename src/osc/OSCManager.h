#pragma once
#include "ethernetManager.h"
#include <OSCBundle.h>
#include "UUID.h"
#include "../core/utils.h"

// UUID global
extern UUID uuid;

// Fonctions utilitaires
char* get_uuid();

// Ajout d'arguments à un message OSC
void addToOSCMessage(OSCMessage& msg, uint8_t* array, int length);
void addToOSCMessage(OSCMessage& msg);


// Fonctions template (doivent rester dans le .h)
template<typename T, typename... Rest>
void addToOSCMessage(OSCMessage& msg, T firstArg, Rest... rest) {
  msg.add(firstArg);
  addToOSCMessage(msg, rest...);
}

template<typename... Args>
void sendOSCAbleset(const char* address, Args... args) {
  OSCMessage oscMessage(address);
  addToOSCMessage(oscMessage, args...);
  oscMessage.add(get_uuid());
  for (int i = 0; i < ethernet.totalServiceCount; i++) {
    if (ethernet.discoveredServices[i].serviceType == ABLESETSRV) {
      DEBUG_LOG("Send Ableset: ");
      DEBUG_LOG(address);
      DEBUG_LOG(" to ");
      DEBUG_LOG(ethernet.discoveredServices[i].ip);
      DEBUG_LOG(":");
      DEBUG_LOGLN(ethernet.discoveredServices[i].port);
      showcontrolUdp.beginPacket(ethernet.discoveredServices[i].ip, ethernet.discoveredServices[i].port);
      oscMessage.send(showcontrolUdp);
      showcontrolUdp.endPacket();
    }
  }
}

template<typename... Args>
void sendOSCShowControl(const char* address, Args... args) {
  OSCMessage oscMessage(address);
  addToOSCMessage(oscMessage, args...);
  for (int i = 0; i < ethernet.totalServiceCount; i++) {
    if (ethernet.discoveredServices[i].serviceType == SHOWCONTROLSRV) {
      DEBUG_LOG("Send Showcontrol: ");
      DEBUG_LOG(ethernet.discoveredServices[i].port);
      DEBUG_LOGLN(address);
      showcontrolUdp.beginPacket(ethernet.discoveredServices[i].ip, ethernet.discoveredServices[i].port);
      oscMessage.send(showcontrolUdp);
      showcontrolUdp.endPacket();
    }
  }
}
void getAblesetValues();
void sendOSCAblesetSubscribe();
void getValuesOSC();
void sendOSCSysex(uint8_t* sysex_msg, int _length);