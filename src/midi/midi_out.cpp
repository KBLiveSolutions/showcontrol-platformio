#include "midi.h"
#include "midi_out.h"
#include "../core/pages.h"
#include "../core/settingsParser.h"
#include "../core/mainPageParser.h"
#include "../osc/OSCManager.h"
#include "midi_in.h"
#include "../core/utils.h"

/**
 * @brief Crée un en-tête SysEx standard dans le buffer fourni
 * @param buffer Buffer de destination (doit être au minimum de 6 bytes)
 * @param command Commande SysEx à inclure
 */
static void createSysExHeader(uint8_t *buffer, uint8_t command) {
  if (!buffer) {
    DEBUG_LOGLN("MIDI_OUT: Null buffer in createSysExHeader");
    return;
  }
  
  buffer[0] = SYSEX_START_BYTE;
  buffer[1] = SYSEX_MANUFACTURER_ID;
  buffer[2] = SYSEX_DEVICE_ID;
  buffer[3] = SYSEX_FAMILY_ID_SEND;
  buffer[4] = SYSEX_FAMILY_ID_RECV;
  buffer[5] = command;
}

/**
 * @brief Envoie la configuration des contrôles pour un mode donné
 * @param user_mode Mode utilisateur (0-4)
 */
void midi::sendControls(uint8_t user_mode) {
  // Validation stricte du paramètre d'entrée
  if (user_mode >= 5) {
    DEBUG_LOG_VALUE("MIDI_OUT: Invalid user_mode in sendControls: ", user_mode);
    return;
  }
  
  const uint8_t adjusted_mode = user_mode + 1;
  const uint8_t mode_index = user_mode; // Pour l'index dans les tableaux (0-based)
  
  DEBUG_LOG_VALUE("MIDI_OUT: Send controls for mode: ", adjusted_mode);
  
  // Envoi des configurations de contrôles
  if (!midi::sendControlConfigurations(mode_index, adjusted_mode)) {
    DEBUG_LOGLN("MIDI_OUT: Failed to send control configurations");
    return;
  }
  
  // Envoi des configurations d'affichage
  if (!midi::sendDisplayConfigurations(mode_index, adjusted_mode)) {
    DEBUG_LOGLN("MIDI_OUT: Failed to send display configurations");
    return;
  }
  
  DEBUG_LOGLN("MIDI_OUT: Controls configuration sent successfully");
}

/**
 * @brief Envoie les configurations des boutons et LEDs pour un mode
 * @param mode_index Index du mode (0-based)
 * @param adjusted_mode Mode ajusté (1-based)
 * @return true si succès, false sinon
 */
bool midi::sendControlConfigurations(uint8_t mode_index, uint8_t adjusted_mode) {
  for (uint8_t i = 0; i < NUM_CONTROLS; i++) {
    // Configuration du bouton
    if (!midi::sendButtonConfiguration(mode_index, adjusted_mode, i)) {
      DEBUG_LOG_VALUE("MIDI_OUT: Failed to send button config for control: ", i);
      return false;
    }
    delay(2);
    
    // Configuration du LED
    if (!midi::sendLedConfiguration(mode_index, adjusted_mode, i)) {
      DEBUG_LOG_VALUE("MIDI_OUT: Failed to send LED config for control: ", i);
      return false;
    }
    delay(2);
  }
  return true;
}

/**
 * @brief Envoie la configuration d'un bouton spécifique
 * @param mode_index Index du mode (0-based)
 * @param adjusted_mode Mode ajusté (1-based)
 * @param control_index Index du contrôle
 * @return true si succès, false sinon
 */
bool midi::sendButtonConfiguration(uint8_t mode_index, uint8_t adjusted_mode, uint8_t control_index) {
  if (control_index >= NUM_CONTROLS) {
    DEBUG_LOG_VALUE("MIDI_OUT: Invalid control_index in sendButtonConfiguration: ", control_index);
    return false;
  }
  
  const uint8_t cc = pages[adjusted_mode].control_cc[control_index];
  const uint8_t ch = pages[adjusted_mode].control_ch[control_index];
  const uint8_t custom = pages[adjusted_mode].control_custom[control_index];
  const uint8_t toggle = pages[adjusted_mode].control_toggle[control_index];
  const uint8_t type = 1;
  
  uint8_t button_config[SYSEX_BUTTON_CONFIG_SIZE];
  createSysExHeader(button_config, CMD_CONFIGURE_BUTTON);
  button_config[6] = mode_index;
  button_config[7] = control_index;
  button_config[8] = type;
  button_config[9] = cc;
  button_config[10] = ch;
  button_config[11] = custom;
  button_config[12] = toggle;
  button_config[13] = SYSEX_END_BYTE;
  
  sendSysexToDAW(button_config, SYSEX_BUTTON_CONFIG_SIZE);
  return true;
}

