#include "sprites.h"
#include "displaySetup.h"
#include "colors.h"
#include "../config/consts.h"
#include "../core/settingsParser.h"
// #include "pages/mainPage.h"
#include "fonts/fonts.h"

BasicSprite* mainSprite = new BasicSprite {TFT_eSprite(&display), 4, 0, topBarHeight + 24, 48, WIDTH, 0, 1};

int mainSpriteY = topBarHeight * 2;
BasicSprite* tempoSprite = new BasicSprite { TFT_eSprite(&display), 3, 2, 0, topBarHeight, 80, 18, 4 };
BasicSprite* signatureSprite = new BasicSprite { TFT_eSprite(&display), 2, tempoSprite->width, 0, topBarHeight, 64, 18, 4 };
BasicSprite* counterSprite = new BasicSprite { TFT_eSprite(&display), 3, 64 + 24, 0, topBarHeight, 32, 18, 4};
BasicSprite* modeSprite = new BasicSprite { TFT_eSprite(&display), 2, 144, 0, topBarHeight, 24, 18, 5 };
BasicSprite* titleSprite = new BasicSprite { TFT_eSprite(&display), 3, WIDTH / 2 - 70, 0, topBarHeight, 140, 18, 4 };
BasicSprite* ETHSprite = new BasicSprite { TFT_eSprite(&display), 2, WIDTH - 48, 0, topBarHeight, 48, 18, 3};
BasicSprite* sceneSprite = new BasicSprite { TFT_eSprite(&display), 3, WIDTH - 88, 0, topBarHeight, 32, 18, 4};
int pedalSpriteWidth = 16;
BasicSprite* PedalSprite[2] = {
  new BasicSprite{ TFT_eSprite(&display), 1, WIDTH - ETHSprite->width - 2 * pedalSpriteWidth + 8, -8, topBarHeight, 15, pedalSpriteWidth, 4 },
  new BasicSprite{ TFT_eSprite(&display), 1, WIDTH - ETHSprite->width - pedalSpriteWidth + 8, -8, topBarHeight, 16, pedalSpriteWidth, 4 }
};
int bottomSpriteHeight = HEIGHT - topBarHeight - 5;

int prePostWidth = 96;
int prePostHeight = 32;
int sub1SpriteY = mainSprite->positionY + mainSprite->height + 8;
BasicSprite* sub1Sprite = new BasicSprite { TFT_eSprite(&display), 3, WIDTH / 4, sub1SpriteY, prePostHeight, WIDTH / 2, 0, 1};
BasicSprite* sub1PreSprite = new BasicSprite { TFT_eSprite(&display), 3, 0, sub1SpriteY, prePostHeight, prePostWidth, 0, 1};
BasicSprite* sub1PostSprite = new BasicSprite { TFT_eSprite(&display), 3, 2 * WIDTH / 4, sub1SpriteY, prePostHeight, prePostWidth, 0, 1};

int progressBarY = sub1Sprite->positionY  + sub1Sprite->height + 8;
BasicSprite* progressBarSprite = new BasicSprite { TFT_eSprite(&display), 3, 0, progressBarY, 16, WIDTH };
int timerBarY = progressBarY + 40;
BasicSprite* timeSprite = new BasicSprite { TFT_eSprite(&display), 3, WIDTH/2 - 48, timerBarY, 32, 96, 8, 1 };
BasicSprite* songsCountSprite = new BasicSprite { TFT_eSprite(&display), 3, WIDTH - 88, timerBarY, 32, 72, 8, 1 };
BasicSprite* totalTimeSprite = new BasicSprite { TFT_eSprite(&display), 3, WIDTH - 80, timerBarY + 40, 32, 72, 8, 1 };

int sub2SpriteY = progressBarSprite->positionY + progressBarSprite->height + 104;
BasicSprite* nextSprite = new BasicSprite { TFT_eSprite(&display), 3, 20, sub2SpriteY, 32, 72, 8, 1 };
BasicSprite* sub2Sprite = new BasicSprite { TFT_eSprite(&display), 3, WIDTH/4, sub2SpriteY, 32, WIDTH/2, 8, 1 };
BasicSprite* playSprite = new BasicSprite { TFT_eSprite(&display), 3, WIDTH - 96, sub2SpriteY - 8, 48, 48, 8, 1};
BasicSprite* loopSprite = new BasicSprite { TFT_eSprite(&display), 3, WIDTH - 56, sub2SpriteY - 8, 48, 48, 8, 1};

