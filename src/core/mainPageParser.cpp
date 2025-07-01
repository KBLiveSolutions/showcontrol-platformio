#include <Arduino.h>
#include "../config/jsonManager.h"

#include "utils.h"
#include "pages.h"
#include "settingsParser.h"
#include "mainPageParser.h"

#include "../display/pages/mainPage.h"
#include "../display/pages/globalPage.h"
#include "../display/pages/menuPage.h"
#include "../display/colors.h"

#include "../midi/midi_out.h"
#include "../osc/OSCManager.h"

Main _main;

// Fonction utilitaire optimisée pour tronquer les chaînes avec un point
void truncateStringWithDot(char* destination, const char* source, size_t maxLength) {
  if (!source || !destination || maxLength == 0) {
    if (destination) destination[0] = '\0';
    return;
  }
  
  size_t len = strlen(source);
  
  if (len <= maxLength) {
    // Copie directe si la chaîne tient dans la limite
    strcpy(destination, source);
  } else {
    // Troncature avec point si trop long
    strncpy(destination, source, maxLength - 1);
    destination[maxLength - 1] = '.';
    destination[maxLength] = '\0';
  }
}

void Main::sendSetlistArrayRequest(){
  static uint8_t sysex_msg[7] = { 240, 122, 29, 1, 19, 14, 247 };
  midi::sendSysexToDAW(sysex_msg, 7);
  sendOSCSysex(sysex_msg, 7);
}

void Main::sendSongArrayRequest(){
  static uint8_t sysex_msg[7] = { 240, 122, 29, 1, 19, 15, 247 };
  midi::sendSysexToDAW(sysex_msg, 7);
  sendOSCSysex(sysex_msg, 7);
}

void Main::setActiveSongName(char* songName) {
  DEBUG_LOG_VALUE("onActiveSongName: ", songName);
  
  // Limitation à 20 caractères avec point de troncature
  truncateStringWithDot(activeSongName, songName, 20);
  
  activePage.showActiveSongName();
}

void Main::setActiveSongColor(uint16_t color) {
  activeSongColor = color;
  DEBUG_LOG_VALUE("onActiveSongColor: ", activeSongColor);
  // activeSongColorShade = hexStringtoRGB565Shade(strBuf);
  activeSectionColor = activeSongColorShade;
  mainPage.updateProgressBarFine(true);
  mainPage.showRemainingTimeInSong(true);
}

void Main::setActiveSongStart(float time) {
  activeSongStart = time;
  activeSongEnd = 0;
  DEBUG_LOG_VALUE("onActiveSongStart: ", activeSongStart);
}

void Main::setActiveSongEnd(float time) {
  activeSongEnd = time;
  DEBUG_LOG_VALUE("activeSongEnd: ", activeSongEnd);
}

void Main::setActiveSongIndex(int index) {
  activeSongIndex = index;
  DEBUG_LOG_VALUE("activeSongIndex changed: ", activeSongIndex);
  mainPage.showSongsCounter(true);
}

void Main::setActiveSongDuration(float duration) {
  activeSongDuration = duration;
  DEBUG_LOG_VALUE("activeSongDuration changed: ", activeSongDuration);
  mainPage.showRemainingTimeInSong(true);
}

void Main::setActiveSongProgress(float progress) {
  songPercentage = progress;
  mainPage.updateProgressBarFine(true);
}

void Main::setRemainingTimeInSet(float time) {
  remainingTimeInSet = time;
  DEBUG_LOG_VALUE("remainingTimeInSet changed: ", remainingTimeInSet);
  mainPage.showRemainingTimeInSet(true);
}

void Main::setRemainingTimeInSong(float time) {
  remainingTimeInSong = time + 1;
  DEBUG_LOG_VALUE("remainingTimeInSong changed: ", remainingTimeInSong);
  currentSeconds = remainingTimeInSong;
  if(currentSeconds != previousSeconds) {
    mainPage.showRemainingTimeInSong(true);
    previousSeconds = currentSeconds;
  }
}
void Main::setNextSongName(char* name) {
    // Limitation à 20 caractères avec point de troncature
    truncateStringWithDot(nextSongName, name, 20);
    
    DEBUG_LOG_VALUE("onNextSongName: ", nextSongName);
    activePage.showNextSongName();
}

