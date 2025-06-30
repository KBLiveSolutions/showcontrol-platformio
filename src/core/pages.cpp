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
       return (l[num+1].r + l[num+1].g + l[num+1].b) > 100;
  }

  void Page::showButtons(bool show) {
    for (uint8_t buttonNum = 0; buttonNum < 5; ++buttonNum) {
      if (!show) {
        l[buttonNum + 1].led_off();
        mainPage.showButtonSprite(false, buttonNum, "", defaultBgColor, false);
      } else {
        l[buttonNum + 1].setInitColor();
        l[buttonNum + 1].show_color();
        mainPage.showButtonSprite(
          buttonPressed[buttonNum], 
          buttonNum, 
          getActionName(pageNumber, buttonNum), 
          getActionColor(pageNumber, buttonNum), 
          getLuminance(buttonNum)
        );
      }
    }
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
    // Traitement des pages spéciales avec early return
    switch (pageType) {
      case MENU:
        menuPage.showPage();
        return;
      case SETTINGS:
        settingsPage.showPage();
        return;
      case SPLASH:
        splashPage.showPage();
        return;
      default:
        break;
    }
    
    // Traitement des pages normales (SETLIST et USER)
    bool isSetlist = (pageType == SETLIST);
    
    if (isSetlist) {
      mainPage.updateProgressBarFine(true);
      mainPage.showNextSprite(true);
      mainPage.showRemainingTimeInSong(true);
      mainPage.showRemainingTimeInSet(true);
      mainPage.showSongsCounter(true);
    }
    
    // Affichage des 3 sprites selon displayedItem
    for (int i = 0; i < 3; ++i) {
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
          // Rien à afficher pour ce slot
          break;
      }
    }
    showButtons(!isSetlist); // Affiche les boutons seulement si ce n'est pas une page SETLIST
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
    case SETLIST:
      mainPage.showRemainingTimeInSet(false);
      mainPage.updateProgressBarFine(false);
      mainPage.showNextSprite(false);
      mainPage.showRemainingTimeInSong(false);
      mainPage.showSongsCounter(false);
      break;
    case USER:
      showButtons(false);
      mainPage.showMainUserSprite(" ", defaultBgColor);
      mainPage.showSub1UserSprite(" ", defaultBgColor);
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
      l[i + 1].setColor(value, channel);
      l[i + 1].show_color();
      // if (pageType == USER) mainPage.showButtonSprite(buttonPressed[i], i, getActionName(pageNumber, i), getActionColor(pageNumber, i), getLuminance(i));
      break;
    }
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
  sendOSCShowControl("/showcontrol/longPress", control_cc[buttonNum], 127, control_ch[buttonNum] + 1);
}

void Page::buttonDoublePress(uint8_t buttonNum) {
  sendOSCShowControl("/showcontrol/doublePress", control_cc[buttonNum], 127, control_ch[buttonNum] + 1);
}

void Page::onButtonShortPress(uint8_t buttonNum) {
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
    int user_mode = _main.selectedMode;
    control_type[controlNum] = type;
    control_cc[controlNum] = cc;
    control_ch[controlNum] = channel;
    control_custom[controlNum] = custom;
    control_toggle[controlNum] = toggled;
    // if(pageNumber == _main.selectedMode) mainPage.showButtonSprite(false, controlNum, getActionName(user_mode, controlNum), getActionColor(user_mode, controlNum), getLuminance(controlNum));
  };

  void Page::setButtonControlShifted(uint8_t controlNum, control_type_t type, uint8_t cc, uint8_t channel) {
    control_type[controlNum] = type;
    control_cc[controlNum] = cc;
    control_ch[controlNum] = channel;
  };

  void Page::setLedControl(uint8_t controlNum, control_type_t type, uint8_t cc, uint8_t channel) {
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
    displayedItem[display_num] = static_cast<displayed_item_t>(display_type);
  }
  
// Opérateurs globaux
bool operator==(const Page& lhs, const Page& rhs) {
  return lhs.pageNumber == rhs.pageNumber;
}
bool operator!=(const Page& lhs, const Page& rhs) {
  return !(lhs == rhs);
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

  activePage = SPLASH_PAGE;
  previousActivePage = SPLASH_PAGE;
  DEBUG_LOGLN("SETUP PAGES DONE");
}