/**
 * @brief Envoie la configuration d'un LED spécifique
 * @param mode_index Index du mode (0-based)
 * @param adjusted_mode Mode ajusté (1-based)
 * @param control_index Index du contrôle
 * @return true si succès, false sinon
 */
bool midi::sendLedConfiguration(uint8_t mode_index, uint8_t adjusted_mode, uint8_t control_index) {
  if (control_index >= NUM_CONTROLS) {
    DEBUG_LOG_VALUE("MIDI_OUT: Invalid control_index in sendLedConfiguration: ", control_index);
    return false;
  }
  
  const uint8_t led_cc = pages[adjusted_mode].control_led_cc[control_index];
  const uint8_t led_ch = pages[adjusted_mode].control_led_ch[control_index];
  const uint8_t type = 1;
  
  uint8_t led_config[SYSEX_LED_CONFIG_SIZE];
  createSysExHeader(led_config, CMD_CONFIGURE_LED);
  led_config[6] = mode_index;
  led_config[7] = control_index;
  led_config[8] = type;
  led_config[9] = led_cc;
  led_config[10] = led_ch;
  led_config[11] = 0;
  led_config[12] = 0;
  led_config[13] = SYSEX_END_BYTE;
  
  sendSysexToDAW(led_config, SYSEX_LED_CONFIG_SIZE);
  return true;
}

/**
 * @brief Envoie les configurations d'affichage pour un mode
 * @param mode_index Index du mode (0-based)
 * @param adjusted_mode Mode ajusté (1-based)
 * @return true si succès, false sinon
 */
bool midi::sendDisplayConfigurations(uint8_t mode_index, uint8_t adjusted_mode) {
  for (uint8_t i = 0; i < 3; i++) {
    const uint8_t displayType = indexOfItem(pages[adjusted_mode].displayedItem[i]);
    
    uint8_t display_config[SYSEX_DISPLAY_CONFIG_SIZE];
    createSysExHeader(display_config, CMD_CONFIGURE_DISPLAY);
    display_config[6] = mode_index;
    display_config[7] = i;
    display_config[8] = displayType;
    display_config[9] = 0;
    display_config[10] = 0;
    display_config[11] = SYSEX_END_BYTE;
    
    sendSysexToDAW(display_config, SYSEX_DISPLAY_CONFIG_SIZE);
    delay(2);
  }
  return true;
}

/**
 * @brief Transfère un message vers le port USB MIDI (actuellement désactivé)
 * @param packet Paquet MIDI à transférer
 * @note Cette fonction est commentée car elle pourrait être utilisée à l'avenir
 */
void midi::forwardMessageToUSBMIDI(uint8_t *packet) {
  // Fonction actuellement désactivée
  // Pourrait être implémentée plus tard si nécessaire
  DEBUG_LOGLN("MIDI_OUT: forwardMessageToUSBMIDI - Function disabled");
  
  // Code original commenté pour référence future:
  // size_t packet_length = sizeof(packet) / sizeof(packet[0]);
  // uint8_t message[packet_length - 1];
  // for (size_t i = 0; i < packet_length - 1; i++) {
  //   message[i] = packet[i + 1];
  // }
  // tuh_midi_stream_flush(midi_dev_addr[0]);
  // uint8_t cable = tuh_midih_get_num_tx_cables(midi_dev_addr[0]) - 1;
  // tuh_midi_stream_write(midi_dev_addr[0], cable, message, sizeof(message));
  // tuh_midi_stream_flush(midi_dev_addr[0]);
}

/**
 * @brief Envoie un message SysEx au DAW
 * @param data Données SysEx à envoyer (doit être non-null)
 * @param length Longueur des données
 */
void midi::sendSysexToDAW(const uint8_t *data, uint16_t length) {
  if (!data) {
    DEBUG_LOGLN("MIDI_OUT: Null data pointer in sendSysexToDAW");
    return;
  }
  
  if (length == 0) {
    DEBUG_LOGLN("MIDI_OUT: Zero length in sendSysexToDAW");
    return;
  }
  
  // Validation basique du format SysEx
  if (length >= 2 && (data[0] != SYSEX_START_BYTE || data[length - 1] != SYSEX_END_BYTE)) {
    DEBUG_LOGLN("MIDI_OUT: Invalid SysEx format in sendSysexToDAW");
    return;
  }
  
  DEBUG_LOG_VALUE("MIDI_OUT: Sending SysEx message, length: ", length);
  MIDI.sendSysEx(length, data, true);
}

