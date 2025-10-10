#include "pages.h"
#include "utils.h"
#include "../config/consts.h"
#include "pages.h"
#include "../display/pages/mainPage.h"
// #include "../display/pages/settingsPage.h"
// #include "../display/pages/menuPage.h"
// #include "../display/pages/splashPage.h"
#include "../display/pages/globalPage.h"
#include "../display/colors.h"
#include "../leds/leds.h"
// #include "../osc/OSCManager.h"
// #include "../midi/midi_out.h"
// #include "mainPageParser.h"
#include "globalParser.h"

// ====================================
// LEDS
// ====================================

void Page::updateLed(uint8_t index, uint8_t value) {
  int led_index = index % 5 + 1;
  if(value>64){
    l[led_index].setIndexedColor(controls[index].led_color);
    l[led_index].show_color();
  }
  else {
    l[led_index].led_off();
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