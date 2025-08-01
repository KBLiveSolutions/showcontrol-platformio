#pragma once
#include <Arduino.h>
#include "../core/utils.h"
#include "../config/consts.h"

// Types externes nécessaires (à adapter selon ton projet)
enum page_type { SETLIST, USER, SETTINGS, MENU, SPLASH, NONE };
// typedef enum { ActiveSong, ActiveSection, NextSong } displayed_item_t; // Idem

struct Page {
    uint8_t pageNumber;
    page_type pageType;
    bool buttonPressed[8];
    uint8_t buttonState[8];
    uint8_t control_custom[5];
    control_type_t control_type[5];
    uint8_t control_led_cc[5];
    uint8_t control_led_ch[5];
    uint8_t control_cc[5];
    uint8_t control_ch[5];
    uint8_t control_toggle[5];
    uint8_t pedalState[2];
    uint8_t pedal_cc[2];
    uint8_t pedal_ch[2];
    uint8_t pedal_toggle[2];
    uint8_t pedal_custom[2];
    uint8_t maxSpritesAmnt;
    displayed_item_t displayedItem[3];

    Page(uint8_t pageNumber, page_type pageType);

    // Méthodes d'affichage
    void showButtons(bool);
    void displayItemByType(displayed_item_t itemType, const char* content);
    void showSceneName();
    void showTrackName();
    void showLooperName();
    void showMarkerName();
    void showActiveSongName();
    void showActiveSectionName();
    void showNextSongName();
    void showNextSectionName();
    void showNextSceneName();
    void showMainSprite(const char* , uint16_t );
    void showSub1Sprite(const char* , uint16_t );
    void showSub2Sprite(const char* , uint16_t );
    void showMainUserSprite(const char* , uint16_t );
    void showSub1UserSprite(const char* , uint16_t );
    bool getLuminance(int);
    void showPage();
    void clearPage();
    void updatePlaySprite();
    void updateLoopSprite();
    void updateSongIndex();

    // Helper functions
    void handleSetlistButton(uint8_t buttonNum);

    // Input
    void onButtonShortPress(uint8_t buttonNum);
    void buttonLongPress(uint8_t buttonNum);
    void buttonDoublePress(uint8_t buttonNum);
    void press_button(uint8_t buttonNum);
    void release_button(uint8_t buttonNum);
    void doubleClick(uint8_t buttonNum);
    void expressionPedal(uint8_t pedalNum, uint8_t value);
    void press_pedal(uint8_t pedalNum);
    void release_pedal(uint8_t pedalNum);
    
    // Leds

    uint8_t led[NUM_LEDS][3] = {{100, 100, 100}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
    void setRGBColor(uint8_t, uint8_t, uint8_t, uint8_t); // Exemple de méthode pour définir la couleur d'une LED
    void checkLeds(uint8_t channel, uint8_t _control, uint8_t value);
    void showLeds();

    // Configuration
    void setDisplay(uint8_t display_num, uint8_t);
    void setLedControl(uint8_t controlNum, control_type_t type, uint8_t cc, uint8_t channel);
    void setButtonControl(uint8_t controlNum, control_type_t type, uint8_t cc, uint8_t channel, uint8_t custom, uint8_t toggled);
    void setButtonControlShifted(uint8_t controlNum, control_type_t type, uint8_t cc, uint8_t channel);
    void setPedal(int controlNum, control_type_t type, uint8_t cc, uint8_t channel, uint8_t custom, uint8_t toggled);

    // Opérateur d'affectation
    Page& operator=(const Page& other);
};

// Variables globales
extern Page pages[6];
extern Page SETTINGS_PAGE;
extern Page MENU_PAGE;
extern Page SPLASH_PAGE;
extern Page activePage;
extern Page previousActivePage;

// Fonctions utilitaires
bool operator==(const Page& lhs, const Page& rhs);
bool operator!=(const Page& lhs, const Page& rhs);

void removeUserPage(int pageNum);
void setupPages();


// void changeActivePage(uint8_t pageNum);

// void activePage.showPage();
// void showSongsCounter(bool show);

// Helper functions
    void handleSetlistButton(uint8_t buttonNum);
    
void switchActivePage(Page& );