#include "globalPage.h"
#include "../colors.h"
#include "../../core/globalParser.h"
#include "../../core/settingsParser.h"
#include "../../core/mainPageParser.h"
#include "../fonts/fonts.h"

GlobalPage globalPage;

void GlobalPage::showTempo() {
  // Protection contre les valeurs invalides
  if (global.tempo < 0 || global.tempo > 999) {
    DEBUG_LOG_VALUE("showTempo: invalid tempo value: ", global.tempo);
    global.tempo = 120;  // Valeur par défaut raisonnable
  }
  
  char buffer[8];  // Buffer agrandi pour sécurité
  int result = snprintf(buffer, sizeof(buffer), "%d", global.tempo);
  
  // Protection contre overflow du buffer
  if (result < 0 || result >= sizeof(buffer)) {
    DEBUG_LOGLN("showTempo: buffer overflow");
    strcpy(buffer, "120");
  }
  
  if (!tempoSprite) {
    DEBUG_LOGLN("showTempo: tempoSprite is null");
    return;
  }
  
  showSprite(buffer, _Amber, tempoSprite);
}

void GlobalPage::showCounter(){  
  // Protection contre les valeurs invalides
  if (global.current_beat < 0 || global.current_beat > 999) {
    DEBUG_LOG_VALUE("showCounter: invalid current_beat: ", global.current_beat);
    global.current_beat = 1;  // Valeur par défaut
  }
  
  char buffer[8];  // Buffer agrandi pour sécurité
  int result = snprintf(buffer, sizeof(buffer), "%d", global.current_beat);
  
  // Protection contre overflow du buffer
  if (result < 0 || result >= sizeof(buffer)) {
    DEBUG_LOGLN("showCounter: buffer overflow");
    strcpy(buffer, "1");
  }
  
  if (!counterSprite) {
    DEBUG_LOGLN("showCounter: counterSprite is null");
    return;
  }
  
  const char* result_str = buffer;
  showSpriteColor(result_str, defaultBgColor, _Amber, counterSprite, global.current_beat != 1);
}

void GlobalPage::showEthSprite(bool midiConnected, int num) {
  // Vérification critique du pointeur sprite
  if (!ETHSprite) {
    DEBUG_LOGLN("showEthSprite: ETHSprite is null");
    return;
  }
  
  // Validation des dimensions
  if (ETHSprite->width <= 0 || ETHSprite->height <= 0) {
    DEBUG_LOGLN("showEthSprite: invalid sprite dimensions");
    return;
  }
  
  // Protection contre les valeurs de num extrêmes
  if (num < -999 || num > 999) {
    DEBUG_LOG_VALUE("showEthSprite: capping extreme num value: ", num);
    num = (num < 0) ? -1 : 999;
  }
  
  uint16_t color = _LightGray;
  if (num == -1) color = _Red;
  else if (num == 0) color = _Orange;
  
  ETHSprite->sprite.createSprite(ETHSprite->width, ETHSprite->height);
  midiConnected = false;
  
  if(midiConnected) {
    ETHSprite->sprite.fillRoundRect(3, 9, 34, 13, 6, _LightGray);
    ETHSprite->sprite.fillRoundRect(5, 11, 30, 9, 4, _Black);
    ETHSprite->sprite.fillRoundRect(8, 14, 24, 3, 2, _LightGray);
  }
  else {
    // Protection contre les coordonnées négatives
    ETHSprite->sprite.drawRoundRect(0, 9, 24, 16, 2, color);
    ETHSprite->sprite.fillRoundRect(1, 10, 22, 14, 2, defaultBgColor);
    ETHSprite->sprite.drawRoundRect(1, 10, 22, 14, 2, color);
    ETHSprite->sprite.fillRect(4, 23, 16, 4, color);
    ETHSprite->sprite.fillRect(5, 12, 2, 3, color);
    ETHSprite->sprite.fillRect(9, 12, 2, 3, color);
    ETHSprite->sprite.fillRect(13, 12, 2, 3, color);
    ETHSprite->sprite.fillRect(17, 12, 2, 3, color);
  }
  
  ETHSprite->sprite.setTextColor(_LightGray, defaultBgColor);
  ETHSprite->sprite.loadFont(FONT20);
  
  // Protection du buffer et sprintf
  char buffer[8];  // Buffer agrandi
  if (num > 0) {
    int result = snprintf(buffer, sizeof(buffer), "%d", num);
    if (result < 0 || result >= sizeof(buffer)) {
      DEBUG_LOGLN("showEthSprite: buffer overflow");
      strcpy(buffer, "?");
    }
    ETHSprite->sprite.drawString(buffer, 32, 8, GFXFF);
  }
  
  ETHSprite->sprite.pushSprite(ETHSprite->positionX, ETHSprite->positionY);
  ETHSprite->sprite.unloadFont();
  ETHSprite->sprite.deleteSprite();
}

void GlobalPage::updateEthSprite(int amount){
  // Protection contre les valeurs extremes
  if (amount < -999 || amount > 999) {
    DEBUG_LOG_VALUE("updateEthSprite: invalid amount value: ", amount);
    amount = (amount < 0) ? -1 : 999;
  }
  
  showEthSprite(settings.MIDIConnected, amount);
}

