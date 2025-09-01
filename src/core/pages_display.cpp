
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

void Page::updateSongIndex(){
  if(pageType == SETLIST){
    mainPage.showSongsCounter(true);
    if (_main.activeSongIndex == 0) {
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
    if (_main.activeSongIndex == _main.songsListSize - 1) {
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
      // Vérification de sécurité pour l'accès au tableau showSpriteFuncs[5]
      if (i < 5) {
        (this->*showSpriteFuncs[i])(content, defaultTxtColor);
      }
      break;
    }
  }
}