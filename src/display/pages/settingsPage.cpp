#include "settingsPage.h"
#include "../../core/mainPageParser.h"
#include "../../core/settingsParser.h"
#include "../../core/pages.h"
#include "../../core/utils.h"
#include "../../leds/leds.h"
#include "../sprites.h"
#include "../colors.h"
#include "../fonts/fonts.h"
#include "../../osc/ethernetManager.h"

SettingsPage settingsPage;


const char* SettingsPage::toConstChar(ItemContent item, char* buffer, size_t bufSize) {
  // Validation du buffer
  if (!buffer || bufSize == 0) {
    DEBUG_LOGLN("toConstChar: invalid buffer");
    return "";
  }
  
  switch (item.type) {
    case BOOL_TYPE:
      strncpy(buffer, item.boolValue ? "true" : "false", bufSize - 1);  // Correction: "true" au lieu de "brrrr"
      buffer[bufSize - 1] = '\0';
      return buffer;
    case INT_TYPE:
      // Protection contre valeurs extrêmes
      if (item.intValue < -999999 || item.intValue > 999999) {
        DEBUG_LOG_VALUE("toConstChar: extreme int value: ", item.intValue);
        snprintf(buffer, bufSize, "ERR");
      } else {
        snprintf(buffer, bufSize, "%d", item.intValue);
      }
      return buffer;
    case CHAR_PTR_TYPE:
      if (item.charPtrValue) {
        return item.charPtrValue;
      } else {
        DEBUG_LOGLN("toConstChar: null charPtrValue");
        return "";
      }
    default:
      DEBUG_LOG_VALUE("toConstChar: unknown type: ", item.type);
      return "";
  }
}

void SettingsPage::changeSelectedItem(int value){
  // Validation de la valeur d'entrée
  if (value < -1000 || value > 1000) {
    DEBUG_LOG_VALUE("changeSelectedItem: extreme value capped: ", value);
    value = (value < 0) ? -1 : 1;
  }
  
  if(isEditingValue){
    long unsigned timeDiff = millis() - _now;
    const double base = 1.4; 
    int valueChange = pow(base, (500.0 / (timeDiff + 1)));
    _now = millis();
    if (valueChange < 1) valueChange = 1;
    
    // Protection contre overflow
    if (valueChange > 100) {
      DEBUG_LOG_VALUE("changeSelectedItem: capping valueChange: ", valueChange);
      valueChange = 100;
    }
    
    valueChange *= value;
    settings.changeSettingsValue(valueChange);
  }
  else {
    selectedItem += value;
    selectedItem = max(selectedItem, 0);
    
    // Protection contre ITEMSAMNT invalide ou non défini
    #ifndef ITEMSAMNT
    #define ITEMSAMNT 10  // Valeur par défaut raisonnable
    #endif
    
    selectedItem = min(selectedItem, ITEMSAMNT - 1);  // Correction: -1 pour index valide
    updateSelection();
  }
}


// const char* toConstChar(const ItemContent& item, char* buffer, size_t bufSize) {
//   switch (item.type) {
//     case BOOL_TYPE:
//       strncpy(buffer, item.boolValue ? "true" : "false", bufSize);
//       buffer[bufSize - 1] = '\0';
//       return buffer;
//     case INT_TYPE:
//       snprintf(buffer, bufSize, "%d", item.intValue);
//       return buffer;
//     case CHAR_PTR_TYPE:
//       if (item.charPtrValue) return item.charPtrValue;
//       else return "";
//     default:
//       return "";
//   }
// }

void SettingsPage::showSettingsSprite(int i, const char* text, uint16_t txtColor) {
    // Validation des paramètres
    if (i < 0 || i >= sizeof(settingSprites)/sizeof(settingSprites[0])) {
        DEBUG_LOG_VALUE("showSettingsSprite: invalid index: ", i);
        return;
    }
    
    if (!text) {
        DEBUG_LOGLN("showSettingsSprite: text is null");
        text = "";
    }
    
    BasicSprite* spriteName = settingSprites[i];
    if (!spriteName) {
        DEBUG_LOG_VALUE("showSettingsSprite: sprite is null at index: ", i);
        return;
    }
    
    // Validation des dimensions du sprite
    if (spriteName->width <= 0 || spriteName->height <= 0) {
        DEBUG_LOGLN("showSettingsSprite: invalid sprite dimensions");
        return;
    }
    
    bool framed = (selectedItem == i);
    spriteName->sprite.setColorDepth(16);
    spriteName->sprite.loadFont(FONT20);
    spriteName->sprite.createSprite(spriteName->width, spriteName->height);
    
    if (framed) {
        // Protection contre les coordonnées négatives
        int rectW = max(spriteName->width, 1);
        int rectH = max(spriteName->height, 1);
        spriteName->sprite.drawSmoothRoundRect(0, 0, RADIUS, RADIUS-1, rectW, rectH, txtColor, defaultBgColor);
    }
    
    spriteName->sprite.setTextColor(txtColor, defaultBgColor);
    spriteName->sprite.setTextDatum(spriteName->datum);
    
    // Protection contre padding invalide
    int textY = max(spriteName->padding, 0);
    
    spriteName->sprite.drawString(text, (spriteName->width / 2), textY, GFXFF);
    spriteName->sprite.unloadFont();
    spriteName->sprite.pushSprite(spriteName->positionX, spriteName->positionY);
    spriteName->sprite.deleteSprite();
}