BasicSprite* ethernetStatusSprite = new BasicSprite { TFT_eSprite(&display), 3, 0, sub2SpriteY, 32, WIDTH, 8, 0 };

BasicSprite* keySprite = new BasicSprite { TFT_eSprite(&display), 2, 2, bottomSpriteHeight, topBarHeight, 70, 15, 4 };
BasicSprite* infoSprite = new BasicSprite { TFT_eSprite(&display), 3, 16, timerBarY + 40, 32, WIDTH-32, 8, 1 };

BasicSprite* lockSprite = new BasicSprite { TFT_eSprite(&display), 3, 16, timerBarY, 32, 32, 8, 1 };

BasicSprite*  CancelButtonSprite =  new BasicSprite { TFT_eSprite(&display), 3, 16, HEIGHT / 2 - 16, 32, 32, 8, 1 };

TFT_eSprite mainmenuSprites[MAX_NUM_PAGES] = {
  TFT_eSprite(&display), TFT_eSprite(&display), TFT_eSprite(&display),
  TFT_eSprite(&display), TFT_eSprite(&display), TFT_eSprite(&display),
  TFT_eSprite(&display), TFT_eSprite(&display), TFT_eSprite(&display)
};

BasicSprite* mainUserSprite = new BasicSprite { TFT_eSprite(&display), 4, 0, topBarHeight + 72, 48, WIDTH, 0, 1};
BasicSprite* sub1UserSprite = new BasicSprite { TFT_eSprite(&display), 3, 0, topBarHeight + 96 + 48, prePostHeight, WIDTH, 8, 1};
ButtonSprite* userButtonSprite = new ButtonSprite {TFT_eSprite(&display), 3, 0, HEIGHT, HEIGHT / 6, WIDTH / 6, 0, 4};

int itemWidth = 48;
int itemPadding = 4;
int itemOffset = WIDTH / 4 + 64;

BasicSprite* brightnessTxtSprite = new BasicSprite { TFT_eSprite(&display), 3, 0, 64, 36, itemOffset, 8, 1};
BasicSprite* brightnessSprite = new BasicSprite { TFT_eSprite(&display), 3, itemOffset, 64, 36, WIDTH/2, 8, 1};
BasicSprite* displayBrightTxtSprite = new BasicSprite { TFT_eSprite(&display), 3, 0, 64 + HEIGHT/6, 36, itemOffset, 8, 1};
BasicSprite* displayBrtSprite = new BasicSprite { TFT_eSprite(&display), 3, itemOffset, 64 + HEIGHT/6 , 36, WIDTH/2, 8, 1};
BasicSprite* ipTxtSprite = new BasicSprite { TFT_eSprite(&display), 3, 0,  64 + 2 * HEIGHT/6 , 36, itemOffset, 8, 1};
BasicSprite* ipSprite[4] = {
 new BasicSprite{TFT_eSprite(&display), 3, itemOffset, 64 + 2 * HEIGHT/6, 36, itemWidth, 8, 1},
 new BasicSprite{TFT_eSprite(&display), 3, itemOffset + itemWidth + itemPadding, 64 + 2 * HEIGHT/6, 36, itemWidth, 8, 1},
 new BasicSprite{TFT_eSprite(&display), 3, itemOffset + 2*itemWidth + itemPadding, 64 + 2 * HEIGHT/6, 36, itemWidth, 8, 1},
new BasicSprite{TFT_eSprite(&display), 3, itemOffset + 3*itemWidth + itemPadding, 64 + 2 * HEIGHT/6, 36, itemWidth, 8, 1}
};
BasicSprite* portTxtSprite = new BasicSprite { TFT_eSprite(&display), 3, 0, 64 + 3 * HEIGHT/6 , 36, itemOffset, 8, 1};
BasicSprite* portSprite = new BasicSprite { TFT_eSprite(&display), 3, itemOffset, 64 + 3 * HEIGHT/6, 36, WIDTH/2, 8, 1};
BasicSprite* exisSettingsSprite = new BasicSprite { TFT_eSprite(&display), 3, itemOffset, 64 + 4 * HEIGHT/6, 36, WIDTH/2, 8, 1};

