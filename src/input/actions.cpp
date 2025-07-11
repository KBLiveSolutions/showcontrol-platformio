#include "actions.h"

#include "../config/jsonManager.h"

#include "../config/consts.h"

#include "../core/pages.h"
#include "../core/utils.h"
#include "../core/settingsParser.h"
#include "../core/settingsParser.h"

#include "../display/pages/menuPage.h"
#include "../display/pages/globalPage.h"
#include "../display/pages/settingsPage.h"
#include "../display/pages/splashPage.h"

#include "../osc/OSCManager.h"
#include "../midi/midi_out.h"
#include "../leds/leds.h"

bool shift = false;

void onLockButtonPress()
{
    if (activePage.pageType == MENU || activePage.pageType == SETTINGS) {
        switchActivePage(previousActivePage); // Return to previous page if in MENU or SETTINGS
        DEBUG_LOGLN("Returning to previous page from MENU or SETTINGS");    
        return;
    }
    settings.isLocked = !settings.isLocked;
    // jsonManager.writeOption("settings.isLocked", settings.isLocked);
    // globalPage.showLock(settings.isLocked);
    if (settings.isLocked){
        l[0].setRGBColor(255, 0, 0); // Show red color for locked state
        l[0].show_color();
    }
    else l[0].led_off();
    shift = true;
};

void onLockButtonRelease()
{
    // settings.isLocked = !settings.isLocked;
    // jsonManager.writeOption("settings.isLocked", settings.isLocked);
    // globalPage.showLock(settings.isLocked);
    // if (settings.isLocked)
    //     l[0].show_red();
    // else
    //     l[0].led_off();
    shift = false;
};

void onEncoderButtonPress(){
    if(shift) debugOn = !debugOn;
    DEBUG_LOG_VALUE("Encoder button pressed, shift: ", shift);  
    switch (activePage.pageType){
        case SETTINGS:
            settings.validateSettings();
            break;
        case MENU:{
            DEBUG_LOGLN("Encoder button pressed in MENU_PAGE");
            DEBUG_LOG_VALUE("Active menu: ", menuPage.activeMenuItem);
            DEBUG_LOG_VALUE("Active menu size -1: ", menuPage.activeMenuSize - 1);
            switch (menuPage.activeMenu){
                case MAIN_MENU:{
                    if (menuPage.activeMenuItem == menuPage.activeMenuSize - 1){ // Settings
                        DEBUG_LOGLN("Settings menu item selected");
                        switchActivePage(SETTINGS_PAGE);
                    }
                    else{ // Main menu item selected
                        DEBUG_LOGLN("Main menu item selected");
                        DEBUG_LOG_VALUE("Selected mode: ", menuPage.activeMenuItem);
                        _main.selectedMode = menuPage.activeMenuItem;
                        switchActivePage(pages[_main.selectedMode]);
                        jsonManager.writeOption("selectedMode", _main.selectedMode);
                    }
                }
                break;
                case SONG_MENU:
                {
                    DEBUG_LOG_VALUE("Selected song: ", menuPage.activeMenuItem);
                    switchActivePage(pages[0]);
                    delay(20);
                    sendOSCAbleset("/setlist/jumpToSong", menuPage.activeMenuItem + 1);
                }
                break;
                // case SETLIST_MENU:
                // {
                //     int selectedSetlist = menuPage.activeMenuItem;
                //     midi::sendActiveSetlistChange(selectedSetlist);
                // }
                // break;
            }
        }
        menuPage.activeMenu = MENU_OFF;
        break;
        case SETLIST:{
            DEBUG_LOGLN("Encoder button pressed in SETLIST_PAGE");
            menuPage.activeMenu = SONG_MENU;
            getAblesetValues();
            menuPage.activeMenuItem = _main.activeSongIndex;
            switchActivePage(MENU_PAGE);
        }
        break;
    }
}

void onEncoderButtonLongPress()
{
    if (activePage == SPLASH_PAGE){
        switchActivePage(SETTINGS_PAGE);
        return;
    }
    menuPage.activeMenu = MAIN_MENU;
    menuPage.activeMenuItem = _main.selectedMode;
    switchActivePage(MENU_PAGE);
};

void onButtonShortPress(uint8_t idx)
{
    DEBUG_LOG_VALUE("Button short pressed: ", idx);
    if (!settings.isLocked) activePage.onButtonShortPress(idx);
};

void onButtonLongPress(uint8_t idx)
{
    if (!settings.isLocked) activePage.buttonLongPress(idx);
};

void press_button(uint8_t idx)
{
    DEBUG_LOG_VALUE("Button pressed: ", idx);
    activePage.press_button(idx);
    if(idx < NUM_LEDS){
        if (!settings.isLocked) l[idx].show_white();
        else l[idx].show_red();
    }
    if (idx == 0) onLockButtonPress(); // Lock button
};

void release_button(uint8_t idx)
{
    activePage.release_button(idx);
    if(idx < NUM_LEDS) l[idx].show_color();
    if (idx == 0) onLockButtonRelease(); // Lock button
};

void onEncoderTurned(int value)
{
    DEBUG_LOG_VALUE("Encoder value: ", value);
    if (activePage.pageType == SETTINGS)
    {
        settingsPage.changeSelectedItem(value);
    }
    else if (activePage.pageType == MENU)
    {
        menuPage.changeSelectedItem(value);
    }
};