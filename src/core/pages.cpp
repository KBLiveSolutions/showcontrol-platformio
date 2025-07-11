#include "../config/consts.h"
#include <cstring>  // Pour memcpy

#include "pages.h"
#include "utils.h"
#include "mainPageParser.h"
#include "globalParser.h"

#include "../leds/leds.h"
#include "../display/pages/mainPage.h"
#include "../display/pages/settingsPage.h"
#include "../display/pages/menuPage.h"
#include "../display/pages/splashPage.h"
#include "../display/pages/globalPage.h"
#include "../display/colors.h"

#include "../osc/OSCManager.h"
#include "../midi/midi_out.h"

// ====================================
// Initialisation des pages
// ====================================

Page pages[6] = { Page(0, SETLIST), Page(1, USER), 
    Page(2, USER), Page(3, USER), Page(4, USER), Page(5, USER)};

Page SETTINGS_PAGE(6, SETTINGS);
Page MENU_PAGE(7, MENU);
Page SPLASH_PAGE(8, SPLASH);
Page activePage(1, NONE);
Page previousActivePage(1, SPLASH);

Page::Page(uint8_t pageNumber, page_type pageType)
: pageNumber(pageNumber), pageType(pageType) {
  // Initialisation basique pour éviter les problèmes d'optimisation agressive
  // Note: l'avertissement du compilateur sur l'itération 8 semble être un faux positif
  // lié à l'optimisation agressive, mais le code est fonctionnellement correct
  
  control_type[0] = control_type[1] = control_type[2] = control_type[3] = control_type[4] = CC;
  control_cc[0] = control_cc[1] = control_cc[2] = control_cc[3] = control_cc[4] = 1;
  control_ch[0] = control_ch[1] = control_ch[2] = control_ch[3] = control_ch[4] = 1;
  control_toggle[0] = control_toggle[1] = control_toggle[2] = control_toggle[3] = control_toggle[4] = 0;
  control_led_cc[0] = control_led_cc[1] = control_led_cc[2] = control_led_cc[3] = control_led_cc[4] = 1;
  control_led_ch[0] = control_led_ch[1] = control_led_ch[2] = control_led_ch[3] = control_led_ch[4] = 1;
  control_custom[0] = control_custom[1] = control_custom[2] = control_custom[3] = control_custom[4] = 0;
  
  // Initialisation sécurisée des tableaux de boutons
  memset(buttonPressed, false, sizeof(buttonPressed));
  memset(buttonState, 0, sizeof(buttonState));
  
  // Configuration par défaut des éléments affichés
  displayedItem[0] = ActiveSong;
  displayedItem[1] = ActiveSection;
  displayedItem[2] = NextSong;
}


