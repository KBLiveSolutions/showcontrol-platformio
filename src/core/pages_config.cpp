#include "pages.h"
#include "utils.h"
#include "../config/consts.h"
// #include "../display/pages/mainPage.h"
// #include "../display/pages/settingsPage.h"
// #include "../display/pages/menuPage.h"
// #include "../display/pages/splashPage.h"
// #include "../display/pages/globalPage.h"
// #include "../display/colors.h"
// #include "../leds/leds.h"
// #include "../osc/OSCManager.h"
// #include "../midi/midi_out.h"
#include "mainPageParser.h"
// #include "globalParser.h"
    
// ====================================
// CONFIGURE
// ====================================

  void Page::setButtonControl(uint8_t controlNum, control_type_t type, uint8_t cc, uint8_t channel, uint8_t custom, uint8_t toggled) {
    if (controlNum >= 5) return;  // Protection contre l'accès hors limites
    
    int user_mode = mainParser.selectedMode;
    control_type[controlNum] = type;
    control_cc[controlNum] = cc;
    control_ch[controlNum] = channel;
    control_custom[controlNum] = custom;
    control_toggle[controlNum] = toggled;
    // if(pageNumber == mainParser.selectedMode) mainPage.showButtonSprite(false, controlNum, getActionName(user_mode, controlNum), getActionColor(user_mode, controlNum), getLuminance(controlNum));
  };

  void Page::setButtonControlShifted(uint8_t controlNum, control_type_t type, uint8_t cc, uint8_t channel) {
    if (controlNum >= 5) return;  // Protection contre l'accès hors limites
    
    control_type[controlNum] = type;
    control_cc[controlNum] = cc;
    control_ch[controlNum] = channel;
  };

  void Page::setLedControl(uint8_t controlNum, control_type_t type, uint8_t cc, uint8_t channel) {
    if (controlNum >= 5) return;  // Protection contre l'accès hors limites
    
    control_led_cc[controlNum] = cc;
    control_led_ch[controlNum] = channel;
  };

  void Page::setPedal(int controlNum, control_type_t type, uint8_t cc, uint8_t channel, uint8_t custom, uint8_t toggled) {
    // pedal_type[controlNum] = type;
    // pedal_cc[controlNum] = cc;
    // pedal_ch[controlNum] = channel;
    // pedal_custom[controlNum] = custom;
    // pedal_toggle[controlNum] = toggled;
  };

  void Page::setDisplay(uint8_t display_num, uint8_t display_type) {
    if (display_num >= 3) return;  // Protection contre l'accès hors limites au tableau displayedItem[3]
    displayedItem[display_num] = static_cast<displayed_item_t>(display_type);
  }
  