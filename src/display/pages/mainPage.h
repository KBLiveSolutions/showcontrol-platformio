#pragma once
#include "../sprites.h"

// Variables globales (déclarées extern si utilisées ailleurs)
class MainPage {
public:
    void showMainSprite(const char* , uint16_t);
    void showSub1Sprite(const char* , uint16_t );
    void showSub2Sprite(const char* , uint16_t );
    void showButton(bool , int , const char* , uint16_t , bool );
    void showButtonSprite(bool , int , const char* , uint16_t , bool) ;
    void showMainUserSprite(const char* text, uint16_t txtColor);
    void showSub1UserSprite(const char* text, uint16_t txtColor);

    // Prototypes des fonctions
    void showRemainingTimeInSet(bool);
    void updateProgressBarFine(bool);
    void showNextSprite(bool );
    void showRemainingTimeInSong(bool);
    void showSongsCounter(bool);
};

const char* formatTime(float seconds);
extern MainPage mainPage;