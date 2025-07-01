#include "OSCReceive.h"
#include "../core/mainPageParser.h"
#include "../core/settingsParser.h"
#include "../core/globalParser.h"
#include "../display/colors.h"


// ACTIVE SONG

void onActiveSongName(OSCMessage &msg, int addrOffset) {
  char strBuf[128];  // Buffer plus grand pour accepter n'importe quelle chaîne
  msg.getString(0, strBuf, 128);
  _main.setActiveSongName(strBuf);
}

void onActiveSongColor(OSCMessage &msg, int addrOffset) {
  char strBuf[32];
  msg.getString(0, strBuf, 32);
  // ETHERNET_DEBUG_LOG("onActiveSongColor OSC : ");
  // ETHERNET_DEBUG_LOGLN(strBuf);
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
  // ETHERNET_DEBUG_LOG(F("activeSectionIndex changed: "));
  // ETHERNET_DEBUG_LOGLN(activeSectionIndex);
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
  // ETHERNET_DEBUG_LOG("onActiveSectionStart: ");
  // ETHERNET_DEBUG_LOGLN(_main.activeSectionStart);
}

void onActiveSectionEnd(OSCMessage &msg, int addrOffset) {
  // _main.activeSectionEnd = msg.getFloat(0);
  // ETHERNET_DEBUG_LOG("_main.activeSectionEnd: ");
  // ETHERNET_DEBUG_LOGLN(_main.activeSectionEnd);
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
  // ETHERNET_DEBUG_LOG("global.beatsPosition: ");
  // ETHERNET_DEBUG_LOGLN(global.beatsPosition);
  global.beatsPosition = msg.getFloat(0);
  global.current_bar = int(global.beatsPosition / 4) + 1;
  global.current_beat = (int)global.beatsPosition % 4 + 1;
  global.setCurrentBar(global.current_bar);
  global.setCurrentBeat(global.current_beat);
  // ETHERNET_DEBUG_LOG("global.current_bar: ");
  // ETHERNET_DEBUG_LOG(global.current_bar);
  // ETHERNET_DEBUG_LOG(" | global.current_beat: ");
  // ETHERNET_DEBUG_LOGLN(global.current_beat);
  // globalPage.showCounter();
  // updateProgressBar();
  // showProgressBarMillis = millis();
  // showProgressBarLatch = true;
}

void onIsPlaying(OSCMessage &msg, int addrOffset) {
  global.isPlaying = msg.getInt(0);
  ETHERNET_DEBUG_LOG("global.isPlaying: ");
  ETHERNET_DEBUG_LOGLN(global.isPlaying);
  global.setIsPlaying(global.isPlaying);
}

void onAudioInterfaceScene(OSCMessage &msg, int addrOffset) {
  // settings.audioInterfaceScene = msg.getInt(0);
  // globalPage.showAudioInterfaceScene();
}

void onLoopEnabled(OSCMessage &msg, int addrOffset) {
  // ETHERNET_DEBUG_LOG("global.loopEnabled ");
  // global.loopEnabled = msg.getInt(0);
  // ETHERNET_DEBUG_LOG(global.loopEnabled);
  // if(activePage == pages[0]){
  // if (global.loopEnabled > 0) l[5].show_direct_color(255, 100, 0);
  // else l[5].led_off();
  // }
}

void onLoopStart(OSCMessage &msg, int addrOffset) {
  // ETHERNET_DEBUG_LOG("onLoopStart ");
  // global.loopStart = msg.getFloat(0);
  // ETHERNET_DEBUG_LOG(global.loopEnabled);
  // // showActiveSongProgressBar();
}

void onLoopEnd(OSCMessage &msg, int addrOffset) {
  // ETHERNET_DEBUG_LOG("global.loopEnabled ");
  // global.loopEnd = msg.getFloat(0);
  // ETHERNET_DEBUG_LOG(global.loopEnabled);
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
  // int size = msg.size();
  // ETHERNET_DEBUG_LOG("onSections size: ");
  // ETHERNET_DEBUG_LOGLN(size);
  // for (int j = 0; j < size; j++) {
  //   char strBuf[MAX_SONG_NAME];
  //   msg.getString(j, strBuf, MAX_SONG_NAME);
  //   strcpy(_main.sections[j], strBuf);
  //   ETHERNET_DEBUG_LOG("_main.sections: ");
  //   ETHERNET_DEBUG_LOGLN(_main.sections[j]);
  // }
}

void onSongs(OSCMessage &msg, int addrOffset) {
  // int size = msg.size();
  // ETHERNET_DEBUG_LOG("songs size: ");
  // ETHERNET_DEBUG_LOGLN(size);
  // _main. songsListSize = size;
  // for (int j = 0; j < size; j++) {
  //   char strBuf[MAX_SONG_NAME];
  //   msg.getString(j, strBuf, MAX_SONG_NAME);
  //   for (int i = 0; i < MAX_SONG_NAME; i++) {
  //     _main.songsList[j][i] = strBuf[i];
  //   }
  //   ETHERNET_DEBUG_LOG("songs: ");
  //   ETHERNET_DEBUG_LOGLN(_main.songsList[j]);
  // }
  // mainPage.showSongsCounter();
}

