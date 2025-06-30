#pragma once

#include <Arduino.h>
#include "../config/consts.h"

// Déclarations des tableaux (extern)
extern int leds_array[6][5];
extern byte init_led_color_red[NUM_LEDS];
extern byte init_led_color_green[NUM_LEDS];
extern byte init_led_color_blue[NUM_LEDS];
extern const byte colorIndex[128][3];