void SettingsPage::updateSelection(){
  DEBUG_LOGLN("SettingsPage::updateSelection() starting");
  
  // Protection contre ITEMSAMNT invalide
  #ifndef ITEMSAMNT
  #define ITEMSAMNT 10
  #endif
  
  for (int i = 0; i < ITEMSAMNT; i++){
    // Vérification de l'accès au tableau itemContent
    if (i >= sizeof(itemContent)/sizeof(itemContent[0])) {
      DEBUG_LOG_VALUE("updateSelection: itemContent index out of bounds: ", i);
      break;
    }
    
    uint16_t color = defaultTxtColor;
    char buf[32];  // Buffer agrandi pour sécurité
    
    const char* text = toConstChar(itemContent[i], buf, sizeof(buf));
    if (!text) {
      DEBUG_LOG_VALUE("updateSelection: toConstChar returned null for index: ", i);
      text = "ERR";
    }
    
    showSettingsSprite(i, text, color);
  }
  
  DEBUG_LOGLN("SettingsPage::updateSelection() completed");
}

void SettingsPage::clearSettingsSprites(){
  DEBUG_LOGLN("SettingsPage::clearSettingsSprites() starting");
  
  // Vérification des sprites avant usage
  if (brightnessTxtSprite) {
    showSprite("", defaultBgColor, brightnessTxtSprite);
  } else {
    DEBUG_LOGLN("clearSettingsSprites: brightnessTxtSprite is null");
  }
  
  if (displayBrightTxtSprite) {
    showSprite("", defaultTxtColor, displayBrightTxtSprite);
  } else {
    DEBUG_LOGLN("clearSettingsSprites: displayBrightTxtSprite is null");
  }
  
  if (ipTxtSprite) {
    showSprite("", defaultBgColor, ipTxtSprite);
  } else {
    DEBUG_LOGLN("clearSettingsSprites: ipTxtSprite is null");
  }
  
    if (idTxtSprite) {
    showSprite("", defaultBgColor, idTxtSprite);
  } else {
    DEBUG_LOGLN("clearSettingsSprites: ipTxtSprite is null");
  }
  
  // showSprite("Port:", defaultTxtColor, portTxtSprite);
  // showSprite("_", defaultBgColor, submaskTxtSprite);      
  
  // Protection contre ITEMSAMNT invalide
  #ifndef ITEMSAMNT
  #define ITEMSAMNT 10
  #endif
  
  for (int i = 0; i < ITEMSAMNT; i++){
    showSettingsSprite(i, "", defaultBgColor);
  }
  
  DEBUG_LOGLN("SettingsPage::clearSettingsSprites() completed");
}

void SettingsPage::showSetting(int selectedItem){
  showSprite("Id Number: ", defaultTxtColor, idTxtSprite);
  showSprite("LEDs:", defaultTxtColor, brightnessTxtSprite);
  showSprite("Display:", defaultTxtColor, displayBrightTxtSprite);
  showSprite("IP Address:", defaultTxtColor, ipTxtSprite);
  showSprite("Port:", defaultTxtColor, portTxtSprite);
  showSprite("Save & Exit", defaultTxtColor, exisSettingsSprite);
  // showSprite("Submask:", defaultTxtColor, submaskTxtSprite);
  for (int i=0; i<ITEMSAMNT; i++){
    switch (i) {
      case 0:
        itemContent[i].intValue = settings.ledBrightness;
        break;
      case 1:
        itemContent[i].intValue = settings.displayBrightness;
        break;
      case 2:
        itemContent[i].intValue = manualIP[0];
        break;
      case 3:
        itemContent[i].intValue = manualIP[1];
        break;
      case 4:
        itemContent[i].intValue = manualIP[2];
        break;
      case 5:
        itemContent[i].intValue = manualIP[3];
        break;
      case 6:
        itemContent[i].intValue = showcontrolLocalPort;
        break;
      case 7:
        itemContent[i].intValue = settings.idNumber;
        break;
      case 8:
        itemContent[i].charPtrValue = "Save & Exit";
        break;
      // case 8:
      //   itemContent[i].intValue = subnet[1];
      //   break;
      // case 9:
      //   itemContent[i].intValue = subnet[2];
      //   break;
      // case 10:
      //   itemContent[i].intValue = subnet[3];
      //   break;
    }
    char buf[16];
    showSettingsSprite(i, toConstChar(itemContent[i], buf, sizeof(buf)), defaultTxtColor);
  }
}

void SettingsPage::showPage() {
  DEBUG_LOGLN("Settings page displayed.");
  for(int i=0; i<ITEMSAMNT; i++){
    showSetting(selectedItem);
  }
  isEditingValue = false;
}

void SettingsPage::clearPage(){
  clearSettingsSprites();
  showSprite("", defaultBgColor, brightnessTxtSprite);
  showSprite("", defaultBgColor, displayBrightTxtSprite);
  showSprite("", defaultBgColor, ipTxtSprite);
  showSprite("", defaultBgColor, portTxtSprite);
  // showSprite("", defaultBgColor, submaskTxtSprite);
  selectedItem = 0;
  isEditingValue = false;
  DEBUG_LOGLN("Settings page cleared.");
  for (int i=0; i<ITEMSAMNT; i++){
    showSettingsSprite(i, "", defaultBgColor);
  }
}