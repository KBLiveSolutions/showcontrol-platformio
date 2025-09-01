#pragma once
#include <Arduino.h>
#include "../../core/utils.h"

// Constantes et types
#include "../../config/consts.h"

#define MAX_MENU_ITEMS 5
#define MAX_MENU_SPRITES 5

// Variables globales
class MenuPage {
    public:
        menu_type_t activeMenu = MENU_OFF;
        uint8_t activeMenuSize = 0;
        char menuItems[MAX_MENU_ITEMS][MAX_SONG_NAME];
        uint8_t activeMenuItem = 0;

        // Fonctions
        void changeSelectedItem(int );
        void showPage();
        void clearPage();
        void showCancelSprite(bool show);
    private:
        int maxItems = 0;
        uint8_t startIndex = 0;
        char modesList[7][MAX_SONG_NAME] = {"Setlist", "User 1", "User 2", "User 3", "User 4", "User 5", "Settings"};
        void showMenuEntry(const char* , uint8_t , uint8_t );
        void updateMainMenu();
};

extern MenuPage menuPage;

