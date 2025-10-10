#include "midi.h"
#include "midi_in.h"
#include "midi_out.h"
#include "../core/mainPageParser.h"
#include "../core/settingsParser.h"
#include "../core/globalParser.h"
#include "../core/utils.h"
#include "../config/consts.h"

// Variables globales pour la gestion SysEx
bool isSysExContinued = false;
size_t sysExBufferSize = 0;
uint8_t sysExBuffer[SYSEX_BUFFER_SIZE];

void midi::onControlChange(uint8_t channel, uint8_t control, uint8_t value) {
  mainParser.onCCReceived(channel, control, value);
}

// Fonction utilitaire pour valider l'en-tête SysEx
bool isValidSysExHeader(const uint8_t *data, unsigned int length) {
  return (length >= 6 && 
          data[1] == SYSEX_MANUFACTURER_ID && 
          data[2] == SYSEX_DEVICE_ID && 
          data[3] == SYSEX_FAMILY_ID_IN && 
          data[4] == 20);
}

// Fonction utilitaire pour construire une adresse IP depuis les données MIDI
void extractIPFromSysEx(const uint8_t *data, int *ip) {
  for (int i = 0; i < 4; i++) {
    ip[i] = data[6 + i * 2] * 127 + data[7 + i * 2];
    // Validation de chaque composant IP
    if (ip[i] > 255) {
      DEBUG_LOG_VALUE("Invalid IP component extracted: ", ip[i]);
      ip[i] = 255; // Limiter à la valeur maximale valide
    }
  }
}