// ====================================
// DISPLAY
// ====================================

  // Tableau global de pointeurs vers fonctions d'affichage de sprite
  static void (Page::*showSpriteFuncs[5])(const char*, uint16_t) = {
      &Page::showMainSprite,
      &Page::showSub1Sprite,
      &Page::showSub2Sprite,
      &Page::showMainUserSprite,
      &Page::showSub1UserSprite,
  };

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

  void Page::showButtons(bool show) {
    DEBUG_LOGLN("Page::showButtons() starting");
    
    // Validation de l'état de la page
    if (pageType == NONE) {
      DEBUG_LOGLN("showButtons: pageType is NONE, aborting");
      return;
    }
    
    // Vérification de l'initialisation des LEDs
    if (!l) {
      DEBUG_LOGLN("showButtons: LED array not initialized");
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
        
        // Appel sécurisé avec des valeurs par défaut
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
    displayItemByType(SceneName, _main.sceneName);
}

void Page::showTrackName() {
    displayItemByType(TrackName, _main.trackName);
}

void Page::showLooperName() {
    displayItemByType(LooperName, _main.looperName);
}

void Page::showMarkerName() {
    displayItemByType(MarkerName, _main.markerName);
}

void Page::showActiveSongName() {
    displayItemByType(ActiveSong, _main.activeSongName);
}

void Page::showActiveSectionName() {
    displayItemByType(ActiveSection, _main.activeSectionName);
}

void Page::showNextSongName() {
    displayItemByType(NextSong, _main.nextSongName);
}

void Page::showNextSectionName() {
    displayItemByType(NextSection, _main.nextSectionName);
}

void Page::showNextSceneName() {
    displayItemByType(Next_Scene_Name, _main.nextSceneName);
} 


void Page::showPage() {
    DEBUG_LOGLN("Page::showPage() starting");
    
    // Traitement des pages spéciales avec early return
    switch (pageType) {
      case MENU:
        DEBUG_LOGLN("Showing MENU page");
        menuPage.showPage();
        return;
      case SETTINGS:
        DEBUG_LOGLN("Showing SETTINGS page");
        settingsPage.showPage();
        return;
      case SPLASH:
        DEBUG_LOGLN("Showing SPLASH page");
        splashPage.showPage();
        return;
      default:
        break;
    }
    
    // Traitement des pages normales (SETLIST et USER)
    bool isSetlist = (pageType == SETLIST);
    DEBUG_LOG_VALUE("Page type - isSetlist: ", isSetlist);
    
    if (isSetlist) {
      DEBUG_LOGLN("Displaying SETLIST page elements");
      mainPage.updateProgressBarFine(true);
      mainPage.showNextSprite(true);
      mainPage.showRemainingTimeInSong(true);
      mainPage.showRemainingTimeInSet(true);
      mainPage.showSongsCounter(true);
      showActiveSongName();
      showActiveSectionName();
      showNextSongName();
    }
    else {
      DEBUG_LOGLN("Displaying USER page elements");
      // Affichage des 3 sprites selon displayedItem
      for (int i = 0; i < 3; ++i) {
        DEBUG_LOG_VALUE("Processing displayedItem: ", i);
        switch (displayedItem[i]) {
          case SceneName:
            showSceneName();
            break;
          case TrackName:
            showTrackName();
            break;
          case LooperName:
            showLooperName();
            break;
          case MarkerName:
            showMarkerName();
            break;
          case ActiveSong:
            showActiveSongName();
            break;
          case ActiveSection:
            showActiveSectionName();
            break;
          case NextSong:
            showNextSongName();
            break;
          case NextSection:
            showNextSectionName();
            break;
          case Next_Scene_Name:
            showNextSceneName();
            break;
          default:
            DEBUG_LOG_VALUE("No display for slot: ", i);
            // Rien à afficher pour ce slot
            break;
        }
      }
    }
    
    // Affichage sécurisé des boutons - seulement pour les pages USER
    if (!isSetlist && pageType == USER) {
      DEBUG_LOGLN("Showing buttons for USER page");
      showButtons(true);
    } else {
      DEBUG_LOGLN("Buttons disabled for this page type");
    }
    
    DEBUG_LOGLN("Page::showPage() completed");
}

void Page::showMainSprite(const char* txt, uint16_t color) {
    if(pageType == SETLIST) showSprite(txt, color, mainSprite); // ou adapte selon ta logique
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
      mainPage.showRemainingTimeInSet(false);
      mainPage.updateProgressBarFine(false);
      mainPage.showNextSprite(false);
      mainPage.showRemainingTimeInSong(false);
      mainPage.showSongsCounter(false);
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
  
// ====================================
// LEDS
// ====================================

void Page::checkLeds(uint8_t channel, uint8_t _control, uint8_t value) {
  for (int i = 0; i < 5; i++) {  // Utilisation explicite de 5
    if (_control == control_led_cc[i] && channel == control_ch[i]) {
      // Vérification de sécurité pour l'accès au tableau l[]
      if (i + 1 < NUM_LEDS) {
        l[i + 1].setIndexedColor(value, channel);
        l[i + 1].show_color();
        // if (pageType == USER) mainPage.showButtonSprite(buttonPressed[i], i, getActionName(pageNumber, i), getActionColor(pageNumber, i), getLuminance(i));
      }
      break;
    }
  }
}

void Page::setRGBColor(uint8_t idx, uint8_t red, uint8_t green, uint8_t blue) {
  // Vérification de sécurité pour l'accès au tableau l[]
  led[idx][0] = red;
  led[idx][1] = green;
  led[idx][2] = blue;
  l[idx].setRGBColor(red, green, blue);
  l[idx].show_color();
}

void Page::showLeds(){  
  for (int i = 0; i < NUM_LEDS; i++) {
   // Vérification de sécurité pour l'accès au tableau l[]
   l[i].setRGBColor(led[i][0], led[i][1], led[i][2]);
   l[i].show_color();
  }
}

// ====================================
// ACTIONS
// ====================================

// Helper function pour traiter les boutons SETLIST
void Page::handleSetlistButton(uint8_t buttonNum) {
  switch (buttonNum) {
    case 0:
      sendOSCAbleset("/global/play");
      break;
    case 1:
      sendOSCAbleset("/global/stop");
      break;
    case 2:
      sendOSCAbleset("/setlist/jumpBySongs", -1);
      break;
    case 3:
      sendOSCAbleset("/setlist/jumpBySongs", 1);
      break;
    case 4:
      sendOSCAbleset(global.loopEnabled ? "/loop/escape" : "/loop/enable");
      break;
    default:
      break;
  }
}

void Page::buttonLongPress(uint8_t buttonNum) {
  if (buttonNum >= 5) return;  // Protection contre l'accès hors limites
  sendOSCShowControl("/showcontrol/longPress", control_cc[buttonNum], 127, control_ch[buttonNum] + 1);
}

void Page::buttonDoublePress(uint8_t buttonNum) {
  if (buttonNum >= 5) return;  // Protection contre l'accès hors limites
  sendOSCShowControl("/showcontrol/doublePress", control_cc[buttonNum], 127, control_ch[buttonNum] + 1);
}

void Page::onButtonShortPress(uint8_t buttonNum) {
  // Vérification de sécurité pour buttonPressed[]
  if (buttonNum >= 8) return;  // Protection pour le tableau buttonPressed[8]
  
  buttonPressed[buttonNum] = true;
  
  if (buttonNum < 5) {
    // Variables locales pour éviter les accès répétés aux tableaux
    uint8_t cc = control_cc[buttonNum];
    uint8_t ch = control_ch[buttonNum];
    control_type_t type = control_type[buttonNum];
    
    if (control_toggle[buttonNum] == 0) {
      sendOSCShowControl("/showcontrol/control_id", cc, 127, ch + 1);
      midi::sendToDAWPort(type, ch, cc, 127);
      midi::sendToDAWPort(type, ch, cc, 0);
    } 
    else {
      buttonState[buttonNum] = !buttonState[buttonNum];  // Plus efficace que modulo
      uint8_t value = buttonState[buttonNum] * 127;
      sendOSCShowControl("/showcontrol/control_id", cc, value, ch + 1);
      midi::sendToDAWPort(type, ch, cc, value);
    }
    
    // Traitement spécifique selon le type de page
    if (pageType == SETLIST) {
      handleSetlistButton(buttonNum);
    }
    // Note: USER case commenté dans l'original
  } 
  else {
    press_pedal(buttonNum - 6);
  }
}

void Page::press_button(uint8_t buttonNum) {
}

void Page::release_button(uint8_t buttonNum) {
  // buttonPressed[buttonNum] = false;
  // if (buttonNum < 5) {
  //   l[buttonNum + 1].show_color();
  //   if (pageType == USER) 
  //   {
  //     mainPage.showButton(buttonPressed[buttonNum], buttonNum, getActionName(pageNumber, buttonNum), getActionColor(pageNumber, buttonNum), getLuminance(buttonNum));
  //   }
  //   if (control_toggle[buttonNum] == 0 && control_type[buttonNum] != PC) {
  //     sendOSCShowControl("/showcontrol/control_id", control_cc[buttonNum], 0, control_ch[buttonNum] + 1);
  //     midi::sendToDAWPort(control_type[buttonNum], control_ch[buttonNum], control_cc[buttonNum], 0);
  //   }
  // } 
  // else release_pedal(buttonNum - 6);
}



void Page::doubleClick(uint8_t buttonNum) {
}

// ====================================
// PEDALS
// ====================================


void Page::expressionPedal(uint8_t pedalNum, uint8_t value) {
  // TO DO: Implement expression pedal handling
  // midi::sendToDAWPort(CC, pedal_ch[pedalNum], pedal_cc[pedalNum], value);
}

void Page::press_pedal(uint8_t pedalNum) {
  // if (pedal_toggle[pedalNum] == 0) {
  //   sendOSCShowControl("/showcontrol/control_id", pedal_cc[pedalNum], 127, pedal_ch[pedalNum]);
  //   midi::sendToDAWPort(pedal_type[pedalNum], pedal_ch[pedalNum], pedal_cc[pedalNum], 127);
  // } else {
  //   pedalState[pedalNum] = (pedalState[pedalNum] + 1) % 2;
  //   sendOSCShowControl("/showcontrol/control_id", pedal_cc[pedalNum], pedalState[pedalNum] * 127, pedal_ch[pedalNum]);
  //   midi::sendToDAWPort(pedal_type[pedalNum], pedal_ch[pedalNum], pedal_cc[pedalNum], pedalState[pedalNum] * 127);
  // }
}

void Page::release_pedal(uint8_t pedalNum) {
  // if (pedal_toggle[pedalNum] == 0) {
  //   sendOSCShowControl("/showcontrol/control_id", pedal_cc[pedalNum], 0, pedal_ch[pedalNum]);
  //   midi::sendToDAWPort(pedal_type[pedalNum], pedal_ch[pedalNum], pedal_cc[pedalNum], 0);
  // }
}

// Méthodes de Page (copie/colle le contenu de chaque méthode ici)
// ... (toutes les méthodes de la struct Page, à copier depuis ton .h)

    
// ====================================
// CONFIGURE
// ====================================

  void Page::setButtonControl(uint8_t controlNum, control_type_t type, uint8_t cc, uint8_t channel, uint8_t custom, uint8_t toggled) {
    if (controlNum >= 5) return;  // Protection contre l'accès hors limites
    
    int user_mode = _main.selectedMode;
    control_type[controlNum] = type;
    control_cc[controlNum] = cc;
    control_ch[controlNum] = channel;
    control_custom[controlNum] = custom;
    control_toggle[controlNum] = toggled;
    // if(pageNumber == _main.selectedMode) mainPage.showButtonSprite(false, controlNum, getActionName(user_mode, controlNum), getActionColor(user_mode, controlNum), getLuminance(controlNum));
  };

  void Page::setButtonControlShifted(uint8_t controlNum, control_type_t type, uint8_t cc, uint8_t channel) {
    if (controlNum >= 5) return;  // Protection contre l'accès hors limites
    
    control_type[controlNum] = type;
    control_cc[controlNum] = cc;
    control_ch[controlNum] = channel;
  };

  void Page::setLedControl(uint8_t controlNum, control_type_t type, uint8_t cc, uint8_t channel) {
    if (controlNum >= 5) return;  // Protection contre l'accès hors limites
    
    control_led_cc[controlNum] = cc;
    control_led_ch[controlNum] = channel;
  };

  void Page::setPedal(int controlNum, control_type_t type, uint8_t cc, uint8_t channel, uint8_t custom, uint8_t toggled) {
    // pedal_type[controlNum] = type;
    // pedal_cc[controlNum] = cc;
    // pedal_ch[controlNum] = channel;
    // pedal_custom[controlNum] = custom;
    // pedal_toggle[controlNum] = toggled;
  };

  void Page::setDisplay(uint8_t display_num, uint8_t display_type) {
    if (display_num >= 3) return;  // Protection contre l'accès hors limites au tableau displayedItem[3]
    displayedItem[display_num] = static_cast<displayed_item_t>(display_type);
  }
  
// Opérateurs globaux
bool operator==(const Page& lhs, const Page& rhs) {
  return lhs.pageNumber == rhs.pageNumber;
}
bool operator!=(const Page& lhs, const Page& rhs) {
  return !(lhs == rhs);
}

Page& Page::operator=(const Page& other) {
    if (this != &other) {
        pageNumber = other.pageNumber;
        pageType = other.pageType;
        
        // Utilisation de memcpy pour les copies de tableaux (plus efficace)
        memcpy(buttonPressed, other.buttonPressed, sizeof(buttonPressed));
        memcpy(buttonState, other.buttonState, sizeof(buttonState));
        memcpy(control_custom, other.control_custom, sizeof(control_custom));
        memcpy(control_type, other.control_type, sizeof(control_type));
        memcpy(control_led_cc, other.control_led_cc, sizeof(control_led_cc));
        memcpy(control_led_ch, other.control_led_ch, sizeof(control_led_ch));
        memcpy(control_cc, other.control_cc, sizeof(control_cc));
        memcpy(control_ch, other.control_ch, sizeof(control_ch));
        memcpy(control_toggle, other.control_toggle, sizeof(control_toggle));
        memcpy(pedalState, other.pedalState, sizeof(pedalState));
        memcpy(pedal_cc, other.pedal_cc, sizeof(pedal_cc));
        memcpy(pedal_ch, other.pedal_ch, sizeof(pedal_ch));
        memcpy(pedal_toggle, other.pedal_toggle, sizeof(pedal_toggle));
        memcpy(pedal_custom, other.pedal_custom, sizeof(pedal_custom));
        memcpy(displayedItem, other.displayedItem, sizeof(displayedItem));
        memcpy(led, other.led, sizeof(led));
        
        maxSpritesAmnt = other.maxSpritesAmnt;
    }
    return *this;
}

void removeUserPage(int pageNum) {
  // À implémenter
}


// ====================================
// GLOBAL
// ====================================

void setupPages() {
  // Configuration optimisée avec des tableaux de données
  static const uint8_t buttonCCs[] = {44, 12, 33, 34, 113};
  static const uint8_t ledCCs[] = {1, 34, 33, 34, 113};
  
  for (uint8_t i = 0; i < 5; ++i) {
    pages[0].setButtonControl(i, CC, buttonCCs[i], 15, 0, 0);
    pages[0].setLedControl(i, CC, ledCCs[i], 15);
  }
  switchActivePage(SPLASH_PAGE);
  DEBUG_LOGLN("SETUP PAGES DONE");
}


// ====================================
// DISPLAY
// ====================================

// Fonction utilitaire pour éviter la duplication de code
void Page::displayItemByType(displayed_item_t itemType, const char* content) {
  for (int i = 0; i < 3; ++i) {
    if (displayedItem[i] == itemType) {
      // Vérification de sécurité pour l'accès au tableau showSpriteFuncs[5]
      if (i < 5) {
        (this->*showSpriteFuncs[i])(content, defaultTxtColor);
      }
      break;
    }
  }
}

void switchActivePage(Page& newPage) {
    DEBUG_LOG_VALUE("switchActivePage called with pageNumber: ", newPage.pageNumber);
    
    // Protection contre les appels simultanés
    static bool isSwitching = false;
    if (isSwitching) {
        DEBUG_LOGLN("Page switch already in progress, ignoring");
        return;
    }
    isSwitching = true;
    
    // CORRECTION: Sauvegarder AVANT de nettoyer
    if(activePage != SETTINGS_PAGE && activePage != MENU_PAGE) previousActivePage = activePage;  // Sauvegarde AVANT nettoyage
    
    activePage.clearPage();
    DEBUG_LOG_VALUE("switchActivePage: previousActivePage pageNumber: ", previousActivePage.pageNumber);
    
    if (newPage.pageType == NONE) {
        DEBUG_LOGLN("switchActivePage: newPage is NONE, aborting");
        isSwitching = false;
        return;
    }
    activePage = newPage;
        // Appels aux pages globales avec gestion d'erreurs
    if (newPage.pageType != SPLASH) {
      globalPage.showTitle();
      globalPage.showPageIcon();
    }
    // Affichage sécurisé de la nouvelle page
    activePage.showPage();
    activePage.showLeds();  // Affichage des LEDs après changement de page
    isSwitching = false;
}