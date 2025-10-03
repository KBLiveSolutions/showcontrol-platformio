#include "pages.h"
#include "utils.h"
#include "../config/consts.h"
#include "../display/pages/mainPage.h"
#include "../display/pages/settingsPage.h"
#include "../display/pages/menuPage.h"
#include "../display/pages/splashPage.h"
#include "../display/pages/globalPage.h"
#include "../display/colors.h"
#include "../leds/leds.h"
#include "../osc/OSCManager.h"
#include "../midi/midi_out.h"
#include "mainPageParser.h"
#include "globalParser.h"

// ====================================
// DISPLAY
// ====================================


void Page::showPage() {
    DEBUG_LOG_VALUE("Page::showPage() called for pageNumber: ", pageNumber);

    // Pages spéciales
    switch (pageType) {
      case MENU:      menuPage.showPage();      return;
      case SETTINGS:  settingsPage.showPage();  return;
      case SPLASH:    splashPage.showPage();    return;
      default: break;
    }

    if (pageType == SETLIST) {
        showSetlistElements();
    } else {
        showUserElements();
    }

    DEBUG_LOGLN("Page::showPage() completed");
}

void Page::showSetlistElements() {
    mainPage.updateProgressBarFine(true);
    mainPage.showNextSprite(true);
    mainPage.showRemainingTimeInSong(true);
    mainPage.showRemainingTimeInSet(true);
    mainPage.showSongsCounter(true);
    showActiveSongName();
    showActiveSectionName();
    showNextSongName();
    updateSongIndex();
    updatePlaySprite();
    updateLoopSprite();
}

void Page::showUserElements() {
    DEBUG_LOG_VALUE("Page::showUserElements() called for pageNumber: ", pageNumber);
    // Affichage des 3 sprites selon displayedItem
    for (int i = 0; i < 3; ++i) {
        switch (displayedItem[i]) {
          case SceneName:      showSceneName();        break;
          case TrackName:      showTrackName();        break;
          case LooperName:     showLooperName();       break;
          case MarkerName:     showMarkerName();       break;
          case ActiveSong:     showActiveSongName();   break;
          case ActiveSection:  showActiveSectionName();break;
          case NextSong:       showNextSongName();     break;
          case NextSection:    showNextSectionName();  break;
          case Next_Scene_Name:showNextSceneName();    break;
          default: break;
        }
    }
    if (pageType == USER) showButtons(true);
}

void Page::clearPage() {
  switch (pageType) {
    case MENU:
      menuPage.clearPage();
      break;
    case SETTINGS:
      settingsPage.clearPage();
      break;
    case SPLASH:
      splashPage.clearPage();
      break;
    case SETLIST:{
      mainPage.showMainSprite(" ", defaultBgColor);
      mainPage.showSub1Sprite(" ", defaultBgColor);
      mainPage.showSub2Sprite(" ", defaultBgColor);
      mainPage.showRemainingTimeInSet(false);
      mainPage.updateProgressBarFine(false);
      mainPage.showNextSprite(false);
      mainPage.showRemainingTimeInSong(false);
      mainPage.showSongsCounter(false);
      mainPage.showPlaySprite(false, false);
      mainPage.showLoopSprite(false, false);
      }
      break;
    case USER:{
      showButtons(false);
      mainPage.showMainUserSprite(" ", defaultBgColor);
      mainPage.showSub1UserSprite(" ", defaultBgColor);
      }
      break;
    default:
      break;
  }
}

  // Tableau global de pointeurs vers fonctions d'affichage de sprite
static void (Page::*showSpriteFuncs[5])(const char*, uint16_t) = {
    &Page::showMainSprite,
    &Page::showSub1Sprite,
    &Page::showSub2Sprite,
    &Page::showMainUserSprite,
    &Page::showSub1UserSprite,
};