BasicSprite* settingSprites[ITEMSAMNT] = {
  brightnessSprite, displayBrtSprite, ipSprite[0], ipSprite[1], ipSprite[2], ipSprite[3], portSprite, exisSettingsSprite
};


void showSpriteColor(const char* text, uint16_t txtColor, uint16_t bgColor, BasicSprite* spriteName, bool inverted) {
  int padding = 30;
  spriteName->sprite.setColorDepth(16);
  if (spriteName->txtsize == 4) spriteName->sprite.loadFont(FONT32);
  else if (spriteName->txtsize == 3) spriteName->sprite.loadFont(FONT20);
  else if (spriteName->txtsize == 2) spriteName->sprite.loadFont(FONT12);
  spriteName->sprite.createSprite(spriteName->width, spriteName->height);
  if (inverted){
    spriteName->sprite.drawSmoothRoundRect(0, 0, RADIUS, RADIUS-1,  spriteName->width, spriteName->height, bgColor, TFT_BLACK);
    
    // drawRoundRect(0, 0, spriteName->width, spriteName->height, 6, bgColor);
    spriteName->sprite.setTextColor(bgColor, defaultBgColor);
  }
  else{
    spriteName->sprite.fillSmoothRoundRect(0, 0, spriteName->width, spriteName->height, 6, bgColor, TFT_BLACK);
    spriteName->sprite.setTextColor(defaultBgColor, defaultBgColor);
  }
  spriteName->sprite.setTextDatum(spriteName->datum);
  spriteName->sprite.drawString(text, (spriteName->width / 2), spriteName->padding, GFXFF);
  spriteName->sprite.unloadFont();
  spriteName->sprite.pushSprite(spriteName->positionX, spriteName->positionY);
  spriteName->sprite.deleteSprite();
}


void showSprite(const char* text, uint16_t txtColor, BasicSprite* spriteName) {
  spriteName->sprite.setColorDepth(16);
  if (spriteName->txtsize == 4) spriteName->sprite.loadFont(FONT32);
  else if (spriteName->txtsize == 3) spriteName->sprite.loadFont(FONT20);
  else if (spriteName->txtsize == 2) spriteName->sprite.loadFont(FONT12);
  spriteName->sprite.createSprite(spriteName->width, spriteName->height);
  spriteName->sprite.setTextDatum(spriteName->datum);
  spriteName->sprite.setTextColor(txtColor, defaultBgColor);
  spriteName->sprite.drawString(text, (spriteName->width / 2), spriteName->padding, GFXFF);
  spriteName->sprite.unloadFont();
  spriteName->sprite.pushSprite(spriteName->positionX, spriteName->positionY);
  spriteName->sprite.deleteSprite();
}


  void clearSprite(BasicSprite* spriteName){
    showSprite(" ", defaultBgColor, spriteName);
  }


void drawTopLine(){
  display.drawFastHLine(0, topBarHeight+5, WIDTH, _LightGray);
  display.drawFastHLine(0, topBarHeight+6, WIDTH, _LightGray);
}


// TOP BAR SPRITES

// void showTempo(int tempo) {
//   char tempoText[8];
//   if (tempo < 0) {
//     strcpy(tempoText, "N/A");
//   } else {
//     snprintf(tempoText, sizeof(tempoText), "%d", tempo);
//   }
//   showSprite(tempoText, _Amber, tempoSprite);
// }

// void showCounter(int global.current_beat) {
//   if (global.current_beat < 1) {
//     global.current_beat = 1; // Ensure global.current_beat is at least 1
//   }
//   char buffer[4];
//   sprintf(buffer, "%d", global.current_beat);
//   const char* result = buffer;
//   showSpriteColor(result, defaultBgColor, _Amber, counterSprite, global.current_beat!=1) ;
// }

