#include "splashPage.h"
#include "mainPage.h"
#include "../../core/mainPageParser.h"
#include "../../core/utils.h"
#include "../sprites.h"
#include "../colors.h"
#include "../fonts/fonts.h"

SplashPage splashPage;

void SplashPage::showSplashMainSprite(const char* text, uint16_t txtColor) {
    showSprite(text, txtColor, mainSprite);
}
void SplashPage::showSplashSub1Sprite(const char* text, uint16_t txtColor) {
    showSprite(text, txtColor, sub1Sprite);
}
void SplashPage::showSplashSub2Sprite(const char* text, uint16_t txtColor) {
    showSprite(text, txtColor, sub2Sprite);
}

void SplashPage::showPage() {
    showSplashMainSprite("showcontrol", _White);
    showSpriteHPadding("Push & Hold encoder for Settings", _LightGray, -200);
}

void SplashPage::showSpriteHPadding(const char* text, uint16_t txtColor, int hPadding) {
  ethernetStatusSprite->sprite.setColorDepth(16);
  if (ethernetStatusSprite->txtsize == 4) ethernetStatusSprite->sprite.loadFont(FONT32);
  else if (ethernetStatusSprite->txtsize == 3) ethernetStatusSprite->sprite.loadFont(FONT20);
  else if (ethernetStatusSprite->txtsize == 2) ethernetStatusSprite->sprite.loadFont(FONT12);
  ethernetStatusSprite->sprite.createSprite(ethernetStatusSprite->width, ethernetStatusSprite->height);
  ethernetStatusSprite->sprite.setTextDatum(ethernetStatusSprite->datum);
  ethernetStatusSprite->sprite.setTextColor(txtColor, defaultBgColor);
  ethernetStatusSprite->sprite.drawString(text, (ethernetStatusSprite->width / 2) + hPadding, ethernetStatusSprite->padding, GFXFF);
  ethernetStatusSprite->sprite.unloadFont();
  ethernetStatusSprite->sprite.pushSprite(ethernetStatusSprite->positionX, ethernetStatusSprite->positionY);
  ethernetStatusSprite->sprite.deleteSprite();
}

void SplashPage::clearPage(){
    DEBUG_LOGLN("Clearing splash page");
    showSplashMainSprite(" ", defaultBgColor);
    showSplashSub1Sprite(" ", defaultBgColor);
    showSplashSub2Sprite(" ", defaultBgColor);
    showSpriteHPadding("                                                    ", defaultBgColor, -200);
}