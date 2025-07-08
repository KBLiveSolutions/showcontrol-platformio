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
    // settings.isLocked = !settings.isLocked;
    // jsonManager.writeOption("settings.isLocked", settings.isLocked);
    // globalPage.showLock(settings.isLocked);
    // if (settings.isLocked)
    //     l[0].show_red();
    // else
    //     l[0].led_off();
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

void onEncoderButtonPress()
{
    if(shift) debugOn = !debugOn;
    DEBUG_LOG_VALUE("Encoder button pressed, shift: ", shift);  
    switch (activePage.pageType)
    {
    case SETTINGS:
        settings.validateSettings();
        break;
    case MENU:
        activePage.clearPage();
        DEBUG_LOGLN("Encoder button pressed in MENU_PAGE");
        DEBUG_LOG_VALUE("Active menu: ", menuPage.activeMenuItem);
        DEBUG_LOG_VALUE("Active menu siez -1: ", menuPage.activeMenuSize - 1);
        if (menuPage.activeMenu == MAIN_MENU)
        {
            if (menuPage.activeMenuItem == menuPage.activeMenuSize - 1)
            { // Settings
                DEBUG_LOGLN("Settings menu item selected");
                // previousActivePage = activePage;
                activePage = SETTINGS_PAGE;
                settingsPage.showPage();
            }
            else
            { // Main menu item selected
                DEBUG_LOGLN("Main menu item selected");
                DEBUG_LOG_VALUE("Selected mode: ", menuPage.activeMenuItem);
                _main.selectedMode = menuPage.activeMenuItem;
                activePage = pages[_main.selectedMode];
                activePage.showPage();
                jsonManager.writeOption("selectedMode", _main.selectedMode);
            }
        }
        else if (menuPage.activeMenu == SONG_MENU)
        {
            // menuPage.createMenu(SONG_MENU, _main.selectedMode);
        }
        else if (menuPage.activeMenu == SETLIST_MENU)
        {
            int selectedSetlist = menuPage.activeMenuItem;
            midi::sendActiveSetlistChange(selectedSetlist);
        }
    }

    menuPage.activeMenu = MENU_OFF;
}

void onEncoderButtonLongPress()
{
    activePage.clearPage();
    previousActivePage = activePage;
    if (activePage == SPLASH_PAGE)
    {
        activePage = SETTINGS_PAGE;
        settingsPage.showPage();
        return;
    }
    activePage = MENU_PAGE;
    menuPage.activeMenu = MAIN_MENU;
    menuPage.activeMenuItem = _main.selectedMode;
    menuPage.showPage();
};

void onButtonShortPress(uint8_t idx)
{
    DEBUG_LOG_VALUE("Button short pressed: ", idx);
    activePage.onButtonShortPress(idx);
};

void onButtonLongPress(uint8_t idx)
{
    activePage.buttonLongPress(idx);
};

void press_button(uint8_t idx)
{
    DEBUG_LOG_VALUE("Button pressed: ", idx);
    activePage.press_button(idx);
    l[idx].show_white();
    if (idx == 0) onLockButtonPress(); // Lock button
};

void release_button(uint8_t idx)
{
    activePage.release_button(idx);
    l[idx].show_color();
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