void Main::setNextSongColor(uint16_t color) {
  nextSongColor = color;
  DEBUG_LOG_VALUE("onNextSongColor: ", nextSongColor);
  // activePage.showNextSongColor();
}

void Main::setActiveSectionName(char* name) {
  // Limitation à 20 caractères avec point de troncature
  truncateStringWithDot(activeSectionName, name, 20);
  
  DEBUG_LOG_VALUE("onActiveSectionName: ", activeSectionName);
  activePage.showActiveSectionName();
}

void Main::setActiveSectionIndex(int index) {
  activeSectionIndex = index;
  DEBUG_LOG_VALUE("activeSectionIndex changed: ", activeSectionIndex);
  // activePage.showActiveSectionIndex();
}
void Main::setActiveSectionStart(float time) {
  activeSectionStart = time;
  activeSectionEnd = 0;
  // DEBUG_LOG_VALUE("onActiveSectionStart changed: ", onActiveSectionStart);
}
void Main::setActiveSectionEnd(float time) {
  activeSectionEnd = time;
  DEBUG_LOG_VALUE("activeSectionEnd changed: ", activeSectionEnd);
}
void Main::setActiveSectionColor(uint16_t color) {
  activeSectionColor = color;
  // DEBUG_LOG_VALUE("activeSectionColor changed: ", activeSectionColor);
  // activePage.showActiveSectionColor(activeSectionColor);
}
void Main::setSections(char sections[][MAX_SONG_NAME], int count) {
  // for (int i = 0; i < count; i++) {
  //   strcpy(activePage.sections[i], sections[i]);
  // DEBUG_LOG_VALUE("sections changed: ", activePage.sections[i]);
  // }
  // activePage.showSections(activePage.sections, count);
}
void Main::setSongs(char songs[][MAX_SONG_NAME], int count) {
  // songsListSize = count;
  // for (int i = 0; i < count; i++) {
  //   strcpy(songsList[i], songs[i]);
  // DEBUG_LOG_VALUE("songs changed: ", songsList[i]);
  // }
  // activePage.showSongs(songsList, count);
}
void Main::setSongDurations(int durations[], int count) {
  // for (int i = 0; i < count; i++) {
  //   songDurations[i] = durations[i];
  // DEBUG_LOG_VALUE("durations changed: ", songDurations);
  // }
  // activePage.showSongDurations(songDurations, count);
}
void Main::setSongColors(char colors[][MAX_SONG_NAME], int count) {
  // for (int i = 0; i < count; i++) {
  //   strcpy(songColors[i], colors[i]);
  //   DEBUG_LOG_VALUE("Song colors changed: ", songColors[i]);
  // }
  // activePage.showSongColors(songColors, count);
}


void Main::setCCReceived(uint8_t channel, uint8_t control, uint8_t value) {
  DEBUG_LOG("CC Received: ");
  DEBUG_LOG(control);
  DEBUG_LOG(" Value: ");
  DEBUG_LOGLN(value);
  activePage.checkLeds(channel, control, value);
}


void Main::configureButton(uint8_t user_mode, uint8_t controlNum, uint8_t control_type, uint8_t control_cc, uint8_t control_ch, uint8_t isCustom, uint8_t toggled){
  // Validation des paramètres d'entrée
  if (user_mode >= 5 || controlNum >= 5 || control_type >= 4) {
    return; // Paramètres invalides, sortie anticipée
  }
  
  user_mode++; // Décalage d'index
  pages[user_mode].setButtonControl(controlNum, controlType[control_type], control_cc, control_ch, isCustom, toggled);
  jsonManager.writeJSONControl(user_mode, controlNum, control_type, control_cc, control_ch, isCustom, toggled);
}

