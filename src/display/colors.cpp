
#include <Arduino.h>
#include "colors.h"
#include <stdlib.h>

 uint16_t topBarBgColor = _Black;
 uint16_t defaultTxtColor = _White;
 uint16_t defaultBgColor = _Black;

uint16_t RRRGB888toRGB565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

uint16_t hexStringtoRGB565(char* hex) {
  long rgb = strtol(&hex[1], NULL, 16);
  uint8_t r = (rgb >> 16) & 0xFF;
  uint8_t g = (rgb >> 8) & 0xFF;
  uint8_t b = rgb & 0xFF;
  return RRRGB888toRGB565(r, g, b);
}

uint16_t hexStringtoRGB565Shade(char* hex) {
  long rgb = strtol(&hex[1], NULL, 16);
  uint8_t r = (rgb >> 16);
  uint8_t g = (rgb >> 8) & 0xFF;
  uint8_t b = rgb & 0xFF;
  return RRRGB888toRGB565(r * 2 / 3, g * 2 / 3, b * 2 / 3);
}