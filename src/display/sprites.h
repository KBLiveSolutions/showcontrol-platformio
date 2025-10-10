#pragma once
#include "TFT_eSPI.h"

#include "../config/consts.h"


struct BasicSprite {
  TFT_eSprite sprite;
  int txtsize;
  int positionX;
  int positionY;
  int height;
  int width;
  int padding;
  int datum;
};

extern BasicSprite* mainSprite;
extern int mainSpriteY;

// TOP BAR
extern BasicSprite* tempoSprite;
extern BasicSprite* signatureSprite;
extern BasicSprite* counterSprite;
extern BasicSprite* modeSprite;
extern BasicSprite* titleSprite;
extern BasicSprite* ETHSprite;
extern BasicSprite* sceneSprite;
extern int pedalSpriteWidth;
extern BasicSprite* PedalSprite[2];

// MAIN AREA
extern int bottomSpriteHeight;
extern int prePostWidth;
extern int prePostHeight;
extern int sub1SpriteY;
extern BasicSprite* sub1Sprite;
extern BasicSprite* sub1PreSprite;
extern BasicSprite* sub1PostSprite;
extern int progressBarY;
extern BasicSprite* progressBarSprite;
extern int timerBarY;
extern BasicSprite* timeSprite;
extern BasicSprite* songsCountSprite;
extern BasicSprite* totalTimeSprite;
extern int sub2SpriteY;
extern BasicSprite* nextSprite;
extern BasicSprite* sub2Sprite;
extern BasicSprite* ethernetStatusSprite;
extern BasicSprite* keySprite;
extern BasicSprite* infoSprite;
extern BasicSprite* playSprite;
extern BasicSprite* loopSprite;
extern BasicSprite* CancelButtonSprite;
extern TFT_eSprite mainmenuSprites[MAX_NUM_PAGES];
extern BasicSprite* mainUserSprite;
extern BasicSprite* sub1UserSprite;
extern BasicSprite* lockSprite;

// USER PAGE
struct ButtonSprite {
  TFT_eSprite sprite;
  int txtsize;
  int positionX;
  int positionY;
  int height;
  int width;
  int padding;
  int datum;
};

extern ButtonSprite* userButtonSprite;
extern int itemWidth;
extern int itemPadding;
extern int itemOffset;
extern int itemHeight;
extern int offsetV;

// SETTINGS PAGE
#define ITEMSAMNT 9
extern BasicSprite* idTxtSprite;
extern BasicSprite* idSprite;
extern BasicSprite* brightnessTxtSprite;
extern BasicSprite* brightnessSprite;
extern BasicSprite* displayBrightTxtSprite;
extern BasicSprite* displayBrtSprite;
extern BasicSprite* ipTxtSprite;
extern BasicSprite* ipSprite[4];
extern BasicSprite* portTxtSprite;
extern BasicSprite* portSprite;
extern BasicSprite* exisSettingsSprite;
extern BasicSprite* settingSprites[ITEMSAMNT];


void showSpriteColor(const char* , uint16_t , uint16_t , BasicSprite* , bool ) ;
void showSprite(const char* , uint16_t , BasicSprite* ) ;
void clearSprite(BasicSprite* );
// void activePage->showPage();
void drawTopLine();