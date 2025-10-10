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
#include "../leds/led_utils.h"
#include "../midi/midi_out.h"
#include "../osc/OSCManager.h"

Main mainParser;

// Fonction utilitaire optimisée pour tronquer les chaînes avec un point
void truncateStringWithDot(char *destination, const char *source, size_t maxLength)
{
  if (!source || !destination || maxLength == 0)
  {
    if (destination)
      destination[0] = '\0';
    return;
  }

  size_t len = strlen(source);

  if (len <= maxLength)
  {
    // Copie directe si la chaîne tient dans la limite
    strcpy(destination, source);
  }
  else
  {
    // Troncature avec point si trop long
    strncpy(destination, source, maxLength - 1);
    destination[maxLength - 1] = '.';
    destination[maxLength] = '\0';
  }
}

void Main::onNewSetLoaded()
{
  onActiveSongName(" ");
  onActiveSectionName(" ");
  onNextSongName(" ");
  osc.getValues();
}

void Main::sendSetlistArrayRequest()
{
  static uint8_t sysex_msg[7] = {240, 122, 29, 1, 19, 14, 247};
  midi::sendSysexToDAW(sysex_msg, 7);
  osc.sendSysex(sysex_msg, 7);
}

void Main::sendSongArrayRequest()
{
  static uint8_t sysex_msg[7] = {240, 122, 29, 1, 19, 15, 247};
  midi::sendSysexToDAW(sysex_msg, 7);
  osc.sendSysex(sysex_msg, 7);
}

void Main::onSceneName(char *_sceneName)
{
  strncpy(sceneName, _sceneName, sizeof(sceneName) - 1);
  sceneName[sizeof(sceneName) - 1] = '\0'; // Toujours terminer la chaîne
  DEBUG_LOG_VALUE("onSceneName: ", sceneName);
  activePage->showSceneName();
}

void Main::onTrackName(char *_trackName)
{
  strncpy(trackName, _trackName, sizeof(trackName) - 1);
  trackName[sizeof(trackName) - 1] = '\0';
  activePage->showTrackName();
}

void Main::onLooperName(char *_looperName)
{
  strncpy(looperName, _looperName, sizeof(looperName) - 1);
  looperName[sizeof(looperName) - 1] = '\0';
  DEBUG_LOG_VALUE("onLooperName: ", looperName);
  activePage->showLooperName();
} 
void Main::onLeftMarkerName(char *_markerName)
{
  strncpy(leftMarkerName, _markerName, sizeof(leftMarkerName) - 1);
  leftMarkerName[sizeof(leftMarkerName) - 1] = '\0';
  DEBUG_LOG_VALUE("onLeftMarkerName: ", leftMarkerName);
  activePage->showLeftMarkerName();
}
void Main::onRightMarkerName(char *_nextMarkerName)
{
  strncpy(rightMarkerName, _nextMarkerName, sizeof(rightMarkerName) - 1);
  rightMarkerName[sizeof(rightMarkerName) - 1] = '\0';
  DEBUG_LOG_VALUE("onRightMarkerName: ", rightMarkerName);
  activePage->showRightMarkerName();
}

void Main::onActiveSongName(char *songName)
{
  DEBUG_LOG_VALUE("onActiveSongName: ", songName);

  // Extraction du texte entre parenthèses
  char infoMsg[32] = "";
  char cleanName[64];
  if (songName)
  {
    const char *open = strchr(songName, '(');
    const char *close = strchr(songName, ')');
    if (open && close && close > open)
    {
      size_t infoLen = close - open - 1;
      if (infoLen > 0 && infoLen < sizeof(infoMsg))
      {
        strncpy(infoMsg, open + 1, infoLen);
        infoMsg[infoLen] = '\0';
        onInformationMessage(infoMsg, true);
      }
      // Copier le nom sans la partie entre ()
      size_t prefixLen = open - songName;
      strncpy(cleanName, songName, prefixLen);
      cleanName[prefixLen] = '\0';
      // Ajouter la partie après )
      strncat(cleanName, close + 1, sizeof(cleanName) - strlen(cleanName) - 1);
      // Nettoyer les espaces superflus
      size_t len = strlen(cleanName);
      while (len > 0 && (cleanName[len - 1] == ' ' || cleanName[len - 1] == '-'))
      {
        cleanName[len - 1] = '\0';
        len--;
      }
      truncateStringWithDot(activeSongName, cleanName, 16);
    }
    else
    {
      truncateStringWithDot(activeSongName, songName, 16);
      onInformationMessage("empty", false);
    }
  }
  else
  {
    activeSongName[0] = '\0';
  }
  activePage->showActiveSongName();
  activeSongNameDisplayedOnce = true; // Marquer que le nom de la chanson a été affiché au moins une fois

  strcpy(activeSectionName, " ");
  DEBUG_LOG_VALUE("onActiveSectionName: ", activeSectionName);
  activePage->showActiveSectionName();
}

