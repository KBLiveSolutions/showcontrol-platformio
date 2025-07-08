#include "OSCReceive.h"
#include "../core/mainPageParser.h"
#include "../core/settingsParser.h"
#include "../core/globalParser.h"
#include "../display/colors.h"
#include "../core/utils.h"


// ACTIVE SONG

void onActiveSongName(OSCMessage &msg, int addrOffset) {
  char strBuf[128];  // Buffer plus grand pour accepter n'importe quelle chaîne
  msg.getString(0, strBuf, 128);
  _main.setActiveSongName(strBuf);
}

void onActiveSongColor(OSCMessage &msg, int addrOffset) {
  char strBuf[32];
  msg.getString(0, strBuf, 32);
  // DEBUG_LOG("onActiveSongColor OSC : ");
  // DEBUG_LOGLN(strBuf);
  msg.getString(1, strBuf, 32);
  _main.setActiveSongColor(hexStringtoRGB565(strBuf));
}

void onActiveSongStart(OSCMessage &msg, int addrOffset) {
  _main.setActiveSongStart(msg.getFloat(0));
}

void onActiveSongEnd(OSCMessage &msg, int addrOffset) {
  _main.setActiveSongEnd(msg.getFloat(0));
}

void onActiveSongIndex(OSCMessage &msg, int addrOffset) {
    _main.setActiveSongIndex(msg.getFloat(0));
}

void onActiveSongDuration(OSCMessage &msg, int addrOffset) {
    _main.setActiveSongDuration(msg.getFloat(0));
}

void onActiveSongProgress(OSCMessage &msg, int addrOffset){
    _main.setActiveSongProgress(msg.getFloat(0));
}

void onRemainingTimeInSet(OSCMessage &msg, int addrOffset) {
    _main.setRemainingTimeInSet(msg.getFloat(0));
    settings.isRunning = false;
}

void onRemainingTimeInSong(OSCMessage &msg, int addrOffset) {
    _main.setRemainingTimeInSong(msg.getFloat(0));
}

// NEXT SONG

void onNextSongName(OSCMessage &msg, int addrOffset) {
  char strBuf[128];  // Buffer plus grand pour accepter n'importe quelle chaîne
  msg.getString(0, strBuf, 128);
    _main.setNextSongName(strBuf);
}

void onNextSongColor(OSCMessage &msg, int addrOffset) {
  char strBuf[32];
  msg.getString(0, strBuf, 32);
    _main.setNextSongColor(hexStringtoRGB565(strBuf));
}

// ACTIVE SECTION

void onActiveSectionName(OSCMessage &msg, int addrOffset) {
  char strBuf[128];  // Buffer plus grand pour accepter n'importe quelle chaîne
  msg.getString(0, strBuf, 128);
    _main.setActiveSectionName(strBuf);
}

void onActiveSectionIndex(OSCMessage &msg, int addrOffset) {
  // activeSectionIndex = (int) msg.getFloat(0);
  // DEBUG_LOG(F("activeSectionIndex changed: "));
  // DEBUG_LOGLN(activeSectionIndex);
  // // showSprite(_main.sections[activeSectionIndex], defaultTxtColor, sub1Sprite);    
  // int count = 0;
  //   for (int i = 0; i < MAX_SECTIONS; i++) {
  //       // Check if the section is not empty
  //       if (_main.sections[i][0] != '\0') {
  //           count++;
  //       }
  //   }
  // if (activeSectionIndex > 0) showSprite(_main.sections[((int)activeSectionIndex)-1], _DarkGray, sub1PreSprite);
  // if (activeSectionIndex+1 > 0 && activeSectionIndex+1 < count) showSprite(_main.sections[(int)activeSectionIndex+1], _DarkGray, sub1PostSprite);
  // else if (activeSectionIndex+1 == count) showSprite("", _DarkGray, sub1PostSprite);

}

