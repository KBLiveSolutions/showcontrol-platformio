#include "pages.h"
#include "utils.h"
#include "../config/consts.h"
#include "mainPageParser.h"
    
// ====================================
// CONFIGURE
// ====================================


  void Page::setPedal(int controlNum, control_type_t type, uint8_t cc, uint8_t channel, uint8_t custom, uint8_t toggled) {
    // pedal_type = type;
    // pedal_cc = cc;
    // pedal_ch = channel;
    // pedal_custom = custom;
    // pedal_toggle = toggled;
  };

  // void Page::setDisplay(uint8_t display_num, uint8_t display_type) {
  //   if (display_num >= 3) return;  // Protection contre l'accès hors limites au tableau displayedItem[3]
  //   displayedItem[display_num] = static_cast<displayed_item_t>(display_type);
  // }
  