void onSongDurations(OSCMessage &msg, int addrOffset) {
  // int size = msg.size();
  // ETHERNET_DEBUG_LOG("songs durations: ");
  // ETHERNET_DEBUG_LOGLN(size);
  // for (int j = 0; j < size; j++) {
  //   _main.songDurations[j] = msg.getInt(j);
  //   ETHERNET_DEBUG_LOG("durations: ");
  //   ETHERNET_DEBUG_LOGLN(_main.songDurations[j]);
  // }
}



void onSongColors(OSCMessage &msg, int addrOffset) {
  // int size = msg.size();
  // ETHERNET_DEBUG_LOG("colors size: ");
  // ETHERNET_DEBUG_LOGLN(size);
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
  // //   ETHERNET_DEBUG_LOG("colors: ");
  // //   ETHERNET_DEBUG_LOGLN(songColors[j]);
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
  ETHERNET_DEBUG_LOGLN("Subscribed to Ableset");
  // char strBuf[32];
  // ETHERNET_DEBUG_LOGLN(msg.getString(0, strBuf, 32));
}

void onHeartbeat(OSCMessage &msg, int addrOffset) {    
  ETHERNET_DEBUG_LOGLN("Heartbeat received");
  for (int i = 0; i < ethernet.totalServiceCount; i++) { // Iterate through all discovered services
      if (ethernet.discoveredServices[i].serviceType == ABLESETSRV && 
          ethernet.discoveredServices[i].ip == showcontrolUdp.remoteIP()) {
        ethernet.discoveredServices[i].lastSeen = millis();
        if(!ethernet.discoveredServices[i].subscribed){
          ETHERNET_DEBUG_LOGLN("Service already subscribed, updating last seen");
            ethernet.discoveredServices[i].subscribed = true; // Mark the service as subscribed
          settings.getItStarted(); // Trigger the handshake response to update the UI
        }
        break; // Exit the loop early since IPs are unique
      }
      else{
        ETHERNET_DEBUG_LOG("Service not found, adding service: ");
        ETHERNET_DEBUG_LOGLN(showcontrolUdp.remoteIP());
        ethernet.addService(ABLESETSRV, showcontrolUdp.remoteIP(), showcontrolUdp.remotePort());
        settings.getItStarted();
      }
    }
}

void onError(OSCMessage &msg, int addrOffset) {
  ETHERNET_DEBUG_LOGLN(msg.getType(0));
}

void onLivePing(OSCMessage &msg, int addrOffset) {
  // ETHERNET_DEBUG_LOGLN(("Live Ping Received"));
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
  // ETHERNET_DEBUG_LOGLN(("App Ping Received"));
  //   for (int i = 0; i < ethernet.totalServiceCount; i++) { // Iterate through all discovered services
  //     if (ethernet.discoveredServices[i].serviceType == SHOWCONTROLSRV && 
  //         ethernet.discoveredServices[i].ip == showcontrolUdp.remoteIP()) {
  //       ethernet.discoveredServices[i].lastSeen = millis();
  //       break; // Exit the loop early since IPs are unique
  //     }
  //   }
}

void onMessage(OSCMessage &msg, int addrOffset) {
  ETHERNET_DEBUG_LOGLN(("OSCMessage"));
}

void onCCMessage(OSCMessage &msg, int addrOffset) {
  // ETHERNET_DEBUG_LOG("onCCMessage: ");
  // ETHERNET_DEBUG_LOG(msg.getInt(0)); //ch
  // ETHERNET_DEBUG_LOG(" / ");
  // ETHERNET_DEBUG_LOG(msg.getInt(1)); //control_id
  // ETHERNET_DEBUG_LOG(" / ");
  // ETHERNET_DEBUG_LOGLN(msg.getInt(2)); //val
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
  // ETHERNET_DEBUG_LOGLN(strBuf);
  // if (strcmp(strBuf, "GET_CONFIG") == 0) jsonManager.sendJSONConfigOSC();
  // else if (strcmp(strBuf, "GET_PAGE") == 0) jsonManager.sendJSONPageOSC(numPage);
}


void onDisplayMessage(OSCMessage &msg, int addrOffset) {
  // char strBuf[MAX_SONG_NAME];
  // msg.getString(1, strBuf, MAX_SONG_NAME);
  // parseDisplayItem(indexOfItem(displayedItemsArray[msg.getInt(0)]), strBuf, msg.getInt(2));
}

void onExMIDI(OSCMessage &msg, int addrOffset) {
  // ETHERNET_DEBUG_LOG("onExMIDI: ");
  // ETHERNET_DEBUG_LOG(msg.getInt(0));
  // ETHERNET_DEBUG_LOG(msg.getInt(1));
  // ETHERNET_DEBUG_LOGLN(msg.getInt(2));
  // int32_t packet[4] = {1, msg.getInt(0), msg.getInt(1), msg.getInt(2)};
  // // int32_t data = createUint32FromBytes(packet);
  // // write_to_other_core(data);
}

