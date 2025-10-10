#pragma once
#include <Arduino.h>
#include "../sprites.h"


class GlobalPage {
public:
    void showTempo();
    void showCounter();
    void showAudioInterfaceScene();
    void showEthSprite(bool state, int) ;
    void showTitle();
    // void showPageIcon();
    void setupGlobalPage();
    void showLock(bool );
};

extern GlobalPage globalPage;