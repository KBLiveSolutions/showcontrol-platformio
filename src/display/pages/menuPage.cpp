#include "menuPage.h"
#include "../../display/sprites.h"
#include "../../config/jsonManager.h"
#include "../colors.h"
#include "../../config/consts.h"
#include "../../core/utils.h"
#include "../fonts/fonts.h"

MenuPage menuPage;

void MenuPage::showMenuEntry(const char* optionText, uint8_t i, uint8_t menuItemIndex){
    int16_t padding = 5;
    uint8_t border = 0;
    uint16_t activeBGColor = _Green;
    uint16_t inactiveBGColor = _White;
    uint16_t color = inactiveBGColor;
    uint8_t spriteY = spriteH;
    mainmenuSprites[i].setColorDepth(16);
    mainmenuSprites[i].createSprite(spriteW, spriteH);
    mainmenuSprites[i].setTextDatum(1);
    mainmenuSprites[i].setTextColor(_White, defaultBgColor);
    mainmenuSprites[i].loadFont(FONT20);
    if (menuItemIndex == activeMenuItem) border = 3;
    mainmenuSprites[i].drawSmoothRoundRect(padding, padding, RADIUS, RADIUS-border, spriteW - padding * 2 - 2, spriteH - padding * 2 - 2, _White);
    mainmenuSprites[i].drawString(optionText, spriteW / 2, 3 * padding, GFXFF);
    mainmenuSprites[i].unloadFont();
    mainmenuSprites[i].pushSprite(WIDTH / 2 - spriteW / 2, spriteH * (i+1));
    mainmenuSprites[i].deleteSprite();
}

void MenuPage::updateMainMenu() {
    if (activeMenuSize > 5) {
        if (activeMenuItem >= startIndex + 5) {
            startIndex = activeMenuItem - 4;
        } else if (activeMenuItem < startIndex) {
            startIndex = activeMenuItem;
        }
    } else {
        startIndex = 0;
    }

    for (uint8_t i = 0; i < activeMenuSize; i++) {
        uint8_t menuItemIndex = startIndex + i;
        if (menuItemIndex > activeMenuSize - 1) break;
        const char* optionText = menuItems[menuItemIndex];
        showMenuEntry(optionText, i, menuItemIndex);
    }
}

void MenuPage::changeSelectedItem(int value){
    int prevActiveMenuItem = activeMenuItem;
    activeMenuItem = activeMenuItem + value;
    activeMenuItem = max(activeMenuItem, 0);
    activeMenuItem = min(activeMenuItem, activeMenuSize);
    DEBUG_LOG_VALUE("Active menu item: ", activeMenuItem);
    if (prevActiveMenuItem != activeMenuItem) updateMainMenu();
}

void MenuPage::showPage() {
    switch (activeMenu) {
        case MAIN_MENU:
            activeMenuSize = settings.userPagesAmount + 2;
            DEBUG_LOG_VALUE("Active menu size: ", activeMenuSize);
            for (uint8_t i = 0; i < activeMenuSize - 1; i++) {
                if (i == 0) {
                    strcpy(menuItems[i], "Setlist");
                    // TO DO: check if AbleSet is available
                    // if (ethernet.serviceCounts[0] > 0)  strcpy(menuItems[i], "AbleSet");
                    // else  strcpy(menuItems[i], "Setlist");
                } 
                else {
                    strcpy(menuItems[i], modesList[i]);
                }
            }
            strcpy(menuItems[activeMenuSize-1], "Settings");
            break;

        case SONG_MENU:
            activeMenuSize = _main.songsListSize;
            for (uint8_t i = 0; i < activeMenuSize; i++) {
                strcpy(menuItems[i], _main.songsList[i]);
            }
            break;

        case SETLIST_MENU:
            activeMenuSize = _main.setlistsListSize;
            for (uint8_t i = 0; i < activeMenuSize; i++) {
                strcpy(menuItems[i], _main.setlistsList[i]);
            }
            break;
    }

    updateMainMenu();
}

void MenuPage::clearPage() {
    for (int i = 0; i < 6; i++) {
        mainmenuSprites[i].createSprite(spriteW, spriteH);
        mainmenuSprites[i].fillSprite(defaultBgColor);
        mainmenuSprites[i].pushSprite(WIDTH / 2 - spriteW / 2, spriteH * (i + 1));
        mainmenuSprites[i].deleteSprite();
    }
    CancelButtonSprite.createSprite(30, 30);
    CancelButtonSprite.drawCircle(15, 15, 15, defaultBgColor);
    CancelButtonSprite.drawCircle(15, 15, 14, defaultBgColor);
    CancelButtonSprite.setTextColor(defaultBgColor, defaultBgColor);
    CancelButtonSprite.loadFont(FONT12);
    CancelButtonSprite.drawString("", 0, 0, GFXFF);
    CancelButtonSprite.unloadFont();
    CancelButtonSprite.pushSprite(20, HEIGHT / 2);
    CancelButtonSprite.deleteSprite();
}