void onActiveSectionStart(OSCMessage &msg, int addrOffset) {
  // _main.activeSectionStart = msg.getFloat(0);
  // _main.activeSectionEnd = 0;
  // DEBUG_LOG("onActiveSectionStart: ");
  // DEBUG_LOGLN(_main.activeSectionStart);
}

void onActiveSectionEnd(OSCMessage &msg, int addrOffset) {
  // _main.activeSectionEnd = msg.getFloat(0);
  // DEBUG_LOG("_main.activeSectionEnd: ");
  // DEBUG_LOGLN(_main.activeSectionEnd);
}

void onActiveSectionColor(OSCMessage &msg, int addrOffset) {
  // char strBuf[MAX_SONG_NAME];
  // msg.getString(1, strBuf, MAX_SONG_NAME);
  // activeSectionColor = hexStringtoRGB565(strBuf);
}

// GLOBAL


void onSetlistName(OSCMessage &msg, int addrOffset) {
  // char strBuf[32];
  // msg.getString(0, strBuf, 32);
  // // parseDisplayItem(indexOfItem(ActiveSetlist), strBuf, 0);
  // showSprite(itemToDisplay[indexOfItem(ActiveSetlist)], _LightGray, titleSprite);
}


void onBeatsPosition(OSCMessage &msg, int addrOffset) {
  // DEBUG_LOG("global.beatsPosition: ");
  // DEBUG_LOGLN(global.beatsPosition);
  global.beatsPosition = msg.getFloat(0);
  global.current_bar = int(global.beatsPosition / 4) + 1;
  global.current_beat = (int)global.beatsPosition % 4 + 1;
  global.setCurrentBar(global.current_bar);
  global.setCurrentBeat(global.current_beat);
  // DEBUG_LOG("global.current_bar: ");
  // DEBUG_LOG(global.current_bar);
  // DEBUG_LOG(" | global.current_beat: ");
  // DEBUG_LOGLN(global.current_beat);
  // globalPage.showCounter();
  // updateProgressBar();
  // showProgressBarMillis = millis();
  // showProgressBarLatch = true;
}

void onIsPlaying(OSCMessage &msg, int addrOffset) {
  global.setIsPlaying( msg.getInt(0));
}

void onAudioInterfaceScene(OSCMessage &msg, int addrOffset) {
  // settings.audioInterfaceScene = msg.getInt(0);
  // globalPage.showAudioInterfaceScene();
}

void onLoopEnabled(OSCMessage &msg, int addrOffset) {
  // DEBUG_LOG("global.loopEnabled ");
  global.setLoopEnabled(msg.getInt(0));
  // if(activePage == pages[0]){
  // if (global.loopEnabled > 0) l[5].show_direct_color(255, 100, 0);
  // else l[5].led_off();
  // }
}

void onLoopStart(OSCMessage &msg, int addrOffset) {
  // DEBUG_LOG("onLoopStart ");
  // global.loopStart = msg.getFloat(0);
  // DEBUG_LOG(global.loopEnabled);
  // // showActiveSongProgressBar();
}

void onLoopEnd(OSCMessage &msg, int addrOffset) {
  // DEBUG_LOG("global.loopEnabled ");
  // global.loopEnd = msg.getFloat(0);
  // DEBUG_LOG(global.loopEnabled);
  // // showActiveSongProgressBar();
}

void onTempo(OSCMessage &msg, int addrOffset) {
  global.setTempo(msg.getFloat(0));
}

void onSignature(OSCMessage &msg, int addrOffset) {
  // int global.sig_numerator = msg.getInt(0);
  // int global.sig_denominator = msg.getInt(1);
  // sprintf(itemToDisplay[12], "%d/%d", global.sig_numerator, global.sig_denominator);
  // globalPage.showTempo();
}

// SETLIST

