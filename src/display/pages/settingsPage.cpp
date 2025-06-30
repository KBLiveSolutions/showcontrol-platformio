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
  switch (item.type) {
    // Serial.println("Converting ItemContent to const char*");
    // Serial.print("Item type: ");
    // Serial.println(item.type);
    case BOOL_TYPE:
      strncpy(buffer, item.boolValue ? "brrrr" : "false", bufSize);
      buffer[bufSize - 1] = '\0';
      return buffer;
    case INT_TYPE:
      snprintf(buffer, bufSize, "%d", item.intValue);
      return buffer;
    case CHAR_PTR_TYPE:
      return item.charPtrValue;
  }
  return "";
}

void SettingsPage::changeSelectedItem(int value){
  if(isEditingValue){
    long unsigned timeDiff = millis() - _now;
    const double base = 1.4; 
    int valueChange =  pow(base, (500.0 / (timeDiff + 1)));
    _now = millis();
    if (valueChange < 1) valueChange = 1;
    valueChange *= value;
    settings.changeSettingsValue(valueChange);
  }
  else {
    selectedItem += value;
    selectedItem = max(selectedItem, 0);
    selectedItem = min(selectedItem, ITEMSAMNT);
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
    BasicSprite* spriteName = settingSprites[i];
    bool framed =  selectedItem == i;
  spriteName->sprite.setColorDepth(16);
  spriteName->sprite.loadFont(FONT20);
  spriteName->sprite.createSprite(spriteName->width, spriteName->height);
  if (framed) spriteName->sprite.drawSmoothRoundRect(0, 0, RADIUS, RADIUS-1,  spriteName->width, spriteName->height, txtColor, defaultBgColor);
  spriteName->sprite.setTextColor(txtColor, defaultBgColor);
  spriteName->sprite.setTextDatum(spriteName->datum);
  spriteName->sprite.drawString(text, (spriteName->width / 2), spriteName->padding, GFXFF);
  spriteName->sprite.unloadFont();
  spriteName->sprite.pushSprite(spriteName->positionX, spriteName->positionY);
  spriteName->sprite.deleteSprite();
}

void SettingsPage::updateSelection(){
  for (int i=0; i<ITEMSAMNT; i++){
    uint16_t color = defaultTxtColor;
    char buf[16];
    showSettingsSprite(i, toConstChar(itemContent[i], buf, sizeof(buf)), color);
  }
}

void SettingsPage::clearSettingsSprites(){
  showSprite("", defaultBgColor, brightnessTxtSprite);
  showSprite("", defaultTxtColor, displayBrightTxtSprite);
  showSprite("", defaultBgColor, ipTxtSprite);
  // showSprite("Port:", defaultTxtColor, portTxtSprite);
  // showSprite("_", defaultBgColor, submaskTxtSprite);      
  for (int i=0; i<ITEMSAMNT; i++){
    showSettingsSprite(i, "", defaultBgColor);
  }
}

void SettingsPage::showSetting(int selectedItem){
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
    // Serial.print("Setting item ");
    // Serial.print(i);
    // Serial.print(": ");
    // Serial.println(buf);
  }
  showSprite("LEDs:", defaultTxtColor, brightnessTxtSprite);
  showSprite("Display:", defaultTxtColor, displayBrightTxtSprite);
  showSprite("IP Address:", defaultTxtColor, ipTxtSprite);
  showSprite("Port:", defaultTxtColor, portTxtSprite);
  showSprite("Save & Exit", defaultTxtColor, exisSettingsSprite);
}

void SettingsPage::showPage() {
  DEBUG_LOGLN("Settings page displayed.");
  for(int i=0; i<ITEMSAMNT; i++){
    showSetting(selectedItem);
    // Serial.print("Showing setting item ");
    // Serial.print(i);
    // Serial.print(": ");
    // Serial.println(toConstChar(itemContent[i], nullptr, 0));
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