/**
 * @brief Envoie un message MIDI vers le port DAW
 * @param type Type de contrôle (NOTE, CC, PC)
 * @param channel Canal MIDI (0-15)
 * @param data1 Premier byte de données
 * @param data2 Second byte de données
 */
void midi::sendToDAWPort(control_type_t type, uint8_t channel, uint8_t data1, uint8_t data2) {
  // Validation des paramètres
  if (channel > 15) {
    DEBUG_LOG_VALUE("MIDI_OUT: Invalid MIDI channel: ", channel);
    return;
  }
  
  if (data1 > 127 || data2 > 127) {
    DEBUG_LOG_VALUE("MIDI_OUT: Invalid MIDI data values - data1: ", data1);
    DEBUG_LOG_VALUE(", data2: ", data2);
    return;
  }
  
  uint8_t packet[4] = { 0x0A, 0, 0, 0 };
  
  switch (type) {
    case NOTE:
      packet[1] = static_cast<uint8_t>(0x90 | (channel & 0x0F));
      packet[2] = data1;
      packet[3] = data2;
      usb_midi.writePacket(packet);
      DEBUG_LOG_VALUE("MIDI_OUT: Sent NOTE message, channel: ", channel);
      break;
      
    case CC:
      packet[1] = static_cast<uint8_t>(0xB0 | (channel & 0x0F));
      packet[2] = data1;
      packet[3] = data2;
      usb_midi.writePacket(packet);
      DEBUG_LOG_VALUE("MIDI_OUT: Sent CC message, channel: ", channel);
      break;
      
    case PC:
      // Pour Program Change, on utilise directement l'écriture MIDI
      {
        uint8_t status_byte = static_cast<uint8_t>(0xC0 | (channel & 0x0F));
        usb_midi.write(status_byte);
        usb_midi.write(data1);
        DEBUG_LOG_VALUE("MIDI_OUT: Sent PC message, channel: ", channel);
      }
      break;
      
    default:
      DEBUG_LOG_VALUE("MIDI_OUT: Unknown control type: ", static_cast<int>(type));
      break;
  }
}

/**
 * @brief Envoie un changement de setlist active
 * @param setlistIndex Index de la setlist (doit être valide)
 */
void midi::sendActiveSetlistChange(uint8_t setlistIndex) {
  if (setlistIndex > 127) {
    DEBUG_LOG_VALUE("MIDI_OUT: Invalid setlistIndex: ", setlistIndex);
    return;
  }
  
  const uint8_t setlist_change_msg[SYSEX_MSG_WITH_VALUE_SIZE] = { 
    SYSEX_START_BYTE, 
    SYSEX_MANUFACTURER_ID, 
    SYSEX_DEVICE_ID, 
    SYSEX_FAMILY_ID_SEND, 
    SYSEX_FAMILY_ID_RECV, 
    CMD_SETLIST_CHANGE, 
    setlistIndex, 
    SYSEX_END_BYTE 
  };
  
  DEBUG_LOG_VALUE("MIDI_OUT: Sending setlist change, index: ", setlistIndex);
  sendSysexToDAW(setlist_change_msg, SYSEX_MSG_WITH_VALUE_SIZE);
}

/**
 * @brief Envoie un message vers le port USB MIDI
 * @param array Array contenant les données MIDI (doit être non-null et avoir au moins 4 éléments)
 */
void midi::sendToUSBMIDIPort(uint8_t *array) {
  if (!array) {
    DEBUG_LOGLN("MIDI_OUT: Null array in sendToUSBMIDIPort");
    return;
  }
  
  uint8_t newFirstByte = (0x13 << 4) | (array[0] & 0x0F);
  uint8_t packet[4] = { newFirstByte, array[1], array[2], array[3] };
  
  DEBUG_LOGLN("MIDI_OUT: Sending USB MIDI packet");
  usb_midi.writePacket(packet);
}

/**
 * @brief Envoie le nombre de pages utilisateur
 */
