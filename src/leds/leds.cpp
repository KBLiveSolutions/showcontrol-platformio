
#include "leds.h"
#include "led_utils.h"
#include "../core/settingsParser.h"

CRGB pixels[NUM_LEDS];
Led l[NUM_LEDS] = {Led(0), Led(1), Led(2), Led(3), Led(4), Led(5)};

Led::Led(uint8_t number) {
  ledNumber = number;
};

void Led::begin() {
  set_default();
  show_color();
}

void Led::set_default() {
  r = init_led_color_red[ledNumber];
  g = init_led_color_green[ledNumber];
  b = init_led_color_blue[ledNumber];
}

void Led::setColor(uint8_t color, uint8_t channel) {
  r = colorIndex[color][0];
  g = colorIndex[color][1];
  b = colorIndex[color][2];
  // led_channel[current_layout] = channel;
  if (color == 0) {
    // r = 0;
    // g = 0;
    // b = 0;
    int controls_list[9] = { 13, 14, 15, 18, 19, 22, 28, 35, 41 };   // list of controls that rely on colorIndex
    // if (valueinarray(led_control[current_layout], controls_list)) {  // if value = 0 and the control corresponds to a colorIndex then get colorIndex[0]
    //   r = colorIndex[color][0];
    //   g = colorIndex[color][1];
    //   b = colorIndex[color][2];
    // }
  }
}

void Led::setInitColor() {
    r = 100;
    g = 100;
    b = 100;
}

void Led::showPixel(uint8_t r, uint8_t g, uint8_t b) {
  for(int j=0; j < 5; j++){
    int index = leds_array[ledNumber][j];
    if(index > -1) pixels[index] = CRGB(int(r * settings.ledBrightness / 10), int(g * settings.ledBrightness / 10), int(b * settings.ledBrightness / 10));
    // if(index > -1) pixels.setPixelColor(index, int(r * settings.ledBrightness / 100), int(g * settings.ledBrightness / 100), int(b * settings.ledBrightness / 100), true);
     FastLED.show();
  }
}

void Led::show_color() {
  showPixel(r * 2, g * 2, b * 2);
}

void Led::show_direct_color(int r, int g, int b) {
  showPixel(r * 2, g * 2, b * 2);
}

void Led::led_update(bool button_state) {
  // if (!button_state) show_color();
  // else show_white();
}

void Led::show_green() {
  showPixel(0, 255, 0);
}

void Led::show_lightGreen() {
  showPixel(0, 120, 10);
}

void Led::show_white() {
  showPixel(255, 255, 255);
}

void Led::show_lightWhite() {
  showPixel(r, g, b);
}

void Led::show_red() {
  showPixel(255, 0, 0);
}

void Led::led_off() {
    r = 0;
    g = 0;
    b = 0;
  showPixel(0, 0, 0);
}


void leds::setup() {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    l[i].begin();
  }
  FastLED.addLeds<NEOPIXEL, LED_PIN>(pixels, NUMPIXELS); 
  Serial.println("Setup LEDs done");
}

void leds::clearLeds() {
  for (uint8_t i = 1; i < NUM_LEDS; ++i) {
    l[i].r = 0;
    l[i].g = 0;
    l[i].b = 0;
    l[i].show_color();
  }
}

