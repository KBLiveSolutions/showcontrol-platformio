#include "../config/consts.h"
#include <cstring>  // Pour memcpy

#include "pages.h"
#include "utils.h"
#include "mainPageParser.h"
#include "globalParser.h"

#include "../leds/leds.h"
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
  // Initialisation basique pour éviter les problèmes d'optimisation agressive
  // Note: l'avertissement du compilateur sur l'itération 8 semble être un faux positif
  // lié à l'optimisation agressive, mais le code est fonctionnellement correct
  
  control_type[0] = control_type[1] = control_type[2] = control_type[3] = control_type[4] = CC;
  control_cc[0] = control_cc[1] = control_cc[2] = control_cc[3] = control_cc[4] = 1;
  control_ch[0] = control_ch[1] = control_ch[2] = control_ch[3] = control_ch[4] = 1;
  control_toggle[0] = control_toggle[1] = control_toggle[2] = control_toggle[3] = control_toggle[4] = 0;
  control_led_cc[0] = control_led_cc[1] = control_led_cc[2] = control_led_cc[3] = control_led_cc[4] = 1;
  control_led_ch[0] = control_led_ch[1] = control_led_ch[2] = control_led_ch[3] = control_led_ch[4] = 1;
  control_custom[0] = control_custom[1] = control_custom[2] = control_custom[3] = control_custom[4] = 0;
  
  // Initialisation sécurisée des tableaux de boutons
  memset(buttonPressed, false, sizeof(buttonPressed));
  memset(buttonState, 0, sizeof(buttonState));
  
  // Configuration par défaut des éléments affichés
  displayedItem[0] = ActiveSong;
  displayedItem[1] = ActiveSection;
  displayedItem[2] = NextSong;
}

void setupPages() {
  // Configuration optimisée avec des tableaux de données
  static const uint8_t buttonCCs[] = {44, 12, 33, 34, 113};
  static const uint8_t ledCCs[] = {1, 34, 33, 34, 113};
  
  for (uint8_t i = 0; i < 5; ++i) {
    pages[0].setButtonControl(i, CC, buttonCCs[i], 15, 0, 0);
    pages[0].setLedControl(i, CC, ledCCs[i], 15);
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
