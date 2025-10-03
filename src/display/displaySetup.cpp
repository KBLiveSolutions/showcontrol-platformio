#include "displaySetup.h"
#include "colors.h"
#include "../config/consts.h"
#include "../core/utils.h"

TFT_eSPI display; // = TFT_eSPI();

void displayManager::setup() {

  DEBUG_LOGLN("Initialisation de l'écran TFT...");
  
  pinMode(TFT_LED, OUTPUT);
  analogWrite(TFT_LED, 0);
  display.init();
  delay(50);
  display.setRotation(3);
  display.fillScreen(defaultBgColor);
  delay(100);
  analogWrite(TFT_LED, 255);
  DEBUG_LOGLN("Display setup done");
}