void Main::onActiveSongIndex(int index)
{
  activeSongIndex = index;
  DEBUG_LOG_VALUE("activeSongIndex changed: ", activeSongIndex);
  // Validation des paramètres pour éviter les erreurs
  if (mainParser.songsListSize <= 0)
  {
    DEBUG_LOG_VALUE("onActiveSongIndex: invalid songsListSize: ", mainParser.songsListSize);
    return;
  }

  if (mainParser.activeSongIndex < 0)
  {
    DEBUG_LOG_VALUE("onActiveSongIndex: invalid activeSongIndex: ", mainParser.activeSongIndex);
    return;
  }
  activePage->updateSongIndex();
  // Contrôle des LEDs selon la position dans la liste
  // LED 3 (index 4 dans le tableau l[]) - contrôle "précédent"
}

void Main::onInformationMessage(const char *message, bool show)
{
  if (!message || strlen(message) == 0)
  {
    DEBUG_LOGLN("onInformationMessage: empty message");
    return;
  }

  // Limitation à 32 caractères
  char infoMsg[32];
  strncpy(infoMsg, message, sizeof(infoMsg) - 1);
  infoMsg[sizeof(infoMsg) - 1] = '\0'; // Assurer la terminaison de chaîne

  mainPage.showInfoSprite(infoMsg, _White, show);
}

void Main::updateSongProgress()
{
  if (activePage->pageType == SETLIST)
  {
    // mainPage.updateProgressBarFine(true);
    mainPage.showRemainingTimeInSong(true);
  }
}

void Main::onActiveSongColor(uint16_t color)
{
  activeSongColor = color;
  DEBUG_LOG_VALUE("onActiveSongColor: ", activeSongColor);
  // activeSongColorShade = hexStringtoRGB565Shade(strBuf);
  activeSectionColor = activeSongColorShade;
  updateSongProgress();
}

void Main::onActiveSongStart(float time)
{
  activeSongStart = time;
  activeSongEnd = 0;
  DEBUG_LOG_VALUE("onActiveSongStart: ", activeSongStart);
}

void Main::onActiveSongEnd(float time)
{
  activeSongEnd = time;
  DEBUG_LOG_VALUE("activeSongEnd: ", activeSongEnd);
}

void Main::onActiveSongDuration(float duration)
{
  activeSongDuration = duration;
  DEBUG_LOG_VALUE("activeSongDuration changed: ", activeSongDuration);
  updateSongProgress();
}

void Main::onRemainingTimeInSong(float time)
{
  remainingTimeInSong = time + 1;
  DEBUG_LOG_VALUE("remainingTimeInSong changed: ", remainingTimeInSong);
  currentSeconds = remainingTimeInSong;
  // Calcul automatique de songPercentage
  if (activeSongDuration > 0 && remainingTimeInSong >= 0)
  {
    songPercentage = 1.0f - (remainingTimeInSong / activeSongDuration);
    if (songPercentage < 0)
      songPercentage = 0;
    if (songPercentage > 1)
      songPercentage = 1;
  }
  else
  {
    songPercentage = 0;
  }
  if (currentSeconds != previousSeconds)
  {
    if (activePage->pageType == SETLIST)
      mainPage.showRemainingTimeInSong(true);
    if (activePage->pageType == SETLIST)
    {
      // mainPage.updateProgressBarFine(true);
    }
    previousSeconds = currentSeconds;
  }
}

void Main::onActiveSongProgress(float progress)
{
  // songPercentage = progress;
  // if(activePage->pageType == SETLIST) {
  //   mainPage.updateProgressBarFine(true);
  // }
}

void Main::onRemainingTimeInSet(float time)
{
  remainingTimeInSet = time;
  DEBUG_LOG_VALUE("remainingTimeInSet changed: ", remainingTimeInSet);
  if (activePage->pageType == SETLIST)
    mainPage.showRemainingTimeInSet(true);
}

