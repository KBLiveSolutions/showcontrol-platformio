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




void onLockButtonPress(){
    settings.isLocked = !settings.isLocked;
    jsonManager.writeOption("settings.isLocked", settings.isLocked);
    globalPage.showLock(settings.isLocked);
    if (settings.isLocked) l[0].show_red();
    else l[0].led_off();
};

void onLockButtonLongPress(){};

void onEncoderButtonPress() {
    switch (activePage.pageType) {
        case SETTINGS:
                settings.validateSettings();
            break;
        case MENU:
            activePage.clearPage();
            DEBUG_LOGLN("Encoder button pressed in MENU_PAGE");  
            DEBUG_LOG_VALUE("Active menu: ", menuPage.activeMenuItem);
            DEBUG_LOG_VALUE("Active menu siez -1: ", menuPage.activeMenuSize-1);
            if (menuPage.activeMenu == MAIN_MENU) {            
                if(menuPage.activeMenuItem == menuPage.activeMenuSize-1) { // Settings
                    DEBUG_LOGLN("Settings menu item selected");
                    // previousActivePage = activePage;
                    activePage = SETTINGS_PAGE;
                    settingsPage.showPage();
                }   
                else { // Main menu item selected
                    DEBUG_LOGLN("Main menu item selected");
                    DEBUG_LOG_VALUE("Selected mode: ", menuPage.activeMenuItem);
                    _main.selectedMode = menuPage.activeMenuItem;
                    activePage = pages[_main.selectedMode];
                    activePage.showPage();
                    jsonManager.writeOption("selectedMode", _main.selectedMode);
                }
            }
            else if (menuPage.activeMenu == SONG_MENU) {
                // menuPage.createMenu(SONG_MENU, _main.selectedMode);
            }
            else if (menuPage.activeMenu == SETLIST_MENU) {
                int selectedSetlist = menuPage.activeMenuItem;
                midi::sendActiveSetlistChange(selectedSetlist);
            }
    }

    menuPage.activeMenu = MENU_OFF;
}

void onEncoderButtonLongPress(){
    activePage.clearPage();
    previousActivePage = activePage;
    if(activePage == SPLASH_PAGE) {
        activePage = SETTINGS_PAGE;
        settingsPage.showPage();
        return;
    }
    activePage = MENU_PAGE;
    menuPage.activeMenu = MAIN_MENU;
    menuPage.activeMenuItem = _main.selectedMode;
    menuPage.showPage();
};

void onButtonShortPress(uint8_t idx){
    DEBUG_LOG_VALUE("Button short pressed: ", idx);
    activePage.onButtonShortPress(idx);
};

void onButtonLongPress(uint8_t idx){
    activePage.buttonLongPress(idx);
};

void press_button(uint8_t idx){
    DEBUG_LOG_VALUE("Button pressed: ", idx);
    activePage.press_button(idx);
      l[idx].show_white();
}; 

void release_button(uint8_t idx){
    activePage.release_button(idx);
      l[idx].show_color();
};

void onEncoderTurned(int value){
        DEBUG_LOG_VALUE("Encoder value: ", value);
        if(activePage.pageType == SETTINGS) {
            settingsPage.changeSelectedItem(value);
        }
        else if(activePage.pageType == MENU) {
            menuPage.changeSelectedItem(value);
        }
};


// void NewButton::button_check(bool state) {

//   if (state && !btn_state && (millis() - debounce_time > debounce_delay)) {
//     Serial.print("Button ");
//     Serial.println(number);
//     btn_state = HIGH;
//     debounce_time = millis();
//     if(number != 6) activePage.press_button(number-1);
//   //   else {
//   //     if (activePage == MAINMENU) activePage.showPage();}
//   }
//   if (!state && btn_state && (millis() - debounce_time > debounce_delay)) {
//     btn_state = LOW;
//     activePage.release_button(number-1);
//       debounce_time = millis();
//   }
// }

// void NewButton::button0_check(bool state) {
//   if (state && !btn_state && (millis() - debounce_time > debounce_delay)) {
//     // Serial.print("button ");
//     // Serial.print(number);
//     // Serial.println(state);
//     btn_state = HIGH;
//     if (menuPage.activeMenu == MENU_OFF) shift = true;
//     else {
//       menuPage.clearMainmenuSprites();
//       activePage.showPage();
//     }
//     Serial.println("SHIFT ON");
//     // change_led(HIGH);
//     debounce_time = millis();
//   }
//   if (!state && btn_state && (millis() - debounce_time > debounce_delay)) {
//     btn_state = LOW;
//     if (menuPage.activeMenu == MENU_OFF) shift = false;
//     Serial.println("SHIFT OFF");
//     // if((millis() - debounce_time) < 1000){
//     //   settings.isLocked = !settings.isLocked;
//     //   globalPage.showLock(settings.isLocked);
//     //   if(settings.isLocked) l[0].show_red();
//     //   else l[0].led_off();
//     // }
//     debounce_time = millis();
//   }
// }

// void NewButton::simpleClick() {
//     Serial.print("Button simple ");
//     Serial.println(number);  
//     if(number == 6) {
//       if (menuPage.activeMenu != MENU_OFF) validateMenu();
//       else if (activePage.pageType == SETTINGS) settings.validateSettings();
//       else _main.sendSongArrayRequest();
//     }
// }

// void NewButton::longClick() {
//     Serial.print("Button long ");
//     Serial.println(number);    
//     if(number == 6) {
//       if (menuPage.activeMenu == MENU_OFF) menuPage.createMenu(MAIN_MENU, _main.selectedMode);
//       else validateMenu();
//     }
//     else activePage.buttonLongPress(number-1);
// }

// void NewButton::doubleClick() {
//     Serial.print("Button double ");
//     Serial.println(number);
//     if(number == 6) {
//       if (menuPage.activeMenu == MENU_OFF) _main.sendSetlistArrayRequest();
//       else validateMenu();
//     }
//     else activePage.buttonDoublePress(number-1);
// }

// void NewButton::change_led(bool state) {
//   // led_state = state;
//   // if (number < 6) l[number].led_update(state);
// }


// void validateMenu(){
//     if (menuPage.activeMenu == MAIN_MENU) {
//         _main.selectedMode = menuPage.activeMenuItem;
//         jsonManager.writeOption("selectedMode", _main.selectedMode);
//     }
//     else if (menuPage.activeMenu == SONG_MENU) {
//         if (activePage.pageType == SETLIST) sendOSCAbleset("/setlist/jumpToSong", menuPage.activeMenuItem + 1);
//     }
//     else if (menuPage.activeMenu == SETLIST_MENU) {
//         int selectedSetlist = menuPage.activeMenuItem;
//         midi::sendActiveSetlistChange(selectedSetlist);
//     }
//     menuPage.activeMenu = MENU_OFF;
//     menuPage.clearMainmenuSprites();
//     activePage.showPage();
// }