void onSections(OSCMessage &msg, int addrOffset) {
  int size = msg.size();
  DEBUG_LOG("onSections size: ");
  DEBUG_LOGLN(size);
  for (int j = 0; j < size; j++) {
    char strBuf[MAX_SONG_NAME];
    msg.getString(j, strBuf, MAX_SONG_NAME);
    strcpy(_main.sections[j], strBuf);
    DEBUG_LOG("_main.sections: ");
    DEBUG_LOGLN(_main.sections[j]);
  }
}

void onSongs(OSCMessage &msg, int addrOffset) {
  int size = msg.size();
  DEBUG_LOG("songs size: ");
  DEBUG_LOGLN(size);
  _main. songsListSize = size;
  for (int j = 0; j < size; j++) {
    char strBuf[MAX_SONG_NAME];
    msg.getString(j, strBuf, MAX_SONG_NAME);
    for (int i = 0; i < MAX_SONG_NAME; i++) {
      _main.songsList[j][i] = strBuf[i];
    }
    DEBUG_LOG("songs: ");
    DEBUG_LOGLN(_main.songsList[j]);
  }
  // mainPage.showSongsCounter();
}

void onSongDurations(OSCMessage &msg, int addrOffset) {
  // int size = msg.size();
  // DEBUG_LOG("songs durations: ");
  // DEBUG_LOGLN(size);
  // for (int j = 0; j < size; j++) {
  //   _main.songDurations[j] = msg.getInt(j);
  //   DEBUG_LOG("durations: ");
  //   DEBUG_LOGLN(_main.songDurations[j]);
  // }
}



void onSongColors(OSCMessage &msg, int addrOffset) {
  // int size = msg.size();
  // DEBUG_LOG("colors size: ");
  // DEBUG_LOGLN(size);
  // // for (int j = 0; j < size; j++) {
  // //   char strBuf[MAX_SONG_NAME];
  // //   msg.getString(j*2, strBuf, MAX_SONG_NAME);
  // //   songColors[j] = hexStringtoRGB565(strBuf);
  // // //   for (int i = 0; i < MAX_SONG_NAME; i++) {
  // // //     _main.songsList[j][i] = strBuf[i];
  // // // char strBuf[32];
  // // // msg.getString(1, strBuf, 32);
  // // // activeSongColor = hexStringtoRGB565(strBuf);
  // // // activeSongColorShade = hexStringtoRGB565Shade(strBuf);
  // // // activeSectionColor = activeSongColorShade;
  // //   // }
  // //   DEBUG_LOG("colors: ");
  // //   DEBUG_LOGLN(songColors[j]);
  // // }
  // // showSongs();
}

// UTILITY

void onSubscribed(OSCMessage &msg, int addrOffset) {  
  for (int i = 0; i < ethernet.totalServiceCount; i++) { // Iterate through all discovered services
      if (ethernet.discoveredServices[i].serviceType == ABLESETSRV && 
          ethernet.discoveredServices[i].ip == showcontrolUdp.remoteIP()) {
            ethernet.discoveredServices[i].subscribed = true; // Mark the service as subscribed
        break; // Exit the loop early since IPs are unique
      }
    }
  settings.getItStarted();
  DEBUG_LOGLN("Subscribed to Ableset");
  // char strBuf[32];
  // DEBUG_LOGLN(msg.getString(0, strBuf, 32));
}

