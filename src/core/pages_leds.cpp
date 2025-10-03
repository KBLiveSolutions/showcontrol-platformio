#include "pages.h"
#include "utils.h"
#include "../config/consts.h"
#include "../display/pages/mainPage.h"
// #include "../display/pages/settingsPage.h"
// #include "../display/pages/menuPage.h"
// #include "../display/pages/splashPage.h"
// #include "../display/pages/globalPage.h"
#include "../display/colors.h"
#include "../leds/leds.h"
// #include "../osc/OSCManager.h"
// #include "../midi/midi_out.h"
// #include "mainPageParser.h"
// #include "globalParser.h"

// ====================================
// LEDS
// ====================================

void Page::checkLeds(uint8_t channel, uint8_t _control, uint8_t value) {
  for (int i = 0; i < 5; i++) {  // Utilisation explicite de 5
    if (_control == control_led_cc[i] && channel == control_ch[i]) {
      // Vérification de sécurité pour l'accès au tableau l[]
      if (i + 1 < NUM_LEDS) {
        l[i + 1].setIndexedColor(value, channel);
        l[i + 1].show_color();
        if (pageType == USER) {
          uint16_t ledColor = l[i + 1].getColor(); // Méthode à adapter selon ta classe LED
          mainPage.showButtonSprite(
              buttonPressed[i],
              i,
              getActionName(pageNumber, i),
              ledColor, // couleur de la LED
              getLuminance(i)
          );
        }
      }
      break;
    }
  }
}

void Page::setRGBColor(uint8_t idx, uint8_t red, uint8_t green, uint8_t blue) {
  // Vérification de sécurité pour l'accès au tableau l[]
  led[idx][0] = red;
  led[idx][1] = green;
  led[idx][2] = blue;
  l[idx].setRGBColor(red, green, blue);
  l[idx].show_color();
}

void Page::showLeds(){  
  for (int i = 0; i < NUM_LEDS; i++) {
   // Vérification de sécurité pour l'accès au tableau l[]
   l[i].setRGBColor(led[i][0], led[i][1], led[i][2]);
   l[i].show_color();
  }
}