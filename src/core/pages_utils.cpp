#include "../config/consts.h"
#include <cstring>  // Pour memcpy

#include "pages.h"

// Opérateurs globaux
bool operator==(const Page& lhs, const Page& rhs) {
  return lhs.pageNumber == rhs.pageNumber;
}
bool operator!=(const Page& lhs, const Page& rhs) {
  return !(lhs == rhs);
}

Page& Page::operator=(const Page& other) {
    if (this != &other) {
        pageNumber = other.pageNumber;
        pageType = other.pageType;
        
        // Utilisation de memcpy pour les copies de tableaux (plus efficace)
        memcpy(buttonPressed, other.buttonPressed, sizeof(buttonPressed));
        memcpy(buttonState, other.buttonState, sizeof(buttonState));
        memcpy(control_custom, other.control_custom, sizeof(control_custom));
        memcpy(control_type, other.control_type, sizeof(control_type));
        memcpy(control_led_cc, other.control_led_cc, sizeof(control_led_cc));
        memcpy(control_led_ch, other.control_led_ch, sizeof(control_led_ch));
        memcpy(control_cc, other.control_cc, sizeof(control_cc));
        memcpy(control_ch, other.control_ch, sizeof(control_ch));
        memcpy(control_toggle, other.control_toggle, sizeof(control_toggle));
        memcpy(pedalState, other.pedalState, sizeof(pedalState));
        memcpy(pedal_cc, other.pedal_cc, sizeof(pedal_cc));
        memcpy(pedal_ch, other.pedal_ch, sizeof(pedal_ch));
        memcpy(pedal_toggle, other.pedal_toggle, sizeof(pedal_toggle));
        memcpy(pedal_custom, other.pedal_custom, sizeof(pedal_custom));
        memcpy(displayedItem, other.displayedItem, sizeof(displayedItem));
        memcpy(led, other.led, sizeof(led));
        
        maxSpritesAmnt = other.maxSpritesAmnt;
    }
    return *this;
}