void onHeartbeat(OSCMessage &msg, int addrOffset) {    
    DEBUG_LOG("Heartbeat received: ");
    DEBUG_LOGLN(settings.isRunning);
  if (!settings.isRunning){  
    bool serviceFound = false;
  
    for (int i = 0; i < ethernet.totalServiceCount; i++) { // Iterate through all discovered services
        if (ethernet.discoveredServices[i].serviceType == ABLESETSRV && 
            ethernet.discoveredServices[i].ip == showcontrolUdp.remoteIP()) {
          ethernet.discoveredServices[i].lastSeen = millis();
          if(!ethernet.discoveredServices[i].subscribed){
            DEBUG_LOGLN("Service already subscribed, updating last seen");
              ethernet.discoveredServices[i].subscribed = true; // Mark the service as subscribed
            settings.getItStarted(); // Trigger the handshake response to update the UI
          }
          serviceFound = true;
          break; // Exit the loop early since IPs are unique
        }
    }
    if (!serviceFound) {
      DEBUG_LOG("Service not found, adding service: ");
      DEBUG_LOGLN(showcontrolUdp.remoteIP());
      ethernet.addService(ABLESETSRV, showcontrolUdp.remoteIP(), showcontrolUdp.remotePort());
      settings.getItStarted();
    }
  }
  // else sendOSCAbleset("/getValues");

}

void onError(OSCMessage &msg, int addrOffset) {
  DEBUG_LOGLN(msg.getType(0));
}

void onLivePing(OSCMessage &msg, int addrOffset) {
  // DEBUG_LOGLN(("Live Ping Received"));
  // sendOSCShowControl("/showcontrol/ping");  
  // for (int i = 0; i < ethernet.totalServiceCount; i++) { // Iterate through all discovered services
  //     if (ethernet.discoveredServices[i].serviceType == SHOWCONTROLSRV && 
  //         ethernet.discoveredServices[i].ip == showcontrolUdp.remoteIP()) {
  //       ethernet.discoveredServices[i].lastSeen = millis();
  //       break; // Exit the loop early since IPs are unique
  //     }
  //   }
}

void onSCPong(OSCMessage &msg, int addrOffset) {
  // DEBUG_LOGLN(("App Ping Received"));
  //   for (int i = 0; i < ethernet.totalServiceCount; i++) { // Iterate through all discovered services
  //     if (ethernet.discoveredServices[i].serviceType == SHOWCONTROLSRV && 
  //         ethernet.discoveredServices[i].ip == showcontrolUdp.remoteIP()) {
  //       ethernet.discoveredServices[i].lastSeen = millis();
  //       break; // Exit the loop early since IPs are unique
  //     }
  //   }
}

void onMessage(OSCMessage &msg, int addrOffset) {
  DEBUG_LOGLN(("OSCMessage"));
}

void onCCMessage(OSCMessage &msg, int addrOffset) {
  // DEBUG_LOG("onCCMessage: ");
  // DEBUG_LOG(msg.getInt(0)); //ch
  // DEBUG_LOG(" / ");
  // DEBUG_LOG(msg.getInt(1)); //control_id
  // DEBUG_LOG(" / ");
  // DEBUG_LOGLN(msg.getInt(2)); //val
  // // USER_MODE_1.checkLeds(msg.getInt(0), msg.getInt(1), msg.getInt(2));
  // // activePage.checkLeds(msg.getInt(0), msg.getInt(1), msg.getInt(2));
  // // if (msg.getInt(1) == 117) updateProgressBar( msg.getInt(2));
  // // else 
  // activePage.checkLeds(msg.getInt(0)-1, msg.getInt(1), msg.getInt(2));
}

void onSysexMessage(OSCMessage &msg, int addrOffset) {
  // int len = msg.size();
  // uint8_t sysex[len];
  // for(int i=0; i<len; i++){
  //   sysex[i] = msg.getInt(i);
  // }
  // onSysEx(sysex, len);
}

void onSerialMessage(OSCMessage &msg, int addrOffset) {
  // char strBuf[MAX_SONG_NAME];
  // msg.getString(0, strBuf, MAX_SONG_NAME);
  // int numPage = msg.getInt(1);
  // DEBUG_LOGLN(strBuf);
  // if (strcmp(strBuf, "GET_CONFIG") == 0) jsonManager.sendJSONConfigOSC();
  // else if (strcmp(strBuf, "GET_PAGE") == 0) jsonManager.sendJSONPageOSC(numPage);
}


