#include "displaySetup.h"
#include "colors.h"
#include "../config/consts.h"
#include "../core/utils.h"

TFT_eSPI display; // = TFT_eSPI();

void displayManager::setup() {

  DEBUG_LOGLN("Initialisation de l'écran TFT...");
  
  display.init();
  display.setRotation(3);
  display.fillScreen(defaultBgColor);
  delay(100);
  // analogWrite(TFT_LED, 255);
  digitalWrite(TFT_LED, HIGH);
  DEBUG_LOGLN("Display setup done");
}