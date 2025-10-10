#include "../config/consts.h"
#include <cstring>  // Pour memcpy

#include "pages.h"
#include "utils.h"
#include "mainPageParser.h"
#include "globalParser.h"

#include "../leds/leds.h"
#include "../leds/led_utils.h"
#include "../display/pages/mainPage.h"
#include "../display/pages/settingsPage.h"
#include "../display/pages/menuPage.h"
#include "../display/pages/splashPage.h"
#include "../display/pages/globalPage.h"
#include "../display/colors.h"


// ====================================
// Initialisation des pages
// ====================================

Page pages[6] = { Page(0, SETLIST), Page(1, USER), 
    Page(2, USER), Page(3, USER), Page(4, USER), Page(5, USER)};

Page SETTINGS_PAGE(6, SETTINGS);
Page MENU_PAGE(7, MENU);
Page SPLASH_PAGE(8, SPLASH);
Page previousActivePage(1, SPLASH);

Page* activePage = &pages[0]; // ou SPLASH_PAGE

Page::Page(uint8_t pageNumber, page_type pageType)
: pageNumber(pageNumber), pageType(pageType) {
  DEBUG_LOG_VALUE("Page constructed: ", pageNumber);
  memset(buttonPressed, false, sizeof(buttonPressed));
  memset(buttonState, 0, sizeof(buttonState));
  
  // Configuration par défaut des éléments affichés
  displayedItem[0] = ActiveSong;
  displayedItem[1] = ActiveSection;
  displayedItem[2] = NextSong;
}

void setupPages() {
  for (size_t i = 0; i < 6; i++){  
    for (int j = 0; j < NUM_CONTROLS; ++j) {
    pages[i].controls[j].type    = jsonManager.getControlType(i, j);
    pages[i].controls[j].cc      = jsonManager.getControlCC(i, j);
    pages[i].controls[j].ch      = jsonManager.getControlChannel(i, j);
    pages[i].controls[j].toggle  = jsonManager.getControlToggle(i, j);
    pages[i].controls[j].led_cc  = jsonManager.getControlLedCC(i, j);
    pages[i].controls[j].led_ch  = jsonManager.getControlLedChannel(i, j);
    pages[i].controls[j].custom  = jsonManager.getControlCustom(i, j);
    jsonManager.getActionName(i, j, pages[i].controls[j].actionName, sizeof(pages[i].controls[j].actionName));
    pages[i].controls[j].led_color  = jsonManager.getLedColorIndex(i, j);
    pages[i].controls[j].color      = getColorFromIndex(pages[i].controls[j].led_color);
    pages[i].controls[j].luminance  = jsonManager.getLuminance(i, j);
    }
    for(int j = 0; j < 2; j++){
      pages[i].displayedItem[j] = jsonManager.getDisplayType(i, j);
    }
  }
  
  switchActivePage(SPLASH_PAGE);
  DEBUG_LOGLN("SETUP PAGES DONE");
}

void switchActivePage(Page& newPage) {
    DEBUG_LOG_VALUE("switchActivePage called with pageNumber: ", newPage.pageNumber);

    static bool isSwitching = false;
    if (isSwitching) {
        DEBUG_LOGLN("Page switch already in progress, ignoring");
        return;
    }
    isSwitching = true;

    if (activePage != &SETTINGS_PAGE && activePage != &MENU_PAGE) previousActivePage = *activePage;

    activePage->clearPage();
    DEBUG_LOG_VALUE("switchActivePage: previousActivePage pageNumber: ", previousActivePage.pageNumber);

    if (newPage.pageType == NONE) {
        DEBUG_LOGLN("switchActivePage: newPage is NONE, aborting");
        isSwitching = false;
        return;
    }
    activePage = &newPage;
    DEBUG_LOG_VALUE("switchActivePage: about to assign activePage = &newPage, pageNumber: ", newPage.pageNumber);

    if (newPage.pageType != SPLASH) {
      globalPage.showTitle();
      // globalPage.showPageIcon();
    }
    activePage->showPage();
    isSwitching = false;
}


void removeUserPage(int pageNum) {
  // À implémenter
}
