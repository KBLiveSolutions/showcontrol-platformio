
#pragma once
#define MIDI_HOST_H

#include <stdint.h>
#include <Arduino.h>
#include "usbh_helper.h"

// Language ID: English
#define LANGUAGE_ID 0x0409



typedef struct {
  tusb_desc_device_t desc_device;
  uint16_t manufacturer[32];
  uint16_t product[48];
  uint16_t serial[16];
  bool mounted;
} dev_info_t;

// CFG_TUH_DEVICE_MAX is defined by tusb_config header
extern dev_info_t dev_info[CFG_TUH_DEVICE_MAX];

extern Adafruit_USBH_Host USBHost;

extern bool write_to_other_core(uint32_t data);
class MidiHost {

public:
  MidiHost();
  void begin();
  void task();
  void handleDeviceConnected(uint8_t daddr);
  void handleDeviceDisconnected(uint8_t daddr);

  // Méthodes statiques pour la communication inter-core
  static bool receive(uint32_t* midi_word); // bloquant si dispo
  static bool receive_nb(uint32_t* midi_word); // non bloquant
  static void decode_word(uint32_t midi_word, uint8_t* packet);
  void read();

private:
  // Ajoute ici des variables membres si besoin
};



extern MidiHost midiHost;