void Main::onNextSongName(char *name)
{
  // Limitation à 20 caractères avec point de troncature
  truncateStringWithDot(nextSongName, name, 20);

  DEBUG_LOG_VALUE("onNextSongName: ", nextSongName);
  activePage->showNextSongName();
}

void Main::onNextSongColor(uint16_t color)
{
  nextSongColor = color;
  DEBUG_LOG_VALUE("onNextSongColor: ", nextSongColor);
  // activePage->showNextSongColor();
}

void Main::onActiveSectionName(char *name)
{
  // Si name est vide, afficher >>>
  if (!name || name[0] == '\0')
  {
    strncpy(activeSectionName, ">>>", sizeof(activeSectionName) - 1);
    activeSectionName[sizeof(activeSectionName) - 1] = '\0';
  }
  else
  {
    // Limitation à 20 caractères avec point de troncature
    truncateStringWithDot(activeSectionName, name, 20);
  }
  DEBUG_LOG_VALUE("onActiveSectionName: ", activeSectionName);
  activePage->showActiveSectionName();
  songPercentage = 0.0f; // Réinitialisation du pourcentage de la chanson
}

void Main::onActiveSectionIndex(int index)
{
  activeSectionIndex = index;
  DEBUG_LOG_VALUE("activeSectionIndex changed: ", activeSectionIndex);
  // activePage->showActiveSectionIndex();
}
void Main::onActiveSectionStart(float time)
{
  activeSectionStart = time;
  activeSectionEnd = 0;
  // DEBUG_LOG_VALUE("onActiveSectionStart changed: ", onActiveSectionStart);
}
void Main::onActiveSectionEnd(float time)
{
  activeSectionEnd = time;
  DEBUG_LOG_VALUE("activeSectionEnd changed: ", activeSectionEnd);
}
void Main::onActiveSectionColor(uint16_t color)
{
  activeSectionColor = color;
  // DEBUG_LOG_VALUE("activeSectionColor changed: ", activeSectionColor);
  // activePage->showActiveSectionColor(activeSectionColor);
}
void Main::onSections(char sections[][MAX_SONG_NAME], int count)
{
  for (int i = 0; i < count; i++)
  {
    strcpy(sections[i], sections[i]);
    DEBUG_LOG_VALUE("sections changed: ", sections[i]);
  }
  // activePage->showSections(activePage->sections, count);
}
void Main::onSongs(char songs[][MAX_SONG_NAME], int count)
{
  songsListSize = count;
  for (int i = 0; i < count; i++)
  {
    strcpy(songsList[i], songs[i]);
    DEBUG_LOG_VALUE("songs changed: ", songsList[i]);
  }
  // activePage->showSongs(songsList, count);
}
void Main::onSongDurations(int durations[], int count)
{
  // for (int i = 0; i < count; i++) {
  //   songDurations[i] = durations[i];
  // DEBUG_LOG_VALUE("durations changed: ", songDurations);
  // }
  // activePage->showSongDurations(songDurations, count);
}
void Main::onSongColors(char colors[][MAX_SONG_NAME], int count)
{
  // for (int i = 0; i < count; i++) {
  //   strcpy(songColors[i], colors[i]);
  //   DEBUG_LOG_VALUE("Song colors changed: ", songColors[i]);
  // }
  // activePage->showSongColors(songColors, count);
}

void Main::onCCReceived(uint8_t channel, uint8_t control, uint8_t value)
{
  DEBUG_LOG("CC Received: ");
  DEBUG_LOG_CONT(control);
  DEBUG_LOG_CONT(" Value: ");
  DEBUG_LOG_CONT_LN(value);
  if (activePage->pageType == SETLIST)
  {
    if (control == 1){
      global.isPlaying = value == 126 ? true : false;
      activePage->updatePlaySprite();
    }
    if (control == 113){
      global.loopEnabled = value == 127 ? true : false;
      activePage->updateLoopSprite();
    }
    if (control == 7){
      global.loopEnabled = value == 80 ? true : false;
      activePage->updateLoopSprite();
    }
    return;
  }
  for (int i = 0; i < 5; i++)
  {
    int offset = settings.shiftPressed ? 5 : 0;
    int controlNum = i + offset;
    if (control == activePage->controls[controlNum].led_cc && channel == activePage->controls[controlNum].ch){
      activePage->updateLed(controlNum, value);
      if (activePage->pageType == USER) mainPage.showButtonSprite(activePage->buttonPressed[i], i, activePage->controls[controlNum].actionName, activePage->controls[controlNum].color, activePage->controls[controlNum].luminance);
    }
    break;
  }
}

