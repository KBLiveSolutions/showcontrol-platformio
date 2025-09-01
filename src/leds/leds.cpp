

#include "leds.h"
#include "led_utils.h"
#include "../core/settingsParser.h"
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
Led l[NUM_LEDS] = {Led(0), Led(1), Led(2), Led(3), Led(4), Led(5)};

Led::Led(uint8_t number) {
  ledNumber = number;
};

void Led::begin() {
  set_default();
  show_color();
}

void Led::set_default() {
  RGBColor initColor = LedConfig::getInitColor(ledNumber);
  r = initColor.r;
  g = initColor.g;
  b = initColor.b;
}

void Led::setIndexedColor(uint8_t color, uint8_t channel) {
  RGBColor paletteColor = LedConfig::getPaletteColor(color);
  r = paletteColor.r;
  g = paletteColor.g;
  b = paletteColor.b;
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

void Led::setRGBColor(uint8_t red, uint8_t green, uint8_t blue) {
    r = red;
    g = green;
    b = blue;
}

void Led::setInitColor() {
    r = 100;
    g = 100;
    b = 100;
}

void Led::showPixel(uint8_t r, uint8_t g, uint8_t b) {
  for(int j=0; j < 5; j++){
    LedRow ledRow = LedConfig::getLedRow(ledNumber);
    int8_t index = ledRow.getLed(j);
    if(index > -1) {
      // Appliquer d'abord la luminosité
      uint8_t adjustedR = r * settings.ledBrightness / 10;
      uint8_t adjustedG = g * settings.ledBrightness / 10;
      uint8_t adjustedB = b * settings.ledBrightness / 10;
      strip.setPixelColor(index, strip.Color(adjustedR, adjustedG, adjustedB));
    }
  }
  strip.show();
}

void Led::show_color() {
  showPixel(r, g, b);
}

void Led::show_direct_color(int r, int g, int b) {
  showPixel(r, g, b);
}

void Led::led_update(bool button_state) {
  // if (!button_state) show_color();
  // else show_white();
}

void Led::show_green() {
  showPixel(0, 255, 0);
}

void Led::show_lightGreen() {
  showPixel(0, 40, 5);
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


void Led::set_red() {
    r = 255;
    g = 0;
    b = 0;
  show_color();
}

void Led::led_off() {
    r = 0;
    g = 0;
    b = 0;
  showPixel(0, 0, 0);
}


void leds::setup() {
  Serial.println("Starting LED initialization...");
  Serial.flush();
  strip.begin();
  strip.show(); // Éteint toutes les LEDs
  Serial.println("Initializing LED objects...");
  Serial.flush();
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    l[i].begin();
  }
  Serial.println("LED objects initialized");
  Serial.flush();
  Serial.println("Setup LEDs done (NeoPixel)");
  Serial.flush();
}

void leds::clearLeds() {
  for (uint8_t i = 1; i < NUM_LEDS; ++i) {
    l[i].show_direct_color(0, 0, 0);
  }
}