void changeActivePage(uint8_t pageNum) {
    previousActivePage = activePage;  // Sauvegarde commune
    
    if (pageNum < 6) {
        activePage = pages[pageNum];
        // jsonManager.writeOption("activePage", pageNum);
    } 
    else if (pageNum >= 6 && pageNum <= 8) {
        // Utilisation d'un tableau statique pour éviter les if/else multiples
        static Page* const specialPages[] = {&SETTINGS_PAGE, &MENU_PAGE, &SPLASH_PAGE};
        activePage = *specialPages[pageNum - 6];
        
        if (pageNum == 7) {  // MENU_PAGE
            menuPage.showPage();
            return;  // Early return pour éviter les appels redondants
        }
    }
    else {
        // Page invalide, ne rien faire
        DEBUG_LOG_VALUE("Invalid page number: ", pageNum);
        return;
    }
    
    activePage.showPage();
    globalPage.showTitle();
    globalPage.showPageIcon();
    DEBUG_LOG_VALUE("Active page changed to: ", activePage.pageNumber);
}




// ====================================
// Fonctions utilitaires
// ====================================

const char noteNames[12][3] PROGMEM = {
  "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

void getNoteNameWithOctave(uint8_t noteNumber, char* buffer) {
  uint8_t noteIndex = noteNumber % 12;
  int8_t octave = (noteNumber / 12) - 2;
  strcpy_P(buffer, (PGM_P)noteNames[noteIndex]);
  char octaveStr[3];
  itoa(octave, octaveStr, 10);
  strcat(buffer, octaveStr);
}


const char* getActionName(uint8_t pageNumber, uint8_t buttonNum) {
  // Validation des paramètres d'entrée
  if (pageNumber >= 6 || buttonNum >= 5) {  // Utilisation explicite de 5
    return "Invalid";
  }
  
  static char buffer[20];  // Buffer statique pour les noms personnalisés
  static char formattedActionName[40];  // Buffer pour le résultat formaté
  
  const Page& page = pages[pageNumber];  // Référence pour éviter les accès répétés
  int controlNum = page.control_cc[buttonNum];
  const char* actionName;

  if (page.control_custom[buttonNum] == 0) {
    // Recherche optimisée dans le dictionnaire
    actionName = "Unknown";  // Valeur par défaut
    for (int i = 0; i < ACTUAL_ENTRIES; i++) {
      if (button_actions_values[i] == controlNum) {
        actionName = button_actions_keys[i];
        break;
      }
    }
  } else {
    // Construction efficace du nom personnalisé
    uint8_t channel = page.control_ch[buttonNum] + 1;
    
    switch (page.control_type[buttonNum]) {
      case NOTE: {
        char noteNameWithOctave[6];
        getNoteNameWithOctave(controlNum, noteNameWithOctave);
        snprintf(buffer, sizeof(buffer), "%s Ch.%d", noteNameWithOctave, channel);
        break;
      }
      case CC:
        snprintf(buffer, sizeof(buffer), "CC%d Ch.%d", controlNum, channel);
        break;
      case PC:
        snprintf(buffer, sizeof(buffer), "PC%d Ch.%d", controlNum, channel);
        break;
      default:
        strcpy(buffer, "Unknown");
        break;
    }
    actionName = buffer;
  }

  // Optimisation: vérification rapide s'il n'y a pas d'espaces
  const char* spacePtr = strchr(actionName, ' ');
  if (spacePtr == nullptr) {
    return actionName;  // Pas d'espaces, retour direct
  }

  // Transformation optimisée des espaces en sauts de ligne
  size_t len = strlen(actionName);
  if (len >= sizeof(formattedActionName)) {
    len = sizeof(formattedActionName) - 1;  // Évite le débordement
  }
  
  size_t j = 0;
  for (size_t i = 0; i < len; i++) {
    formattedActionName[j++] = (actionName[i] == ' ') ? '\n' : actionName[i];
  }
  formattedActionName[j] = '\0';

  return formattedActionName;
}

uint16_t getActionColor(uint8_t pageNumber, uint8_t buttonNum) {
  // Validation des paramètres
  if (pageNumber >= 6 || buttonNum >= 5) {  // Utilisation explicite de 5
    return defaultBgColor;  // Couleur par défaut pour les paramètres invalides
  }
  
  // Conversion optimisée des couleurs LED
  const auto& led = l[buttonNum + 1];
  return RRRGB888toRGB565(led.r << 1, led.g << 1, led.b << 1);  // Multiplication par 2 avec shift
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
        
        maxSpritesAmnt = other.maxSpritesAmnt;
    }
    return *this;
}

// ====================================
// DISPLAY
// ====================================

// Fonction utilitaire pour éviter la duplication de code
void Page::displayItemByType(displayed_item_t itemType, const char* content) {
  for (int i = 0; i < 3; ++i) {
    if (displayedItem[i] == itemType) {
      (this->*showSpriteFuncs[i])(content, defaultTxtColor);
      break;
    }
  }
}


