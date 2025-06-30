#include "globalPage.h"
#include "../colors.h"
#include "../../core/globalParser.h"
#include "../../core/settingsParser.h"
#include "../../core/mainPageParser.h"
#include "../fonts/fonts.h"

GlobalPage globalPage;

void GlobalPage::showTempo() {
  char buffer[4];
  sprintf(buffer, "%d", global.tempo);
  showSprite(buffer, _Amber, tempoSprite);
}

void GlobalPage::showCounter(){  
  char buffer[4];
  sprintf(buffer, "%d", global.current_beat);
  const char* result = buffer;
  showSpriteColor(result, defaultBgColor, _Amber, counterSprite, global.current_beat!=1) ;
}

void GlobalPage::showEthSprite(bool midiConnected, int num) {
  uint16_t color = _LightGray;
  if (num ==-1) color = _Red;
  else if (num == 0) color = _Orange;
  ETHSprite->sprite.createSprite(ETHSprite->width, ETHSprite->height);
  midiConnected = false;
  if(midiConnected)  {
    ETHSprite->sprite.fillRoundRect(3, 9, 34, 13, 6, _LightGray);
    ETHSprite->sprite.fillRoundRect(5, 11, 30, 9, 4, _Black);
    ETHSprite->sprite.fillRoundRect(8, 14, 24, 3, 2, _LightGray);
  }
  else {
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
  char buffer[4];  
  sprintf(buffer, "%d", num); 
  if (num>0) ETHSprite->sprite.drawString(buffer, 32, 8, GFXFF);
  ETHSprite->sprite.pushSprite(ETHSprite->positionX, ETHSprite->positionY);
  ETHSprite->sprite.unloadFont();
  ETHSprite->sprite.deleteSprite();
}

void GlobalPage::updateEthSprite(int amount){
  showEthSprite(settings.MIDIConnected, amount);
}

void GlobalPage::showPageIcon(){
  int padding = 6;
  // showSpriteColor(itemToDisplay[10], defaultTxtColor, Blue, global.tempoSprite);
    modeSprite->sprite.createSprite(24, 24);
    modeSprite->sprite.setTextDatum(3); 
    uint16_t color = _main.selectedMode == 0 ? _SetlistBlue : _Purple;
    if (_main.selectedMode == 0) color = _Emerald;
    modeSprite->sprite.setTextColor(color, defaultBgColor);
    modeSprite->sprite.drawRect(0, 0, 16, 16, color);
    if (_main.selectedMode == 0) {
      modeSprite->sprite.drawFastHLine(4, 4, 8, color);
      modeSprite->sprite.drawFastHLine(4, 8, 8, color);
      modeSprite->sprite.drawFastHLine(4, 12, 8, color);
    }
    else if (_main.selectedMode == 0) {
      modeSprite->sprite.drawString("S", 4, topBarHeight/2 - 8, GFXFF);
    }
    else {
      char buffer[20];
      snprintf(buffer, sizeof(buffer), "U%u", _main.selectedMode);
      modeSprite->sprite.drawString(buffer, 2, topBarHeight/2 - 8, GFXFF);
    }
    modeSprite->sprite.pushSprite(modeSprite->positionX, modeSprite->positionY + 8);
    modeSprite->sprite.deleteSprite();

}

void GlobalPage::showTitle(){
  char optionText[20];
  if (activePage.pageType == SETLIST) strcpy(optionText, "Setlist");
  else if (activePage.pageType == USER) snprintf(optionText, sizeof(optionText), "User %u", _main.selectedMode);
  // if (_main.selectedMode == 0) strcpy(optionText, itemToDisplay[indexOfItem(ActiveSetlist)]);
  else if (activePage.pageType == SETTINGS) strcpy(optionText, "Settings");
  showSprite(optionText, _LightGray, titleSprite);
}

void GlobalPage::showAudioInterfaceScene(){
  showSpriteColor((settings.audioInterfaceScene==1) ? "A":"B", defaultBgColor, (settings.audioInterfaceScene==1) ? _Green:_Red, sceneSprite, false) ;
}

void GlobalPage::showLock(bool state) {
  // uint16_t color = defaultBgColor;
  // if (state) settings.nightMode ? color = _White : _Black;
  // display.fillRect(12, 14, 12, 12, color);
  // display.drawRoundRect(12, 5, 12, 16, 6, color);
  // display.drawRoundRect(13, 6, 10, 14, 6, color);
}


void GlobalPage::setupGlobalPage() {
  showTempo();
  showCounter();
  // showEthSprite(settings.MIDIConnected, 0, _Red);
  showPageIcon();
  showTitle();
  showAudioInterfaceScene();
  drawTopLine();
}