void receiveOSCMsg() {
  // updateLastSeen(SHOWCONTROLSRV, showcontrolUdp.remoteIP());
  OSCMessage msg;
  while (ethernet. showcontrolPacketSize--) msg.fill(showcontrolUdp.read());

  if (!msg.hasError()) {
    ETHERNET_DEBUG_LOG("OSC Packet Size: ");
    ETHERNET_DEBUG_LOGLN(ethernet. showcontrolPacketSize);
    ETHERNET_DEBUG_LOG("OSC Message Received: ");
    ETHERNET_DEBUG_LOG(msg.getAddress());
    ETHERNET_DEBUG_LOG(" From: ");
    ETHERNET_DEBUG_LOG(showcontrolUdp.remoteIP());
    ETHERNET_DEBUG_LOG(":");
    ETHERNET_DEBUG_LOGLN(showcontrolUdp.remotePort());

    msg.route("/fromShowcontrolApp/serialMessage", onSerialMessage);
    msg.route("/fromShowcontrolApp/sysex", onSysexMessage);
    msg.route("/fromShowcontrolApp/pong", onSCPong);
    msg.route("/fromLive/selectedSongName", onActiveSongName);
    msg.route("/fromLive/selectedSectionName", onActiveSectionName);
    msg.route("/fromLive/nextSongName", onNextSongName);
    msg.route("/fromLive/control_id", onCCMessage);
    msg.route("/fromLive/sysex", onSysexMessage);
    msg.route("/fromLive/displayMessage", onDisplayMessage);
    // msg.route("/fromLive/display", onDisplayMessage);
    msg.route("/fromLive/pong", onLivePing);
    msg.route("/extMIDI", onExMIDI);


    // ABLESET MESSAGES

    msg.route("/global/beatsPosition", onBeatsPosition);
    msg.route("/global/tempo", onTempo);
    msg.route("/global/isPlaying", onIsPlaying);
    msg.route("/setlist/loopEnabled", onLoopEnabled);
    msg.route("/subscribed", onSubscribed);
    // msg.route("/heartbeat", onHeartbeat);
    msg.route("/setlist/name", onSetlistName);
    msg.route("/setlist/activeSongName", onActiveSongName);
    msg.route("/setlist/activeSongColor", onActiveSongColor);
    msg.route("/setlist/activeSongIndex", onActiveSongIndex);
    msg.route("/setlist/nextSongName", onNextSongName);
    msg.route("/setlist/songDurations", onSongDurations);
    msg.route("/setlist/activeSectionName", onActiveSectionName);
    msg.route("/setlist/activeSectionIndex", onActiveSectionIndex);
    msg.route("/setlist/songs", onSongs);
    msg.route("/setlist/sections", onSections);
    // msg.route("/audioInterfaces/all/scene", onAudioInterfaceScene);

    msg.route("/setlist/remainingTimeInSet", onRemainingTimeInSet);
    msg.route("/setlist/remainingTimeInSong", onRemainingTimeInSong);
    msg.route("/setlist/nextSongName", onActiveSongStart);
    msg.route("/setlist/activeSongEnd", onActiveSongEnd);
    msg.route("/setlist/activeSongDuration", onActiveSongDuration);
    msg.route("/setlist/activeSongProgress", onActiveSongProgress);



    // msg.route("/fromLive/nextSongColor", onNextSongColor);
    // // /setlist/global.loopStart [0|1] the loop's start position in beats
    // msg.route("/fromLive/global.loopStart", onLoopStart);
    // // /setlist/global.loopEnd [0|1] the loop's end position in beats
    // msg.route("/fromLive/global.loopEnd", onLoopEnd);
    // // /setlist/_main.activeSectionName [string] the name of the current section
    // msg.route("/fromLive/_main.activeSectionName", onActiveSectionName);
    // // /setlist/activeSectionIndex [number] the index of the current section in the current song
    // msg.route("/fromLive/activeSectionIndex", onActiveSectionIndex);
    // // /setlist/_main.activeSectionStart [number] the section's start position in beats
    // msg.route("/fromLive/_main.activeSectionStart", onActiveSectionStart);
    // // /setlist/_main.activeSectionEnd [number] the section's end position in beats
    // msg.route("/fromLive/_main.activeSectionEnd", onActiveSectionEnd);
    // // /setlist/activeSectionColor [string] [string] the color of the section, as a color name and as a hex string
    // msg.route("/fromLive/activeSectionColor", onActiveSectionColor);
    // // /setlist/_main.sections [...string] a list of all _main.sections of the current song
    // msg.route("/fromLive/_main.sections", onSections);
    // // /setlist/songs [...string] a list of all song names in the setlist
    // msg.route("/fromLive/songs", onSongs);
    // msg.route("/fromLive/songColors", onSongColors);


  } else {
    ETHERNET_DEBUG_LOGLN("Error OSC message");
    msg.route("/error", onError);
  }
}
