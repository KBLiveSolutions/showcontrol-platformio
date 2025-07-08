#include "splashPage.h"
#include "mainPage.h"
#include "../../core/mainPageParser.h"
#include "../../core/utils.h"
#include "../sprites.h"
#include "../colors.h"
#include "../fonts/fonts.h"

SplashPage splashPage;

void SplashPage::showSplashMainSprite(const char* text, uint16_t txtColor) {
    if (!text) {
        DEBUG_LOGLN("showSplashMainSprite: text is null");
        return;
    }
    
    if (!mainSprite) {
        DEBUG_LOGLN("showSplashMainSprite: mainSprite is null");
        return;
    }
    
    showSprite(text, txtColor, mainSprite);
}

void SplashPage::showSplashSub1Sprite(const char* text, uint16_t txtColor) {
    if (!text) {
        DEBUG_LOGLN("showSplashSub1Sprite: text is null");
        return;
    }
    
    if (!sub1Sprite) {
        DEBUG_LOGLN("showSplashSub1Sprite: sub1Sprite is null");
        return;
    }
    
    showSprite(text, txtColor, sub1Sprite);
}

void SplashPage::showSplashSub2Sprite(const char* text, uint16_t txtColor) {
    if (!text) {
        DEBUG_LOGLN("showSplashSub2Sprite: text is null");
        return;
    }
    
    if (!sub2Sprite) {
        DEBUG_LOGLN("showSplashSub2Sprite: sub2Sprite is null");
        return;
    }
    
    showSprite(text, txtColor, sub2Sprite);
}

void SplashPage::showPage() {
    DEBUG_LOGLN("SplashPage::showPage() starting");
    showSplashMainSprite("showcontrol", _White);
    showSpriteHPadding("Push & Hold encoder for Settings", _LightGray, -200);
    DEBUG_LOGLN("SplashPage::showPage() completed");
}

void SplashPage::showSpriteHPadding(const char* text, uint16_t txtColor, int hPadding) {
  // Validation des paramètres
  if (!text) {
    DEBUG_LOGLN("showSpriteHPadding: text is null");
    return;
  }
  
  if (!ethernetStatusSprite) {
    DEBUG_LOGLN("showSpriteHPadding: ethernetStatusSprite is null");
    return;
  }
  
  // Validation des dimensions du sprite
  if (ethernetStatusSprite->width <= 0 || ethernetStatusSprite->height <= 0) {
    DEBUG_LOGLN("showSpriteHPadding: invalid sprite dimensions");
    return;
  }
  
  // Protection contre hPadding extrême
  if (hPadding < -1000 || hPadding > 1000) {
    DEBUG_LOG_VALUE("showSpriteHPadding: extreme hPadding value capped: ", hPadding);
    hPadding = (hPadding < 0) ? -200 : 200;
  }
  
  ethernetStatusSprite->sprite.setColorDepth(16);
  
  // Validation et chargement sécurisé des polices
  if (ethernetStatusSprite->txtsize == 4) {
    ethernetStatusSprite->sprite.loadFont(FONT32);
  }
  else if (ethernetStatusSprite->txtsize == 3) {
    ethernetStatusSprite->sprite.loadFont(FONT20);
  }
  else if (ethernetStatusSprite->txtsize == 2) {
    ethernetStatusSprite->sprite.loadFont(FONT12);
  }
  else {
    DEBUG_LOG_VALUE("showSpriteHPadding: unknown txtsize, using default: ", ethernetStatusSprite->txtsize);
    ethernetStatusSprite->sprite.loadFont(FONT20);  // Police par défaut
  }
  
  ethernetStatusSprite->sprite.createSprite(ethernetStatusSprite->width, ethernetStatusSprite->height);
  ethernetStatusSprite->sprite.setTextDatum(ethernetStatusSprite->datum);
  ethernetStatusSprite->sprite.setTextColor(txtColor, defaultBgColor);
  
  // Protection contre padding invalide et calcul sécurisé de position
  int textX = (ethernetStatusSprite->width / 2) + hPadding;
  int textY = max(ethernetStatusSprite->padding, 0);
  
  ethernetStatusSprite->sprite.drawString(text, textX, textY, GFXFF);
  ethernetStatusSprite->sprite.unloadFont();
  ethernetStatusSprite->sprite.pushSprite(ethernetStatusSprite->positionX, ethernetStatusSprite->positionY);
  ethernetStatusSprite->sprite.deleteSprite();
}

void SplashPage::clearPage(){
    DEBUG_LOGLN("Clearing splash page");
    
    // Nettoyage sécurisé avec vérification des sprites
    showSplashMainSprite(" ", defaultBgColor);
    showSplashSub1Sprite(" ", defaultBgColor);
    showSplashSub2Sprite(" ", defaultBgColor);
    
    // Nettoyage avec une chaîne sécurisée de longueur raisonnable
    const char* clearText = "                                        ";  // 40 espaces
    showSpriteHPadding(clearText, defaultBgColor, -200);
    
    DEBUG_LOGLN("Splash page cleared");
}