void onDisplayMessage(OSCMessage &msg, int addrOffset) {
  // char strBuf[MAX_SONG_NAME];
  // msg.getString(1, strBuf, MAX_SONG_NAME);
  // parseDisplayItem(indexOfItem(displayedItemsArray[msg.getInt(0)]), strBuf, msg.getInt(2));
}

void onExMIDI(OSCMessage &msg, int addrOffset) {
  // DEBUG_LOG("onExMIDI: ");
  // DEBUG_LOG(msg.getInt(0));
  // DEBUG_LOG(msg.getInt(1));
  // DEBUG_LOGLN(msg.getInt(2));
  // int32_t packet[4] = {1, msg.getInt(0), msg.getInt(1), msg.getInt(2)};
  // // int32_t data = createUint32FromBytes(packet);
  // // write_to_other_core(data);
}

void receiveOSCMsg(char* _packetBuffer, int packetSize) {
  OSCMessage msg;
  
  // CORRECTION : Utiliser la taille du paquet UDP au lieu de chercher '\0'
  for (int i = 0; i < packetSize; i++) {
    msg.fill((uint8_t)_packetBuffer[i]);
  }

  if (!msg.hasError()) {
    String address = msg.getAddress();
    
    // DIAGNOSTIC : Détecter les adresses suspectes avec indexOf au lieu de contains
    if (address.endsWith("$") || address.indexOf("$") >= 0) {
      DEBUG_LOG("*** TRUNCATED ADDRESS DETECTED: '");
      DEBUG_LOG(address);
      DEBUG_LOG("' | Raw packet bytes: ");
      for (int i = 0; i < min(32, packetSize); i++) {
        uint8_t byteVal = (uint8_t)_packetBuffer[i];
        if (byteVal >= 32 && byteVal <= 126) {
          DEBUG_LOG((char)byteVal);  // Caractère lisible
        } else {
          DEBUG_LOG("[");
          DEBUG_LOG(String(byteVal, HEX));
          DEBUG_LOG("]");
        }
      }
      DEBUG_LOGLN("");
      return; // Ignorer les paquets corrompus
    }
    
    DEBUG_LOG("OSC Message Received from: ");
    DEBUG_LOG(showcontrolUdp.remoteIP());
    DEBUG_LOG(" / ");
    DEBUG_LOG(showcontrolUdp.remotePort());
    DEBUG_LOG(" : ");
    DEBUG_LOG(msg.getAddress());
    DEBUG_LOG(" | UDP size: ");
    DEBUG_LOG(packetSize);
    DEBUG_LOG(" | OSC size: ");
    DEBUG_LOGLN(msg.size());

    // Filtrage pendant les séquences critiques
    if (settings.isRunning) {
      // Incrémenter le compteur de messages de séquence
      static int sequenceMessageCount = 0;
      sequenceMessageCount++;
      DEBUG_LOG("*** SEQUENCE MSG #");
      DEBUG_LOG(sequenceMessageCount);
      DEBUG_LOG(" *** ");
      DEBUG_LOG(address);
      DEBUG_LOG(" from ");
      DEBUG_LOG(showcontrolUdp.remoteIP());
      DEBUG_LOGLN("");
      
      // CORRECTION : Ajouter /heartbeat et /subscribed aux messages critiques
      bool isCriticalMessage = (address.startsWith("/global/") || 
                               address.startsWith("/audioInterfaces/") || 
                               address.startsWith("/setlist/") ||
                               address.equals("/heartbeat") ||
                               address.equals("/subscribed"));
      
      if (!isCriticalMessage) {
        DEBUG_LOG("FILTERING: Ignoring non-critical message during sequence: ");
        DEBUG_LOGLN(address);
        return;
      }
      else {
        DEBUG_LOG("CRITICAL: Processing essential Ableset message: ");
        DEBUG_LOGLN(address);
        
        // CORRECTION : Supprimer l'appel à trackMessage car la méthode n'existe pas
        // La séquence se terminera automatiquement avec onRemainingTimeInSet
      }
    }

    // Routes OSC existantes...
    msg.route("/global/beatsPosition", onBeatsPosition);
    msg.route("/global/tempo", onTempo);
    msg.route("/global/isPlaying", onIsPlaying);
    msg.route("/global/timeSignature", onSignature);
    msg.route("/global/humanPosition", [](OSCMessage &msg, int addrOffset) {
      DEBUG_LOGLN("Human position received");
    });
    msg.route("/global/isLoadingFile", [](OSCMessage &msg, int addrOffset) {
      DEBUG_LOGLN("Ableset loading file");
    });
    msg.route("/global/isConnected", [](OSCMessage &msg, int addrOffset) {
      DEBUG_LOGLN("Ableset connected");
    });
    msg.route("/global/isRecording", [](OSCMessage &msg, int addrOffset) {
      DEBUG_LOGLN("Ableset recording state");
    });
    msg.route("/global/isSyncingToRemoteTick", [](OSCMessage &msg, int addrOffset) {
      DEBUG_LOGLN("Ableset sync state");
    });
    msg.route("/global/quantization", [](OSCMessage &msg, int addrOffset) {
      DEBUG_LOGLN("Ableset quantization");
    });
    
    msg.route("/audioInterfaces/connected", [](OSCMessage &msg, int addrOffset) {
      DEBUG_LOGLN("Audio interfaces connected");
    });
    msg.route("/audioInterfaces/count", [](OSCMessage &msg, int addrOffset) {
      DEBUG_LOGLN("Audio interfaces count");
    });
    msg.route("/audioInterfaces/names", [](OSCMessage &msg, int addrOffset) {
      DEBUG_LOGLN("Audio interfaces names");
    });
    msg.route("/audioInterfaces/scenes", [](OSCMessage &msg, int addrOffset) {
      DEBUG_LOGLN("Audio interfaces scenes");
    });
    
    msg.route("/setlist/loopEnabled", onLoopEnabled);
    msg.route("/setlist/name", onSetlistName);
    msg.route("/setlist/songs", onSongs);
    msg.route("/setlist/songColors", onSongColors);
    msg.route("/setlist/songDurations", onSongDurations);
    msg.route("/setlist/sections", onSections);
    msg.route("/setlist/sectionColors", [](OSCMessage &msg, int addrOffset) {
      DEBUG_LOGLN("Section colors received");
    });
    msg.route("/setlist/activeSongName", onActiveSongName);
    msg.route("/setlist/activeSongColor", onActiveSongColor);
    msg.route("/setlist/activeSongIndex", onActiveSongIndex);
    msg.route("/setlist/activeSongStart", onActiveSongStart);
    msg.route("/setlist/activeSongEnd", onActiveSongEnd);
    msg.route("/setlist/activeSongDuration", onActiveSongDuration);
    msg.route("/setlist/activeSongProgress", onActiveSongProgress);
    msg.route("/setlist/activeSectionName", onActiveSectionName);
    msg.route("/setlist/activeSectionIndex", onActiveSectionIndex);
    msg.route("/setlist/nextSongName", onNextSongName);
    msg.route("/setlist/nextSectionName", [](OSCMessage &msg, int addrOffset) {
      DEBUG_LOGLN("Next section name received");
    });
    msg.route("/setlist/remainingTimeInSong", onRemainingTimeInSong);
    msg.route("/setlist/remainingTimeInSet", onRemainingTimeInSet);
    
    msg.route("/subscribed", onSubscribed);
    msg.route("/heartbeat", onHeartbeat);

  } else {
    DEBUG_LOG("Error OSC message - UDP size: ");
    DEBUG_LOG(packetSize);
    DEBUG_LOG(" | Error code: ");
    DEBUG_LOGLN(msg.getError());
  }

}