// void Main::configurePedal(uint8_t user_mode, uint8_t controlNum, uint8_t control_type, uint8_t control_cc, uint8_t control_ch, uint8_t isCustom, uint8_t toggled){
//   pages[user_mode].setPedal(controlNum, controlType[control_type], control_cc, control_ch, isCustom, toggled);
//   jsonManager.writeJSONPedal(user_mode, controlNum, control_type, control_cc, control_ch, isCustom, toggled);
//   if(user_mode == selectedMode) {
//     mainPage.showButtonSprite(false, controlNum, getActionName(user_mode, controlNum), getActionColor(user_mode, controlNum));
//   }
// }

// void Main::configure_short_buttons_with_shift(uint8_t user_mode, uint8_t controlNum, uint8_t control_cc, uint8_t control_ch){
//   pages[user_mode].setButtonControlShifted(controlNum, CC, control_cc, control_ch);
// }


void Main::configureLed(uint8_t user_mode, uint8_t controlNum, uint8_t control_type, uint8_t control_led_cc, uint8_t control_led_ch){
  // Validation des paramètres d'entrée
  if (user_mode >= 5 || controlNum >= 5) {
    return; // Paramètres invalides, sortie anticipée
  }
  
  user_mode++; // Décalage d'index
  pages[user_mode].setLedControl(controlNum, CC, control_led_cc, control_led_ch);
}

void Main::configureDisplay(uint8_t user_mode, uint8_t display_num, uint8_t display_type) {
  // Validation des paramètres d'entrée
  if (user_mode >= 5 || display_num >= 3) {
    return; // Paramètres invalides, sortie anticipée
  }
  
  user_mode++; // Décalage d'index
  pages[user_mode].setDisplay(display_num, display_type);
  jsonManager.writeJSONDisplay(user_mode, display_num, display_type);
}


// Fonction optimisée pour trouver l'index d'un élément
int indexOfItem(displayed_item_t itemToFind){
  // Utilisation de la taille réelle du tableau au lieu de sizeof()
  constexpr size_t arraySize = sizeof(displayedItemsArray) / sizeof(displayedItemsArray[0]);
  
  for (size_t i = 0; i < arraySize; ++i) {
    if (displayedItemsArray[i] == itemToFind) {
      return static_cast<int>(i);
    }
  }
  return -1; // Élément non trouvé
}


void Main::parseArrayItem(uint8_t itemType, char* strBuf, uint8_t listIndex, uint8_t listLength) {
  displayed_item_t displayedItem = displayedItemsArray[itemType];

  // switch (displayedItem) {
  //   case Setlist_Array_Item:
  //     _main.setlistsListSize = listLength;
  //     for (int i = 0; i < MAX_SONG_NAME; i++) {
  //       _main.setlistsList[listIndex][i] = strBuf[i];
  //     }
  //   //     ETHERNET_DEBUG_LOG("setlist: ");
  //   //     ETHERNET_DEBUG_LOG(listIndex);
  //   //     ETHERNET_DEBUG_LOG(" / ");
  //   //     ETHERNET_DEBUG_LOG(listLength);
  //   //     ETHERNET_DEBUG_LOG(" : ");
  //   //     ETHERNET_DEBUG_LOGLN(mainPage.setlistsList[listIndex]);
  //     menuPage.createMenu(SETLIST_MENU, 0);
  //     break;

  //   case Song_Array_Item:
  //     _main.songsListSize = listLength;
  //     for (int i = 0; i < MAX_SONG_NAME; i++) {
  //       _main.songsList[listIndex][i] = strBuf[i];
  //     }
  //   //   ETHERNET_DEBUG_LOG("songs: ");
  //   //   ETHERNET_DEBUG_LOGLN(mainPage.songsList[listIndex]);
  //     menuPage.createMenu(SONG_MENU, _main.activeSongIndex);
  //     break;

  //   default:
  //     // Optionnel : Gérer les cas non prévus
  //   //   ETHERNET_DEBUG_LOGLN("Unknown itemType");
  //     break;
  // }
}