#include "OSCManager.h"
#include <OSCBundle.h>

// Déclarations des callbacks OSC
void onActiveSongName(OSCMessage &msg, int addrOffset);
void onActiveSongColor(OSCMessage &msg, int addrOffset);
void onActiveSongStart(OSCMessage &msg, int addrOffset);
void onActiveSongEnd(OSCMessage &msg, int addrOffset);
void onActiveSongIndex(OSCMessage &msg, int addrOffset);
void onActiveSongDuration(OSCMessage &msg, int addrOffset);
void onActiveSongProgress(OSCMessage &msg, int addrOffset);
void onRemainingTimeInSet(OSCMessage &msg, int addrOffset);
void onRemainingTimeInSong(OSCMessage &msg, int addrOffset);

void onNextSongName(OSCMessage &msg, int addrOffset);
void onNextSongColor(OSCMessage &msg, int addrOffset);

void onActiveSectionName(OSCMessage &msg, int addrOffset);
void onActiveSectionIndex(OSCMessage &msg, int addrOffset);
void onActiveSectionStart(OSCMessage &msg, int addrOffset);
void onActiveSectionEnd(OSCMessage &msg, int addrOffset);
void onActiveSectionColor(OSCMessage &msg, int addrOffset);

void onSetlistName(OSCMessage &msg, int addrOffset);
void onBeatsPosition(OSCMessage &msg, int addrOffset);
void onIsPlaying(OSCMessage &msg, int addrOffset);
void onAudioInterfaceScene(OSCMessage &msg, int addrOffset);
void onLoopEnabled(OSCMessage &msg, int addrOffset);
void onLoopStart(OSCMessage &msg, int addrOffset);
void onLoopEnd(OSCMessage &msg, int addrOffset);
void onTempo(OSCMessage &msg, int addrOffset);
void onSignature(OSCMessage &msg, int addrOffset);

void onSections(OSCMessage &msg, int addrOffset);
void onSongs(OSCMessage &msg, int addrOffset);
void onSongDurations(OSCMessage &msg, int addrOffset);
void onSongColors(OSCMessage &msg, int addrOffset);

void onSubscribed(OSCMessage &msg, int addrOffset);
void onHeartbeat(OSCMessage &msg, int addrOffset);
void onError(OSCMessage &msg, int addrOffset);

// SHOW CONTROL OSC

void onLivePing(OSCMessage &msg, int addrOffset);
void onSCPong(OSCMessage &msg, int addrOffset);
void onMessage(OSCMessage &msg, int addrOffset);
void onCCMessage(OSCMessage &msg, int addrOffset);
void onSysexMessage(OSCMessage &msg, int addrOffset);
void onSerialMessage(OSCMessage &msg, int addrOffset);
void onDisplayMessage(OSCMessage &msg, int addrOffset);
void onExMIDI(OSCMessage &msg, int addrOffset);
void receiveOSCMsg();