void GlobalPage::showPageIcon(){
  // Vérification critique du pointeur sprite
  if (!modeSprite) {
    DEBUG_LOGLN("showPageIcon: modeSprite is null");
    return;
  }
  
  // Validation de _main.selectedMode
  if (_main.selectedMode < 0 || _main.selectedMode > 10) {
    DEBUG_LOG_VALUE("showPageIcon: invalid selectedMode: ", _main.selectedMode);
    _main.selectedMode = 0;  // Valeur par défaut
  }
  
  int padding = 6;
  modeSprite->sprite.createSprite(24, 24);
  modeSprite->sprite.setTextDatum(3); 
  
  // Correction de la logique cassée : simplification
  uint16_t color;
  if (_main.selectedMode == 0) {
    color = _Emerald;  // Page Setlist
  } else {
    color = _Purple;   // Pages User
  }
  
  modeSprite->sprite.setTextColor(color, defaultBgColor);
  modeSprite->sprite.drawRect(0, 0, 16, 16, color);
  
  if (_main.selectedMode == 0) {
    // Affichage pour Setlist
    modeSprite->sprite.drawFastHLine(4, 4, 8, color);
    modeSprite->sprite.drawFastHLine(4, 8, 8, color);
    modeSprite->sprite.drawFastHLine(4, 12, 8, color);
  } else {
    // Affichage pour User pages
    char buffer[8];  // Buffer sécurisé
    int result = snprintf(buffer, sizeof(buffer), "U%u", _main.selectedMode);
    if (result < 0 || result >= sizeof(buffer)) {
      DEBUG_LOGLN("showPageIcon: buffer overflow");
      strcpy(buffer, "U?");
    }
    
    // Protection contre topBarHeight non défini
    int yPos = 8;  // Valeur par défaut au lieu de topBarHeight/2 - 8
    modeSprite->sprite.drawString(buffer, 2, yPos, GFXFF);
  }
  
  modeSprite->sprite.pushSprite(modeSprite->positionX, modeSprite->positionY + 8);
  modeSprite->sprite.deleteSprite();
}

void GlobalPage::showTitle(){
  char optionText[32];  // Buffer agrandi pour sécurité
  
  // Validation de _main.selectedMode
  if (_main.selectedMode < 0 || _main.selectedMode > 10) {
    DEBUG_LOG_VALUE("showTitle: invalid selectedMode: ", _main.selectedMode);
    _main.selectedMode = 0;
  }
  
  if (activePage.pageType == SETLIST) {
    strcpy(optionText, "Setlist");
  } 
  else if (activePage.pageType == USER) {
    int result = snprintf(optionText, sizeof(optionText), "User %u", _main.selectedMode);
    if (result < 0 || result >= sizeof(optionText)) {
      DEBUG_LOGLN("showTitle: buffer overflow for User page");
      strcpy(optionText, "User ?");
    }
  }
  else if (activePage.pageType == SETTINGS) {
    strcpy(optionText, "Settings");
  }
  else if (activePage.pageType == MENU) {
    strcpy(optionText, "Menu");
  }
  else if (activePage.pageType == SPLASH) {
    strcpy(optionText, "Splash");
  }
  else {
    // Type de page inconnu
    DEBUG_LOG_VALUE("showTitle: unknown page type: ", activePage.pageType);
    strcpy(optionText, "Unknown");
  }
  
  if (!titleSprite) {
    DEBUG_LOGLN("showTitle: titleSprite is null");
    return;
  }
  
  showSprite(optionText, _LightGray, titleSprite);
}

void GlobalPage::showAudioInterfaceScene(){
  // Validation de la valeur audioInterfaceScene
  if (settings.audioInterfaceScene < 1 || settings.audioInterfaceScene > 2) {
    DEBUG_LOG_VALUE("showAudioInterfaceScene: invalid scene value: ", settings.audioInterfaceScene);
    settings.audioInterfaceScene = 1;  // Valeur par défaut
  }
  
  if (!sceneSprite) {
    DEBUG_LOGLN("showAudioInterfaceScene: sceneSprite is null");
    return;
  }
  
  const char* sceneText = (settings.audioInterfaceScene == 1) ? "A" : "B";
  uint16_t sceneColor = (settings.audioInterfaceScene == 1) ? _Green : _Red;
  
  showSpriteColor(sceneText, defaultBgColor, sceneColor, sceneSprite, false);
}

void GlobalPage::showLock(bool state) {
  // uint16_t color = defaultBgColor;
  // if (state) settings.nightMode ? color = _White : _Black;
  // display.fillRect(12, 14, 12, 12, color);
  // display.drawRoundRect(12, 5, 12, 16, 6, color);
  // display.drawRoundRect(13, 6, 10, 14, 6, color);
}


void GlobalPage::setupGlobalPage() {
  DEBUG_LOGLN("Setting up global page...");
  
  // Initialisation sécurisée de chaque composant
  DEBUG_LOGLN("Showing tempo...");
  showTempo();
  
  DEBUG_LOGLN("Showing counter...");
  showCounter();
  
  DEBUG_LOGLN("Showing page icon...");
  // showPageIcon();
  
  DEBUG_LOGLN("Showing title...");
  // showTitle();
  
  DEBUG_LOGLN("Showing audio interface scene...");
  showAudioInterfaceScene();
  
  // Vérification de l'existence de drawTopLine avant appel
  DEBUG_LOGLN("Drawing top line...");
  drawTopLine(); // Commenté temporairement si cette fonction cause des problèmes
  
  DEBUG_LOGLN("Global page setup completed");
}