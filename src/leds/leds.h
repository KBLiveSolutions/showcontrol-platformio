#pragma once
#include <FastLED.h>

#include "../config/consts.h"


//////////////////////////////
// LEDs
//////////////////////////////

class Led {
private:
  uint8_t ledNumber;
  bool valueinarray(int val, int *arr) {
    for (uint8_t i = 0; i < sizeof(arr); i++) {
      if (arr[i] == val) return true;
      break;
    }
    return false;
  }

public:
  Led(uint8_t );

  uint8_t r = 0;
  uint8_t g = 0;  
  uint8_t b = 0;

  void begin();
  void set_default();
  void setRGBColor(uint8_t red, uint8_t green, uint8_t blue);
  void setIndexedColor(uint8_t color, uint8_t channel);
  void setInitColor();
  void show_color();
  void show_direct_color(int r, int g, int b);
  void show_color_fade(int fade_amount);
  void led_update(bool button_state);
  void blink_slow(int beat);
  void blink_fast(uint8_t beat);
  void show_white();
  void show_lightWhite();
  void show_lightGreen();
  void show_green();
  void show_red();
  void set_red();
  void led_off();
  void showPixel(uint8_t r, uint8_t g, uint8_t b);
};

extern Led l[NUM_LEDS];
namespace leds{
  void clearLeds();
  void setup();
}