void midi::sendUserPageAmount() {
  // Validation du nombre de pages
  if (settings.userPagesAmount > 127) {
    DEBUG_LOG_VALUE("MIDI_OUT: Invalid userPagesAmount: ", settings.userPagesAmount);
    return;
  }
  
  uint8_t pages_amount_msg[SYSEX_MSG_WITH_VALUE_SIZE] = { 
    SYSEX_START_BYTE, 
    SYSEX_MANUFACTURER_ID, 
    SYSEX_DEVICE_ID, 
    SYSEX_FAMILY_ID_SEND, 
    SYSEX_FAMILY_ID_RECV, 
    CMD_SEND_PAGES_AMOUNT, 
    settings.userPagesAmount, 
    SYSEX_END_BYTE 
  };
  
  DEBUG_LOG_VALUE("MIDI_OUT: Sending pages amount: ", settings.userPagesAmount);
  sendSysexToDAW(pages_amount_msg, SYSEX_MSG_WITH_VALUE_SIZE);
  sendOSCSysex(pages_amount_msg, SYSEX_MSG_WITH_VALUE_SIZE);
}

/**
 * @brief Encode une adresse IP pour transmission SysEx (7-bit encoding)
 * @param ip_octet Octet IP à encoder (0-255)
 * @param msb Pointeur pour stocker le MSB (7-bit)
 * @param lsb Pointeur pour stocker le LSB (7-bit)
 */
void midi::encodeIPOctet(uint8_t ip_octet, uint8_t *msb, uint8_t *lsb) {
  if (!msb || !lsb) {
    DEBUG_LOGLN("MIDI_OUT: Null pointers in encodeIPOctet");
    return;
  }
  
  *msb = ip_octet / 127;
  *lsb = ip_octet % 127;
}

/**
 * @brief Envoie tous les paramètres de configuration
 */
void midi::sendSettings() {
  DEBUG_LOGLN("MIDI_OUT: Sending all settings");
  
  // Envoi de la luminosité des LEDs
  if (!midi::sendLedBrightnessSetting()) {
    DEBUG_LOGLN("MIDI_OUT: Failed to send LED brightness setting");
  }
  
  // Envoi du mode nuit
  if (!midi::sendNightModeSetting()) {
    DEBUG_LOGLN("MIDI_OUT: Failed to send night mode setting");
  }
  
  // Envoi de la luminosité de l'écran
  if (!midi::sendDisplayBrightnessSetting()) {
    DEBUG_LOGLN("MIDI_OUT: Failed to send display brightness setting");
  }
  
  // Envoi de l'IP manuelle
  if (!midi::sendManualIPSetting()) {
    DEBUG_LOGLN("MIDI_OUT: Failed to send manual IP setting");
  }
  
  // Envoi du port
  if (!midi::sendPortSetting()) {
    DEBUG_LOGLN("MIDI_OUT: Failed to send port setting");
  }
  
  DEBUG_LOGLN("MIDI_OUT: All settings sent");
}

/**
 * @brief Envoie le paramètre de luminosité des LEDs
 * @return true si succès, false sinon
 */
bool midi::sendLedBrightnessSetting() {
  if (settings.ledBrightness > 127) {
    DEBUG_LOG_VALUE("MIDI_OUT: Invalid ledBrightness: ", settings.ledBrightness);
    return false;
  }
  
  uint8_t led_brightness_msg[SYSEX_MSG_WITH_VALUE_SIZE] = { 
    SYSEX_START_BYTE, 
    SYSEX_MANUFACTURER_ID, 
    SYSEX_DEVICE_ID, 
    SYSEX_FAMILY_ID_SEND, 
    SYSEX_FAMILY_ID_RECV, 
    CMD_SEND_SETTINGS_LED, 
    settings.ledBrightness, 
    SYSEX_END_BYTE 
  };
  
  DEBUG_LOG_VALUE("MIDI_OUT: Sending LED brightness: ", settings.ledBrightness);
  sendSysexToDAW(led_brightness_msg, SYSEX_MSG_WITH_VALUE_SIZE);
  return true;
}

/**
 * @brief Envoie le paramètre de mode nuit
 * @return true si succès, false sinon
 */
bool midi::sendNightModeSetting() {
  uint8_t night_mode_value = settings.nightMode ? 1 : 0;
  
  uint8_t night_mode_msg[SYSEX_MSG_WITH_VALUE_SIZE] = { 
    SYSEX_START_BYTE, 
    SYSEX_MANUFACTURER_ID, 
    SYSEX_DEVICE_ID, 
    SYSEX_FAMILY_ID_SEND, 
    SYSEX_FAMILY_ID_RECV, 
    CMD_SEND_SETTINGS_NIGHT, 
    night_mode_value, 
    SYSEX_END_BYTE 
  };
  
  DEBUG_LOG_VALUE("MIDI_OUT: Sending night mode: ", night_mode_value);
  sendSysexToDAW(night_mode_msg, SYSEX_MSG_WITH_VALUE_SIZE);
  return true;
}

/**
 * @brief Envoie le paramètre de luminosité de l'écran
 * @return true si succès, false sinon
 */