void Page::showButtons(bool show) {
  DEBUG_LOGLN("Page::showButtons() starting");
  
  // Validation de l'état de la page
  if (pageType == NONE) {
    DEBUG_LOGLN("showButtons: pageType is NONE, aborting");
    return;
  }
  
  for (uint8_t buttonNum = 0; buttonNum < 5; ++buttonNum) {
    // Vérification de sécurité pour l'accès au tableau l[]
    if (buttonNum + 1 >= NUM_LEDS) {
      DEBUG_LOG_VALUE("showButtons: buttonNum+1 out of bounds: ", buttonNum + 1);
      continue;
    }
    
    if (!show) {
      // Mode sécurisé: juste éteindre les LEDs
      l[buttonNum + 1].led_off();
      
      // // Appel sécurisé avec des valeurs par défaut
      if (&mainPage) {  // Vérification que mainPage existe
        mainPage.showButtonSprite(false, buttonNum, "", defaultBgColor, false);
      } else {
        DEBUG_LOGLN("showButtons: mainPage not available");
      }
    } else {
      // Mode affichage: validation plus stricte
      l[buttonNum + 1].setInitColor();
      l[buttonNum + 1].show_color();
      
      // Obtention sécurisée du nom d'action
      const char* actionName = nullptr;
      uint16_t actionColor = defaultBgColor;
      bool luminance = false;
      
      // Protection contre l'accès aux données de page invalides
      if (pageNumber < 6) {
        actionName = getActionName(pageNumber, buttonNum);
        actionColor = getActionColor(pageNumber, buttonNum);
        luminance = getLuminance(buttonNum);
      } else {
        DEBUG_LOG_VALUE("showButtons: invalid pageNumber: ", pageNumber);
        actionName = "Error";
      }
      
      // Validation finale du nom d'action
      if (!actionName) {
        DEBUG_LOG_VALUE("showButtons: null actionName for button: ", buttonNum);
        actionName = "NULL";
      }
      
      // Validation de buttonPressed array
      bool pressed = false;
      if (buttonNum < sizeof(buttonPressed)/sizeof(buttonPressed[0])) {
        pressed = buttonPressed[buttonNum];
      } else {
        DEBUG_LOG_VALUE("showButtons: buttonNum out of buttonPressed bounds: ", buttonNum);
      }
      
      // Appel sécurisé de showButtonSprite
      if (&mainPage) {
        mainPage.showButtonSprite(pressed, buttonNum, actionName, actionColor, luminance);
      } else {
        DEBUG_LOGLN("showButtons: mainPage not available for display");
      }
    }
  }
  
  DEBUG_LOGLN("Page::showButtons() completed");
}

void Page::showSceneName() {
  displayItemByType(SceneName, mainParser.sceneName);
}

void Page::showTrackName() {
    displayItemByType(TrackName, mainParser.trackName);
}

void Page::showLooperName() {
    displayItemByType(LooperName, mainParser.looperName);
}

void Page::showMarkerName() {
    displayItemByType(MarkerName, mainParser.markerName);
}

void Page::showActiveSongName() {
    displayItemByType(ActiveSong, mainParser.activeSongName);
}

void Page::showActiveSectionName() {
    displayItemByType(ActiveSection, mainParser.activeSectionName);
}

void Page::showNextSongName() {
    displayItemByType(NextSong, mainParser.nextSongName);
}

void Page::showNextSectionName() {
    displayItemByType(NextSection, mainParser.nextSectionName);
}

void Page::showNextSceneName() {
    displayItemByType(Next_Scene_Name, mainParser.nextSceneName);
} 

void Page::showMainSprite(const char* txt, uint16_t color) {
    if(pageType == SETLIST || pageType == USER) showSprite(txt, color, mainSprite);
}

void Page::showSub1Sprite(const char* txt, uint16_t color) {
    if(pageType == SETLIST) showSprite(txt, color, sub1Sprite);
}

void Page::showSub2Sprite(const char* txt, uint16_t color) {
    if(pageType == SETLIST) showSprite(txt, color, sub2Sprite);
}

void Page::showMainUserSprite(const char* txt, uint16_t color) {
    if(pageType == USER) showSprite(txt, color, mainUserSprite); // ou adapte selon ta logique
}

void Page::showSub1UserSprite(const char* txt, uint16_t color) {
    if(pageType == USER) showSprite(txt, color, sub1UserSprite);
}

