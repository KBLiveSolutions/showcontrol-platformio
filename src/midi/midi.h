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
static constexpr uint8_t CMD_HANDSHAKE = 4;
static constexpr uint8_t CMD_LIVE_UPDATE = 2;
static constexpr uint8_t CMD_SEND_PAGES_AMOUNT = 7;
static constexpr uint8_t CMD_CONFIGURE_BUTTON = 10;
static constexpr uint8_t CMD_CONFIGURE_LED = 14;
static constexpr uint8_t CMD_CONFIGURE_DISPLAY = 18;
static constexpr uint8_t CMD_SEND_SETTINGS_IP = 56;
static constexpr uint8_t CMD_SEND_SETTINGS_DISPLAY = 61;
static constexpr uint8_t CMD_SEND_SETTINGS_NIGHT = 62;
static constexpr uint8_t CMD_SEND_SETTINGS_PORT = 62;
static constexpr uint8_t CMD_SEND_SETTINGS_LED = 65;
static constexpr uint8_t CMD_SETLIST_CHANGE = 74;

// Constantes pour les commandes SysEx (réception - midi_in)
static constexpr uint8_t CMD_IN_HANDSHAKE_REQUEST = 1;
static constexpr uint8_t CMD_IN_SETTINGS_REQUEST = 3;
static constexpr uint8_t CMD_IN_MIDI_CONNECTED = 4;
static constexpr uint8_t CMD_IN_LIVE_UPDATE_REQUEST = 5;
static constexpr uint8_t CMD_IN_SEND_CONTROLS = 6;
static constexpr uint8_t CMD_IN_USER_PAGES_REQUEST = 7;
static constexpr uint8_t CMD_IN_CONFIGURE_PEDAL = 12;
static constexpr uint8_t CMD_IN_DISPLAY_ITEM = 51;
static constexpr uint8_t CMD_IN_ARRAY_ITEM = 52;
static constexpr uint8_t CMD_IN_SET_PEDAL_TYPE = 55;
static constexpr uint8_t CMD_IN_SET_IP = 56;
static constexpr uint8_t CMD_IN_SET_DNS = 57;
static constexpr uint8_t CMD_IN_SET_GATEWAY = 58;
static constexpr uint8_t CMD_IN_SET_SUBNET = 59;
static constexpr uint8_t CMD_IN_SET_DHCP = 61;
static constexpr uint8_t CMD_IN_SET_PORT = 62;
static constexpr uint8_t CMD_IN_SET_PAGES_AMOUNT = 63;
static constexpr uint8_t CMD_IN_REMOVE_USER_PAGE = 64;
static constexpr uint8_t CMD_IN_SET_BRIGHTNESS = 65;
static constexpr uint8_t CMD_IN_SET_BRIGHTNESS_ALT = 66;

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