void midi::onSysEx(uint8_t *data, unsigned int _length) {
  // Validation de base des paramètres
  if (!data || _length < 6) {
    DEBUG_LOGLN("Invalid SysEx data received");
    return;
  }

  // Validation de l'en-tête SysEx
  if (!isValidSysExHeader(data, _length)) {
    DEBUG_LOGLN("Invalid SysEx header");
    return;
  }

  const uint8_t command = data[5];
  DEBUG_LOG_VALUE("Processing SysEx command: ", command);

  switch (command) {
    case HANDSHAKE_REQUEST: { // Handshake avec l'éditeur
      DEBUG_LOGLN("Handshake request received");
      const uint8_t editor_handshake[] = { 
        SYSEX_START_BYTE, SYSEX_MANUFACTURER_ID, SYSEX_DEVICE_ID, SYSEX_FAMILY_ID_IN, 19, 1, 
        FIRMWARE_MAJOR_VERSION, FIRMWARE_MINOR_VERSION, SYSEX_END_BYTE 
      };
      sendSysexToDAW(editor_handshake, sizeof(editor_handshake));
      break;
    }
    
    case MIDI_CONNECTED: { // MIDI Connection established
      settings.MIDIConnected = true;
      DEBUG_LOGLN("MIDI Connected");
      settings.getItStarted();
      break;
    }
    
    case SETTINGS_REQUEST: 
      DEBUG_LOGLN("Settings request received");
      sendSettings(); 
      break;
      
    case LIVE_UPDATE_REQUEST: 
      DEBUG_LOGLN("Live update request received");
      sendLiveUpdateRequest(); 
      break;
      
    case SEND_CONTROLS: 
      if (_length >= 7) {
        DEBUG_LOG_VALUE("Send controls for mode: ", data[6]);
        sendControls(data[6]); 
      }
      break;
      
    case USER_PAGES_REQUEST: 
      DEBUG_LOGLN("User page amount request received");
      sendUserPageAmount(); 
      break;
      
    case CONFIGURE_BUTTON: // Configure button
      if (_length >= 14) {
        DEBUG_LOGLN("Configure button received");
        mainParser.configureButton(data[6], data[7], data[8], data[9], data[10], data[11], data[12], data[13]); 
      }
      break;
      
    case CONFIGURE_PEDAL: // Configure pedal (commented out)
      DEBUG_LOGLN("Configure pedal received (not implemented)");
      // mainParser.configurePedal(data[6], data[7], data[8], data[9], data[10], data[11], data[12]); 
      break;
      
    case CONFIGURE_LED: // Configure LED
      if (_length >= 11) {
        DEBUG_LOGLN("Configure LED received");
        mainParser.configureLed(data[6], data[7], data[8], data[9], data[10]); 
      }
      break;
      
    case CONFIGURE_DISPLAY: // Configure display
      if (_length >= 9) {
        DEBUG_LOGLN("Configure display received");
        mainParser.configureDisplay(data[6], data[7], data[8]); 
      }
      break;
      
    case SET_PEDAL_TYPE: // Set pedal type (commented out)
      DEBUG_LOGLN("Set pedal type received (not implemented)");
      // setPedalType(data[6], data[7]); 
      break;

    case DISPLAY_ITEM:
    case ARRAY_ITEM: {
      // Validation de la longueur minimale
      const int minLength = (command == DISPLAY_ITEM) ? 9 : 9;
      if (_length < minLength) {
        DEBUG_LOG_VALUE("SysEx message too short for command: ", command);
        break;
      }
      
      const int strLength = _length - minLength;
      if (strLength <= 0 || strLength > MAX_SONG_NAME - 1) {
        DEBUG_LOG_VALUE("Invalid string length in SysEx: ", strLength);
        break;
      }
      
      // Allocation sécurisée du buffer
      char strBuf[MAX_SONG_NAME];
      memset(strBuf, 0, sizeof(strBuf));
      memcpy(strBuf, data + 7, strLength);
      strBuf[strLength] = '\0';
      
      DEBUG_LOG_VALUE("Processing string command: ", command);
      if (command == DISPLAY_ITEM) {
        parseDisplayItem(data[6], strBuf, data[_length - 2]);
      } else {
        mainParser.parseArrayItem(data[6], strBuf, data[_length - 3], data[_length - 2]);
      }
      break;
    }

    case SET_IP: 
    case SET_DNS: 
    case SET_GATEWAY: 
    case SET_SUBNET: { // IP addresses
      if (_length < 14) {
        DEBUG_LOG_VALUE("SysEx message too short for IP command: ", command);
        break;
      }
      
      int ip[4];
      extractIPFromSysEx(data, ip);
      
      switch (command) {
        case SET_IP: 
          DEBUG_LOGLN("Setting IP address");
          settings.onIPReceived(ip); 
          break;
        case SET_DNS: 
          DEBUG_LOGLN("Setting DNS address");
          settings.onDNSReceived(ip); 
          break;
        case SET_GATEWAY: 
          DEBUG_LOGLN("Setting Gateway address");
          settings.onGatewayReceived(ip); 
          break;
        case SET_SUBNET: 
          DEBUG_LOGLN("Setting Subnet mask");
          settings.onSubnetReceived(ip); 
          break;
      }
      break;
    }
    
    case SET_PORT: { // Port configuration
      if (_length < 8) {
        DEBUG_LOGLN("SysEx message too short for port command");
        break;
      }
      const int32_t port = (data[6] << 7) | data[7]; 
      DEBUG_LOG_VALUE("Setting port: ", port);
      settings.onPortReceived(port);
      break;
    }
    
    case SET_DHCP: { // DHCP configuration
      if (_length < 7) {
        DEBUG_LOGLN("SysEx message too short for DHCP command");
        break;
      }
      DEBUG_LOG_VALUE("Setting DHCP: ", data[6]);
      settings.onDHCPReceived(data[6]); 
      break;
    }
    
    case SET_PAGES_AMOUNT: { // Pages amount
      if (_length < 7) {
        DEBUG_LOGLN("SysEx message too short for pages amount command");
        break;
      }
      DEBUG_LOG_VALUE("Setting pages amount: ", data[6]);
      settings.onPagesAmountReceived(data[6]);
      break;
    }
    
    case REMOVE_USER_PAGE: { // Remove user page
      if (_length < 7) {
        DEBUG_LOGLN("SysEx message too short for remove page command");
        break;
      }
      DEBUG_LOG_VALUE("Removing user page: ", data[6]);
      settings.onRemoveUserPage(data[6]); 
      break;
    }
    
    case SET_DISPLAY_BRIGHTNESS: { // Display brightness
      if (_length < 7) {
        DEBUG_LOGLN("SysEx message too short for brightness command");
        break;
      }
      DEBUG_LOG_VALUE("Setting display brightness: ", data[6]);
      settings.setDisplayBrightness(data[6]); 
      break;
    }
    
      case SET_LED_BRIGHTNESS: { // Display brightness
      if (_length < 7) {
        DEBUG_LOGLN("SysEx message too short for brightness command");
        break;
      }
      DEBUG_LOG_VALUE("Setting display brightness: ", data[6]);
      settings.setLedBrightness(data[6]); 
      break;
    }
    
    default:
      DEBUG_LOG_VALUE("Unknown SysEx command: ", command);
      break;
  }
}