bool midi::sendDisplayBrightnessSetting() {
  if (settings.displayBrightness > 127) {
    DEBUG_LOG_VALUE("MIDI_OUT: Invalid displayBrightness: ", settings.displayBrightness);
    return false;
  }
  
  uint8_t display_brightness_msg[SYSEX_MSG_WITH_VALUE_SIZE] = { 
    SYSEX_START_BYTE, 
    SYSEX_MANUFACTURER_ID, 
    SYSEX_DEVICE_ID, 
    SYSEX_FAMILY_ID_SEND, 
    SYSEX_FAMILY_ID_RECV, 
    CMD_SEND_SETTINGS_DISPLAY, 
    settings.displayBrightness, 
    SYSEX_END_BYTE 
  };
  
  DEBUG_LOG_VALUE("MIDI_OUT: Sending display brightness: ", settings.displayBrightness);
  sendSysexToDAW(display_brightness_msg, SYSEX_MSG_WITH_VALUE_SIZE);
  return true;
}

/**
 * @brief Envoie le paramètre d'IP manuelle
 * @return true si succès, false sinon
 */
bool midi::sendManualIPSetting() {
  static constexpr uint8_t SYSEX_IP_MSG_SIZE = 15;
  
  uint8_t ip_msg[SYSEX_IP_MSG_SIZE];
  createSysExHeader(ip_msg, CMD_SEND_SETTINGS_IP);
  
  // Encode chaque octet de l'IP en 7-bit
  for (uint8_t i = 0; i < 4; i++) {
    uint8_t msb, lsb;
    encodeIPOctet(manualIP[i], &msb, &lsb);
    ip_msg[6 + (i * 2)] = msb;
    ip_msg[6 + (i * 2) + 1] = lsb;
  }
  ip_msg[14] = SYSEX_END_BYTE;
  
  DEBUG_LOGLN("MIDI_OUT: Sending manual IP setting");
  sendSysexToDAW(ip_msg, SYSEX_IP_MSG_SIZE);
  return true;
}

/**
 * @brief Envoie le paramètre de port
 * @return true si succès, false sinon
 */
bool midi::sendPortSetting() {
  static constexpr uint8_t SYSEX_PORT_MSG_SIZE = 9;
  
  uint16_t port = showcontrolLocalPort;
  uint8_t port_msb = (port >> 7) & 0x7F;
  uint8_t port_lsb = port & 0x7F;
  
  uint8_t port_msg[SYSEX_PORT_MSG_SIZE] = { 
    SYSEX_START_BYTE, 
    SYSEX_MANUFACTURER_ID, 
    SYSEX_DEVICE_ID, 
    SYSEX_FAMILY_ID_SEND, 
    SYSEX_FAMILY_ID_RECV, 
    CMD_SEND_SETTINGS_PORT, 
    port_msb, 
    port_lsb, 
    SYSEX_END_BYTE 
  };
  
  DEBUG_LOG_VALUE("MIDI_OUT: Sending port: ", port);
  sendSysexToDAW(port_msg, SYSEX_PORT_MSG_SIZE);
  return true;
}

/**
 * @brief Envoie un handshake live au DAW
 */
void midi::sendLiveHandshake() {
  const uint8_t handshake_msg[SYSEX_SIMPLE_MSG_SIZE] = { 
    SYSEX_START_BYTE, 
    SYSEX_MANUFACTURER_ID, 
    SYSEX_DEVICE_ID, 
    SYSEX_FAMILY_ID_SEND, 
    SYSEX_FAMILY_ID_RECV, 
    CMD_HANDSHAKE, 
    SYSEX_END_BYTE 
  };
  
  DEBUG_LOGLN("MIDI_OUT: Sending live handshake");
  sendSysexToDAW(handshake_msg, SYSEX_SIMPLE_MSG_SIZE);
}

/**
 * @brief Envoie une demande de mise à jour live
 */
void midi::sendLiveUpdateRequest() {
  const uint8_t update_request_msg[SYSEX_SIMPLE_MSG_SIZE] = { 
    SYSEX_START_BYTE, 
    SYSEX_MANUFACTURER_ID, 
    SYSEX_DEVICE_ID, 
    SYSEX_FAMILY_ID_SEND, 
    SYSEX_FAMILY_ID_RECV, 
    CMD_LIVE_UPDATE, 
    SYSEX_END_BYTE 
  };
  
  DEBUG_LOGLN("MIDI_OUT: Sending live update request");
  sendSysexToDAW(update_request_msg, SYSEX_SIMPLE_MSG_SIZE);
}
