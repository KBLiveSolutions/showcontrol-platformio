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
int itemHeight = 36;
int offsetV = 48;

BasicSprite* brightnessTxtSprite = new BasicSprite { TFT_eSprite(&display), 3, 0,           offsetV + 0 * HEIGHT/8 ,    itemHeight, itemOffset, 8, 1};
BasicSprite* brightnessSprite = new BasicSprite { TFT_eSprite(&display),    3, itemOffset,  offsetV + 0 * HEIGHT/8 ,    itemHeight, WIDTH/2, 8, 1};
BasicSprite* displayBrightTxtSprite = new BasicSprite { TFT_eSprite(&display), 3, 0,        offsetV + 1 * HEIGHT/8 , itemHeight, itemOffset, 8, 1};
BasicSprite* displayBrtSprite = new BasicSprite { TFT_eSprite(&display),    3, itemOffset,  offsetV + 1 * HEIGHT/8 , itemHeight, WIDTH/2, 8, 1};
BasicSprite* ipTxtSprite = new BasicSprite { TFT_eSprite(&display),         3, 0,           offsetV + 2 * HEIGHT/8 , itemHeight, itemOffset, 8, 1};
BasicSprite* ipSprite[4] = {
 new BasicSprite{TFT_eSprite(&display),                                     3, itemOffset,  offsetV + 2 * HEIGHT/8, itemHeight, itemWidth, 8, 1},
 new BasicSprite{TFT_eSprite(&display),                                     3, itemOffset + itemWidth + itemPadding, offsetV + 2 * HEIGHT/8, itemHeight, itemWidth, 8, 1},
 new BasicSprite{TFT_eSprite(&display),                                     3, itemOffset + 2*itemWidth + itemPadding, offsetV + 2 * HEIGHT/8, itemHeight, itemWidth, 8, 1},
 new BasicSprite{TFT_eSprite(&display),                                     3, itemOffset + 3*itemWidth + itemPadding, offsetV + 2 * HEIGHT/8, itemHeight, itemWidth, 8, 1}
};
BasicSprite* portTxtSprite = new BasicSprite { TFT_eSprite(&display),       3, 0,           offsetV + 3 * HEIGHT/8 , itemHeight, itemOffset, 8, 1};
BasicSprite* portSprite = new BasicSprite { TFT_eSprite(&display),          3, itemOffset,  offsetV + 3 * HEIGHT/8, itemHeight, WIDTH/2, 8, 1};
BasicSprite* idTxtSprite = new BasicSprite { TFT_eSprite(&display),         3, 0,           offsetV + 4 * HEIGHT/8 ,    itemHeight, itemOffset, 8, 1};
BasicSprite* idSprite = new BasicSprite { TFT_eSprite(&display),            3, itemOffset,  offsetV + 4 * HEIGHT/8 ,    itemHeight, WIDTH/2, 8, 1};
BasicSprite* exisSettingsSprite = new BasicSprite { TFT_eSprite(&display),  3, WIDTH/4,  HEIGHT - offsetV , itemHeight, WIDTH/2, 8, 1};

BasicSprite* settingSprites[ITEMSAMNT] = {
  brightnessSprite, displayBrtSprite, ipSprite[0], ipSprite[1], ipSprite[2], ipSprite[3], portSprite, idSprite, exisSettingsSprite
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