void Main::configureButton(uint8_t pageNum, uint8_t controlNum, uint8_t control_type, uint8_t control_cc, uint8_t control_ch, uint8_t isCustom, uint8_t toggled, uint8_t color_index)
{
  // Validation des paramètres d'entrée
  if (pageNum >= 5 || controlNum >= 10 || control_type >= 4)
  {
    return; // Paramètres invalides, sortie anticipée
  }

  pages[pageNum].controls[controlNum].type = controlType[control_type];
  pages[pageNum].controls[controlNum].cc = control_cc;
  pages[pageNum].controls[controlNum].ch = control_ch;
  pages[pageNum].controls[controlNum].custom = isCustom;
  pages[pageNum].controls[controlNum].toggle = toggled;
  const char *name = getActionName(pageNum, controlNum);
  strncpy(pages[pageNum].controls[controlNum].actionName, name ? name : "", sizeof(pages[pageNum].controls[controlNum].actionName) - 1);
  pages[pageNum].controls[controlNum].actionName[sizeof(pages[pageNum].controls[controlNum].actionName) - 1] = '\0';
  pages[pageNum].controls[controlNum].led_color = color_index;
  pages[pageNum].controls[controlNum].color = getColorFromIndex(color_index);
  pages[pageNum].controls[controlNum].luminance = activePage->getLuminance(controlNum);
  
  jsonManager.writeJSONControl(pageNum, controlNum, control_type, control_cc, control_ch, isCustom, toggled,
                               pages[pageNum].controls[controlNum].actionName,
                               color_index,
                               pages[pageNum].controls[controlNum].luminance);

  // Mise à jour de l'affichage si la page active correspond
  if (activePage == &pages[pageNum]){
    pages[pageNum].updateLed(controlNum, 127); // Initialiser la LED à éteinte
    mainPage.showButtonSprite(false, controlNum,  pages[pageNum].controls[controlNum].actionName, pages[pageNum].controls[controlNum].color, pages[pageNum].controls[controlNum].luminance);
  }
}

// void Main::configurePedal(uint8_t pageNum, uint8_t controlNum, uint8_t control_type, uint8_t control_cc, uint8_t control_ch, uint8_t isCustom, uint8_t toggled){
//   pages[pageNum].setPedal(controlNum, controlType[control_type], control_cc, control_ch, isCustom, toggled);
//   jsonManager.writeJSONPedal(pageNum, controlNum, control_type, control_cc, control_ch, isCustom, toggled);
//   if(pageNum == selectedMode) {
//     mainPage.showButtonSprite(false, controlNum, getActionName(pageNum, controlNum), getActionColor(pageNum, controlNum));
//   }
// }

void Main::configureLed(uint8_t pageNum, uint8_t controlNum, uint8_t control_type, uint8_t control_led_cc, uint8_t control_led_ch)
{
  pages[pageNum].controls[controlNum].led_cc = control_led_cc;
  pages[pageNum].controls[controlNum].led_ch = control_led_ch;
  // pages[pageNum].controls[controlNum].color = l[controlNum + 1].getColor(); //getActionColor(pageNum, controlNum);
}

void Main::configureDisplay(uint8_t pageNum, uint8_t display_num, uint8_t display_type)
{
  pages[pageNum].displayedItem[display_num] = static_cast<displayed_item_t>(display_type);
  jsonManager.writeJSONDisplay(pageNum, display_num, display_type);
}

// Fonction optimisée pour trouver l'index d'un élément
int indexOfItem(displayed_item_t itemToFind)
{
  // Utilisation de la taille réelle du tableau au lieu de sizeof()
  constexpr size_t arraySize = sizeof(displayedItemsArray) / sizeof(displayedItemsArray[0]);

  for (size_t i = 0; i < arraySize; ++i)
  {
    if (displayedItemsArray[i] == itemToFind)
    {
      return static_cast<int>(i);
    }
  }
  return -1; // Élément non trouvé
}

void Main::parseArrayItem(uint8_t itemType, char *strBuf, uint8_t listIndex, uint8_t listLength)
{
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