void Page::updateSongIndex(){
  if(pageType == SETLIST){
    mainPage.showSongsCounter(true);
    if (mainParser.activeSongIndex == 0) {
      // Première chanson : LED 3 éteinte (pas de précédent)
      if (3 < NUM_LEDS && l) {
        pages[0].setRGBColor(3, 0, 0, 0);
        DEBUG_LOGLN("LED 3 OFF - First song");
      }
    } else {
      // Pas la première chanson : LED 3 en blanc
      if (3 < NUM_LEDS && l) {
        pages[0].setRGBColor(3, 100, 100, 100);  // Blanc
        DEBUG_LOGLN("LED 3 WHITE - Previous available");
      }
    }
    
    // LED 4 (index 5 dans le tableau l[]) - contrôle "suivant"
    if (mainParser.activeSongIndex == mainParser.songsListSize - 1) {
      // Dernière chanson : LED 4 éteinte (pas de suivant)
      if (4 < NUM_LEDS && l) {
        pages[0].setRGBColor(4, 0, 0, 0);
        DEBUG_LOGLN("LED 4 OFF - Last song");
      }
    } else {
      // Pas la dernière chanson : LED 4 en blanc
      if (4 < NUM_LEDS && l) {
        pages[0].setRGBColor(4, 100, 100, 100);   // Blanc
        DEBUG_LOGLN("LED 4 WHITE - Next available");
      }
    }
    l[3].show_color();
    l[4].show_color();
  }
}

void Page::updatePlaySprite() {
  if (pageType == SETLIST) {
    mainPage.showPlaySprite(global.isPlaying, true);
    if (global.isPlaying) setRGBColor(1, 0, 255, 0);
    else pages[0].setRGBColor(1, 0, 40, 0);
    l[1].show_color();
    setRGBColor(2, 20, 0, 80);
    l[2].show_color();
  }
}

void Page::updateLoopSprite() {
  if (pageType == SETLIST) {
    mainPage.showLoopSprite(global.loopEnabled, true);
    if (global.loopEnabled) {
      {
        setRGBColor(5, 255, 240, 0); // Set to yellow
        l[5].show_color();
      }
    } 
    else {
      setRGBColor(5, 40, 30, 4); // Set to yellow
      l[5].show_color();
    }   
  }
}

// Fonction utilitaire pour éviter la duplication de code
void Page::displayItemByType(displayed_item_t itemType, const char* content) {
    for (int i = 0; i < 3; ++i) {
        if (displayedItem[i] == itemType) {
            DEBUG_LOG_VALUE("Displaying itemType: ", itemType);

            // SETLIST : sprites classiques
            if (pageType == SETLIST) {
                switch (i) {
                    case 0: showMainSprite(content, defaultTxtColor); break;
                    case 1: showSub1Sprite(content, defaultTxtColor); break;
                    case 2: showSub2Sprite(content, defaultTxtColor); break;
                }
            }
            // USER : sprites utilisateurs
            else if (pageType == USER) {
                switch (i) {
                    case 0: showMainUserSprite(content, defaultTxtColor); break;
                    case 1: showSub1UserSprite(content, defaultTxtColor); break;
                    // Ajoute ici d'autres sprites utilisateurs si besoin
                }
            }
            break;
        }
    }
}


bool Page::getLuminance(int num) {
      // Validation stricte des paramètres
      if (num < 0 || num >= 5) {
        DEBUG_LOG_VALUE("getLuminance: invalid num: ", num);
        return false;
      }
      
      // Vérification de sécurité pour l'accès au tableau l[]
      if (num + 1 >= NUM_LEDS) {
        DEBUG_LOG_VALUE("getLuminance: LED index out of bounds: ", num + 1);
        return false;
      }
      
      // Vérification que le tableau l[] est initialisé
      if (!l) {
        DEBUG_LOGLN("getLuminance: LED array not initialized");
        return false;
      }
      
      // Calcul sécurisé de la luminance
      const auto& led = l[num + 1];
      uint16_t totalLuminance = (uint16_t)led.r + (uint16_t)led.g + (uint16_t)led.b;
      
      return totalLuminance > 100;
}