// void globalPage.showPageIcon(){
//   int padding = 6;
//   // showSpriteColor(itemToDisplay[10], defaultTxtColor, Blue, tempoSprite);
//     modeSprite->sprite.createSprite(24, 24);
//     modeSprite->sprite.setTextDatum(3); 
//     uint16_t color = mainPage.selectedMode == 0 ? _SetlistBlue : _Purple;
//     if (mainPage.selectedMode == 0) color = _Emerald;
//     modeSprite->sprite.setTextColor(color, defaultBgColor);
//     modeSprite->sprite.drawRect(0, 0, 16, 16, color);
//     if (mainPage.selectedMode == 0) {
//       modeSprite->sprite.drawFastHLine(4, 4, 8, color);
//       modeSprite->sprite.drawFastHLine(4, 8, 8, color);
//       modeSprite->sprite.drawFastHLine(4, 12, 8, color);
//     }
//     else if (mainPage.selectedMode == 0) {
//       modeSprite->sprite.drawString("S", 4, topBarHeight/2 - 8, GFXFF);
//     }
//     else {
//       char buffer[20];
//       snprintf(buffer, sizeof(buffer), "U%u", mainPage.selectedMode);
//       modeSprite->sprite.drawString(buffer, 2, topBarHeight/2 - 8, GFXFF);
//     }
//     modeSprite->sprite.pushSprite(modeSprite->positionX, modeSprite->positionY + 8);
//     modeSprite->sprite.deleteSprite();

// }

// void globalPage.showTitle(char* optionText){
//   // char optionText[20];
//   // snprintf(optionText, sizeof(optionText), "User %u", mainPage.selectedMode);
//   // if (mainPage.selectedMode == 0) strcpy(optionText, itemToDisplay[indexOfItem(ActiveSetlist)]);
//   // else if (activePage.pageType == SETTINGS) strcpy(optionText, "Settings");
//   showSprite(optionText, _LightGray, titleSprite);
// }

// void showAudioInterfaceScene(int settings.audioInterfaceScene){
//   showSpriteColor((settings.audioInterfaceScene==1) ? "A":"B", defaultBgColor, (settings.audioInterfaceScene==1) ? _Green:_Red, sceneSprite, false) ;
// }

// void globalPage.showEthSprite(bool state, uint8_t num, uint16_t txtColor, BasicSprite* spriteName) {
//   spriteName->sprite.createSprite(spriteName->width, spriteName->height);
//   uint16_t color = txtColor;
//   if(state)  {
//     spriteName->sprite.fillRoundRect(3, 9, 34, 13, 6, _LightGray);
//     spriteName->sprite.fillRoundRect(5, 11, 30, 9, 4, _Black);
//     spriteName->sprite.fillRoundRect(8, 14, 24, 3, 2, _LightGray);
//   }
//   else {
//       spriteName->sprite.drawRoundRect(0, 9, 24, 16, 2, color);
//       spriteName->sprite.fillRoundRect(1, 10, 22, 14, 2, defaultBgColor);
//       spriteName->sprite.drawRoundRect(1, 10, 22, 14, 2, color);
//       spriteName->sprite.fillRect(4, 23, 16, 4, color);
//       spriteName->sprite.fillRect(5, 12, 2, 3, color);
//       spriteName->sprite.fillRect(9, 12, 2, 3, color);
//       spriteName->sprite.fillRect(13, 12, 2, 3, color);
//       spriteName->sprite.fillRect(17, 12, 2, 3, color);
//   }  
//   spriteName->sprite.setTextColor(_LightGray, defaultBgColor);
//   spriteName->sprite.loadFont(FONT20);
//   char buffer[4];  
//   sprintf(buffer, "%d", num); 
//   if (num>0) spriteName->sprite.drawString(buffer, 32, 8, GFXFF);
//   spriteName->sprite.pushSprite(spriteName->positionX, spriteName->positionY);
//   spriteName->sprite.unloadFont();
//   spriteName->sprite.deleteSprite();
// }

// void globalPage.updateEthSprite(int amount){
//   uint16_t color = _Red;
//   if (mainPage.selectedMode == 0) {
//      color = _LightGray;
//   }
//   globalPage.showEthSprite(settings.MIDIConnected, amount, color, ETHSprite);
// }

// MAIN PAGE SPRITES


