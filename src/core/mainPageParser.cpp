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

#include "../leds/leds.h"
#include "../midi/midi_out.h"
#include "../osc/OSCManager.h"

Main mainParser;

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

void Main::onNewSetLoaded(){
    setActiveSongName(" ");
    setActiveSectionName(" ");
    setNextSongName(" ");
    getAblesetValues();
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

void Main::setSceneName(char* _sceneName){
  strncpy(sceneName, _sceneName, sizeof(sceneName) - 1);
  sceneName[sizeof(sceneName) - 1] = '\0'; // Toujours terminer la chaîne
  DEBUG_LOG_VALUE("onSceneName: ", sceneName);
  activePage->showSceneName();
}

void Main::setTrackName(char* _trackName){
  strncpy(trackName, _trackName, sizeof(trackName) - 1);
  trackName[sizeof(trackName) - 1] = '\0';
  activePage->showTrackName();
}

void Main::setActiveSongName(char* songName) {
  DEBUG_LOG_VALUE("onActiveSongName: ", songName);

  // Extraction du texte entre parenthèses
  char infoMsg[32] = "";
  char cleanName[64];
  if (songName) {
    const char* open = strchr(songName, '(');
    const char* close = strchr(songName, ')');
    if (open && close && close > open) {
      size_t infoLen = close - open - 1;
      if (infoLen > 0 && infoLen < sizeof(infoMsg)) {
        strncpy(infoMsg, open + 1, infoLen);
        infoMsg[infoLen] = '\0';
        setInformationMessage(infoMsg, true);
      }
      // Copier le nom sans la partie entre ()
      size_t prefixLen = open - songName;
      strncpy(cleanName, songName, prefixLen);
      cleanName[prefixLen] = '\0';
      // Ajouter la partie après )
      strncat(cleanName, close + 1, sizeof(cleanName) - strlen(cleanName) - 1);
      // Nettoyer les espaces superflus
      size_t len = strlen(cleanName);
      while (len > 0 && (cleanName[len-1] == ' ' || cleanName[len-1] == '-')) {
        cleanName[len-1] = '\0';
        len--;
      }
      truncateStringWithDot(activeSongName, cleanName, 16);
    } 
    else {
      truncateStringWithDot(activeSongName, songName, 16);
      setInformationMessage("empty", false);
    }
  } else {
    activeSongName[0] = '\0';
  }
  activePage->showActiveSongName();
  activeSongNameDisplayedOnce = true; // Marquer que le nom de la chanson a été affiché au moins une fois

  strcpy(activeSectionName, " ");
  DEBUG_LOG_VALUE("onActiveSectionName: ", activeSectionName);
  activePage->showActiveSectionName();
}


void Main::setActiveSongIndex(int index) {
  activeSongIndex = index;
  DEBUG_LOG_VALUE("activeSongIndex changed: ", activeSongIndex);
  // Validation des paramètres pour éviter les erreurs
  if (mainParser.songsListSize <= 0) {
    DEBUG_LOG_VALUE("setActiveSongIndex: invalid songsListSize: ", mainParser.songsListSize);
    return;
  }
  
  if (mainParser.activeSongIndex < 0) {
    DEBUG_LOG_VALUE("setActiveSongIndex: invalid activeSongIndex: ", mainParser.activeSongIndex);
    return;
  }
  activePage->updateSongIndex();
  // Contrôle des LEDs selon la position dans la liste
  // LED 3 (index 4 dans le tableau l[]) - contrôle "précédent"
}

void Main::setInformationMessage(const char* message, bool show) {
  if (!message || strlen(message) == 0) {
    DEBUG_LOGLN("setInformationMessage: empty message");
    return;
  }
  
  // Limitation à 32 caractères
  char infoMsg[32];
  strncpy(infoMsg, message, sizeof(infoMsg) - 1);
  infoMsg[sizeof(infoMsg) - 1] = '\0'; // Assurer la terminaison de chaîne
  
  mainPage.showInfoSprite(infoMsg, _White, show);
}

void Main::updateSongProgress(){
  if(activePage->pageType == SETLIST){
    // mainPage.updateProgressBarFine(true);
    mainPage.showRemainingTimeInSong(true);
  }
}

void Main::setActiveSongColor(uint16_t color) {
  activeSongColor = color;
  DEBUG_LOG_VALUE("onActiveSongColor: ", activeSongColor);
  // activeSongColorShade = hexStringtoRGB565Shade(strBuf);
  activeSectionColor = activeSongColorShade;
  updateSongProgress();
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

void Main::setActiveSongDuration(float duration) {
  activeSongDuration = duration;
  DEBUG_LOG_VALUE("activeSongDuration changed: ", activeSongDuration);
  updateSongProgress();
}

void Main::setRemainingTimeInSong(float time) {
  remainingTimeInSong = time + 1;
  DEBUG_LOG_VALUE("remainingTimeInSong changed: ", remainingTimeInSong);
  currentSeconds = remainingTimeInSong;
  // Calcul automatique de songPercentage
  if (activeSongDuration > 0 && remainingTimeInSong >= 0) {
    songPercentage = 1.0f - (remainingTimeInSong / activeSongDuration);
    if (songPercentage < 0) songPercentage = 0;
    if (songPercentage > 1) songPercentage = 1;
  } else {
    songPercentage = 0;
  }
  if(currentSeconds != previousSeconds) {
    if(activePage->pageType == SETLIST) mainPage.showRemainingTimeInSong(true);
      if(activePage->pageType == SETLIST) {
    // mainPage.updateProgressBarFine(true);
  } 
    previousSeconds = currentSeconds;
  }
}

void Main::setActiveSongProgress(float progress) {
  // songPercentage = progress;
  // if(activePage->pageType == SETLIST) {
  //   mainPage.updateProgressBarFine(true);
  // } 
}

void Main::setRemainingTimeInSet(float time) {
  remainingTimeInSet = time;
  DEBUG_LOG_VALUE("remainingTimeInSet changed: ", remainingTimeInSet);
  if(activePage->pageType == SETLIST) mainPage.showRemainingTimeInSet(true);
}

void Main::setNextSongName(char* name) {
    // Limitation à 20 caractères avec point de troncature
    truncateStringWithDot(nextSongName, name, 20);
    
    DEBUG_LOG_VALUE("onNextSongName: ", nextSongName);
    activePage->showNextSongName();
}

void Main::setNextSongColor(uint16_t color) {
  nextSongColor = color;
  DEBUG_LOG_VALUE("onNextSongColor: ", nextSongColor);
  // activePage->showNextSongColor();
}

void Main::setActiveSectionName(char* name) {
  // Si name est vide, afficher >>>
  if (!name || name[0] == '\0') {
    strncpy(activeSectionName, ">>>", sizeof(activeSectionName) - 1);
    activeSectionName[sizeof(activeSectionName) - 1] = '\0';
  } else {
    // Limitation à 20 caractères avec point de troncature
    truncateStringWithDot(activeSectionName, name, 20);
  }
  DEBUG_LOG_VALUE("onActiveSectionName: ", activeSectionName);
  activePage->showActiveSectionName();
  songPercentage = 0.0f; // Réinitialisation du pourcentage de la chanson
}

void Main::setActiveSectionIndex(int index) {
  activeSectionIndex = index;
  DEBUG_LOG_VALUE("activeSectionIndex changed: ", activeSectionIndex);
  // activePage->showActiveSectionIndex();
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
  // activePage->showActiveSectionColor(activeSectionColor);
}
void Main::setSections(char sections[][MAX_SONG_NAME], int count) {
  for (int i = 0; i < count; i++) {
    strcpy(sections[i], sections[i]);
  DEBUG_LOG_VALUE("sections changed: ", sections[i]);
  }
  // activePage->showSections(activePage->sections, count);
}
void Main::setSongs(char songs[][MAX_SONG_NAME], int count) {
  songsListSize = count;
  for (int i = 0; i < count; i++) {
    strcpy(songsList[i], songs[i]);
    DEBUG_LOG_VALUE("songs changed: ", songsList[i]);
  }
  // activePage->showSongs(songsList, count);
}
void Main::setSongDurations(int durations[], int count) {
  // for (int i = 0; i < count; i++) {
  //   songDurations[i] = durations[i];
  // DEBUG_LOG_VALUE("durations changed: ", songDurations);
  // }
  // activePage->showSongDurations(songDurations, count);
}
void Main::setSongColors(char colors[][MAX_SONG_NAME], int count) {
  // for (int i = 0; i < count; i++) {
  //   strcpy(songColors[i], colors[i]);
  //   DEBUG_LOG_VALUE("Song colors changed: ", songColors[i]);
  // }
  // activePage->showSongColors(songColors, count);
}


void Main::setCCReceived(uint8_t channel, uint8_t control, uint8_t value) {
  DEBUG_LOG("CC Received: ");
  DEBUG_LOG(control);
  DEBUG_LOG(" Value: ");
  DEBUG_LOGLN(value);
  activePage->checkLeds(channel, control, value);
}


void Main::configureButton(uint8_t user_mode, uint8_t controlNum, uint8_t control_type, uint8_t control_cc, uint8_t control_ch, uint8_t isCustom, uint8_t toggled){
  // Validation des paramètres d'entrée
  if (user_mode >= 5 || controlNum >= 5 || control_type >= 4) {
    return; // Paramètres invalides, sortie anticipée
  }
  
  user_mode++; // Décalage d'index
  pages[user_mode].setButtonControl(controlNum, controlType[control_type], control_cc, control_ch, isCustom, toggled);
  jsonManager.writeJSONControl(user_mode, controlNum, control_type, control_cc, control_ch, isCustom, toggled);

  // Mise à jour de l'affichage si la page active correspond
  if (activePage == &pages[user_mode]) {
    mainPage.showButtonSprite(
      false, // ou l'état réel du bouton si tu veux
      controlNum,
      getActionName(user_mode, controlNum),
      getActionColor(user_mode, controlNum),
      activePage->getLuminance(controlNum)
    );
  }
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
  //     mainParser.setlistsListSize = listLength;
  //     for (int i = 0; i < MAX_SONG_NAME; i++) {
  //       mainParser.setlistsList[listIndex][i] = strBuf[i];
  //     }
  //   //     DEBUG_LOG("setlist: ");
  //   //     DEBUG_LOG(listIndex);
  //   //     DEBUG_LOG(" / ");
  //   //     DEBUG_LOG(listLength);
  //   //     DEBUG_LOG(" : ");
  //   //     DEBUG_LOGLN(mainPage.setlistsList[listIndex]);
  //     menuPage.createMenu(SETLIST_MENU, 0);
  //     break;

  //   case Song_Array_Item:
  //     mainParser.songsListSize = listLength;
  //     for (int i = 0; i < MAX_SONG_NAME; i++) {
  //       mainParser.songsList[listIndex][i] = strBuf[i];
  //     }
  //   //   DEBUG_LOG("songs: ");
  //   //   DEBUG_LOGLN(mainPage.songsList[listIndex]);
  //     menuPage.createMenu(SONG_MENU, mainParser.activeSongIndex);
  //     break;

  //   default:
  //     // Optionnel : Gérer les cas non prévus
  //   //   DEBUG_LOGLN("Unknown itemType");
  //     break;
  // }
}