void midi::onUSBSysEx(uint8_t *data, unsigned int _length) {
  onSysEx(data, _length);
}

void midi::handleMIDIDAWMessage(uint8_t *packet) {
  uint8_t channel = (packet[1] & 0x0F);
  switch (packet[1] & 0xF0) {
    case 0x80:  // Note off
      break;
    case 0x90:  // Note on
      break;
    case 0xB0:  // Control change
      {
        uint8_t control = packet[2];
        uint8_t value = packet[3];
        onControlChange(channel, control, value);
        break;
      }
    case 0xC0:  // Program change
      {
        uint8_t program = packet[1] & 0x0F;
        DEBUG_LOG_VALUE("Program Change - Program: ", program);
        break;
      }
    case 0xE0:  // Pitch Bend
      {
        uint16_t pitchBendValue = (packet[3] << 7) | packet[2];
        DEBUG_LOG_VALUE("Pitch Bend - Value: ", pitchBendValue);
        break;
      }
    default:
      DEBUG_LOG("Unknown MIDI message type ");
      break;
  }
}

void midi::clearSysExBuffer() {
  memset(sysExBuffer, 0, SYSEX_BUFFER_SIZE);
  sysExBufferSize = 0;
  isSysExContinued = false;
}

void midi::handleSysExMessage(uint8_t *packet) {
  if (!packet) {
    DEBUG_LOGLN("Invalid packet for SysEx handling");
    return;
  }
  
  // Vérification de débordement de buffer
  const size_t bytesToCopy = MIDI_MAX_PACKET_SIZE - 1;
  if (sysExBufferSize + bytesToCopy > SYSEX_BUFFER_SIZE) {
    DEBUG_LOGLN("SysEx buffer overflow, clearing buffer");
    clearSysExBuffer();
    return;
  }
  
  memcpy(sysExBuffer + sysExBufferSize, packet + 1, bytesToCopy);
  
  for (size_t i = 1; i < MIDI_MAX_PACKET_SIZE; i++) {
    sysExBufferSize++;
    if (packet[i] == SYSEX_END_BYTE) {
      DEBUG_LOG_VALUE("Complete SysEx message received, size: ", sysExBufferSize);
      onUSBSysEx(sysExBuffer, sysExBufferSize);
      clearSysExBuffer();
      delay(2);
      return;
    }
  }
}

void midi::handleSysExStart(uint8_t *packet) {
  if (!packet) {
    DEBUG_LOGLN("Invalid packet for SysEx start");
    return;
  }
  
  DEBUG_LOGLN("Starting SysEx message reception");
  clearSysExBuffer();
  handleSysExMessage(packet);
  isSysExContinued = true;
}

void midi::handleSysExContinuation(uint8_t *packet) {
  if (!packet) {
    DEBUG_LOGLN("Invalid packet for SysEx continuation");
    return;
  }
  
  if (!isSysExContinued) {
    DEBUG_LOGLN("Received SysEx continuation without start, ignoring");
    return;
  }
  
  handleSysExMessage(packet);
}

