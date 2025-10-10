#pragma once
#include <Arduino.h>
#include <MIDI.h>
#include "Adafruit_TinyUSB.h"

extern Adafruit_USBD_MIDI usb_midi;
extern midi::MidiInterface<midi::SerialMIDI<Adafruit_USBD_MIDI>> MIDI;

// =============================================================================
// CONSTANTES SYSEX PARTAGÉES
// =============================================================================

// Constantes de base SysEx
static constexpr uint8_t SYSEX_START_BYTE = 0xF0;
static constexpr uint8_t SYSEX_END_BYTE = 0xF7;
static constexpr uint8_t SYSEX_MANUFACTURER_ID = 122;
static constexpr uint8_t SYSEX_DEVICE_ID = 29;

// Family IDs selon la direction
static constexpr uint8_t SYSEX_FAMILY_ID_SEND = 1;  // Pour l'envoi (midi_out)
static constexpr uint8_t SYSEX_FAMILY_ID_RECV = 19; // Pour la réception (midi_out commands)
static constexpr uint8_t SYSEX_FAMILY_ID_IN = 1;    // Pour la réception (midi_in)

// Constantes pour les commandes SysEx (envoi)
static constexpr uint8_t HANDSHAKE = 4;
static constexpr uint8_t LIVE_UPDATE = 2;
static constexpr uint8_t SEND_PAGES_AMOUNT = 7;
static constexpr uint8_t CONFIGURE_BUTTON = 10;
static constexpr uint8_t CONFIGURE_LED = 14;
static constexpr uint8_t CONFIGURE_DISPLAY = 18;
static constexpr uint8_t SEND_SETTINGS_IP = 56;
static constexpr uint8_t SEND_SETTINGS_NIGHT = 62;
static constexpr uint8_t SEND_SETTINGS_PORT = 62;
static constexpr uint8_t SEND_SETTINGS_LED = 65;
static constexpr uint8_t SEND_SETTINGS_DISPLAY = 66;
static constexpr uint8_t SETLIST_CHANGE = 74;

// Constantes pour les commandes SysEx (réception - midi_in)
static constexpr uint8_t HANDSHAKE_REQUEST = 1;
static constexpr uint8_t SETTINGS_REQUEST = 3;
static constexpr uint8_t MIDI_CONNECTED = 4;
static constexpr uint8_t LIVE_UPDATE_REQUEST = 5;
static constexpr uint8_t SEND_CONTROLS = 6;
static constexpr uint8_t USER_PAGES_REQUEST = 7;
static constexpr uint8_t CONFIGURE_PEDAL = 12;
static constexpr uint8_t DISPLAY_ITEM = 51;
static constexpr uint8_t ARRAY_ITEM = 52;
static constexpr uint8_t SET_PEDAL_TYPE = 55;
static constexpr uint8_t SET_IP = 56;
static constexpr uint8_t SET_DNS = 57;
static constexpr uint8_t SET_GATEWAY = 58;
static constexpr uint8_t SET_SUBNET = 59;
static constexpr uint8_t SET_DHCP = 61;
static constexpr uint8_t SET_PORT = 62;
static constexpr uint8_t SET_PAGES_AMOUNT = 63;
static constexpr uint8_t REMOVE_USER_PAGE = 64;
static constexpr uint8_t SET_LED_BRIGHTNESS = 65;
static constexpr uint8_t SET_DISPLAY_BRIGHTNESS = 66;

// Tailles des buffers SysEx
static constexpr uint8_t SYSEX_HEADER_SIZE = 6;
static constexpr uint8_t SYSEX_BUTTON_CONFIG_SIZE = 14;
static constexpr uint8_t SYSEX_LED_CONFIG_SIZE = 14;
static constexpr uint8_t SYSEX_DISPLAY_CONFIG_SIZE = 12;
static constexpr uint8_t SYSEX_SIMPLE_MSG_SIZE = 7;
static constexpr uint8_t SYSEX_MSG_WITH_VALUE_SIZE = 8;

// Constantes pour midi_in
static constexpr uint8_t MIDI_MAX_PACKET_SIZE = 4;
static constexpr size_t SYSEX_BUFFER_SIZE = (MIDI_MAX_PACKET_SIZE * 10);

namespace midi {
    void setup();
}
