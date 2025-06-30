#pragma once
#include <Arduino.h>


class SplashPage {
    public:
        void showSplashMainSprite(const char* text, uint16_t txtColor);
        void showSplashSub1Sprite(const char* text, uint16_t txtColor);
        void showSplashSub2Sprite(const char* text, uint16_t txtColor);
        void showSpriteHPadding(const char* , uint16_t, int) ;
        void showPage();
        void clearPage();
};

extern SplashPage splashPage;