void midi::read() {
  if (!usb_midi.available()) {
    return; // Pas de données disponibles
  }
  
  uint8_t packet[MIDI_MAX_PACKET_SIZE];
  memset(packet, 0, sizeof(packet)); // Initialisation sécurisée
  
  if (!usb_midi.readPacket(packet)) {
    DEBUG_LOGLN("Failed to read MIDI packet");
    return;
  }
  
  const uint8_t cable = (packet[0] >> 4) + 1;
  
  if (cable == 1) {
    // Canal principal pour les messages ShowControl
    if (isSysExContinued) {
      handleSysExContinuation(packet);
    } else {
      if (packet[1] == SYSEX_START_BYTE) {
        handleSysExStart(packet);
      } else {
        handleMIDIDAWMessage(packet);
      }
    }
  } else if (cable == 2) {
    // Canal de forwarding
    forwardMessageToUSBMIDI(packet);
  } else {
    DEBUG_LOG_VALUE("Unknown MIDI cable: ", cable);
  }
}

void midi::onSerialNoteOn(byte channel, byte note, byte vel) {
  DEBUG_LOG_VALUE("DIN Note in : ", note);
}




// void storeActivePage(uint8_t pageNum) {
//   // Placeholder for storing the active page logic
// }


void midi::parseDisplayItem(uint8_t itemType, char* strBuf, int arg2){
  // Validation des paramètres d'entrée
  if (!strBuf) {
    DEBUG_LOGLN("Invalid string buffer in parseDisplayItem");
    return;
  }
  
  if (itemType >= sizeof(displayedItemsArray)) {
    DEBUG_LOG_VALUE("Invalid itemType: ", itemType);
    return;
  }
  
  const displayed_item_t displayedItem = displayedItemsArray[itemType];
  DEBUG_LOG_VALUE("Parsing display item type: ", itemType);
  
  // Gestion spéciale pour Tempo
  if (displayedItem == Tempo) {
    const float tempoValue = atof(strBuf);
    DEBUG_LOG_VALUE("Setting tempo to: ", tempoValue);
    global.onTempo(tempoValue);
    return;
  }
  
  // Switch principal pour les autres types d'éléments
  switch(displayedItem) {

    case SceneName:
      DEBUG_LOGLN("Setting scene name");
      mainParser.onSceneName(strBuf);
      break;

    case TrackName:
      DEBUG_LOGLN("Setting track name");
      mainParser.onTrackName(strBuf);
      break;

    case LooperName:
      DEBUG_LOGLN("Setting looper name (not implemented)");
      // showSprite(strBuf, _LightGray, titleSprite);
      break;

    case Tempo: {
      const float tempoValue = atof(strBuf);
      DEBUG_LOG_VALUE("Setting tempo to: ", tempoValue);
      global.onTempo(tempoValue);
    }
     break;

    case ActiveSong:
      DEBUG_LOGLN("Setting active song name");
      mainParser.onActiveSongName(strBuf);
      break;
      
    case ActiveSection:
      DEBUG_LOGLN("Setting active section name");
      mainParser.onActiveSectionName(strBuf);
      break;
      
    case NextSong:
      DEBUG_LOGLN("Setting next song name");
      mainParser.onNextSongName(strBuf);
      break;
      
    case ActiveSongIndex:
      DEBUG_LOG_VALUE("Setting active song index: ", arg2);
      mainParser.onActiveSongIndex(arg2);
      break;
      
    case ActiveSongPosition:
      DEBUG_LOGLN("Active song position (not implemented)");
      // mainParser.songPercentage = atof(strBuf) / 200;
      break;
      
    case BeatsPosition:
      // DEBUG_LOGLN("Beats position (not implemented)");
      // global.current_beat = atof(strBuf);
      // globalPage.showCounter();
      break;
      
    case Active_Song_Seconds:
      DEBUG_LOGLN("Active song seconds (not implemented)");
      // mainParser.currentSeconds = atof(strBuf);
      // mainPage.showRemainingTimeInSong();
      break;
      
    case ActiveSetlist:
      DEBUG_LOGLN("Active setlist (not implemented)");
      // showSprite(strBuf, _LightGray, titleSprite);
      // mainParser.setlistTotalTime = arg2;
      // mainPage.showRemainingTimeInSet(true);
      break;
      
    default:
      DEBUG_LOG_VALUE("Unknown display item type: ", displayedItem);
      break;
  }
  
  // TODO: Uncomment when menu functionality is implemented
  // if (menuPage.activeMenu == MENU_OFF) activePage->showItem(itemType);
}