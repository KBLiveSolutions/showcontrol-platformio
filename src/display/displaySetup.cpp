#include "displaySetup.h"
#include "colors.h"
#include "../config/consts.h"
#include "../core/utils.h"

TFT_eSPI display; // = TFT_eSPI();

void displayManager::setup() {

  DEBUG_LOGLN("Initialisation de l'écran TFT...");
  
  // Configuration des pins avant init
  pinMode(TFT_LED, OUTPUT);
  analogWrite(TFT_LED, 0); // Éteindre le rétroéclairage pendant init
  
  DEBUG_LOGLN("TFT pins configured, starting display.init()...");
  
  // Petit délai avant init pour stabiliser
  delay(100);
  
  // Initialisation de l'écran avec debug
  display.init();
  DEBUG_LOGLN("display.init() completed");
  
  // Délai pour s'assurer que l'écran est stable
  delay(50);
  
  // Configuration de la rotation
  DEBUG_LOGLN("Setting display rotation...");
  display.setRotation(3);
  
  // Test de base - remplir l'écran avec une couleur
  DEBUG_LOGLN("Filling screen with default color...");
  display.fillScreen(defaultBgColor);
  
  // Allumer le rétroéclairage en dernier
  DEBUG_LOGLN("Enabling backlight...");
  analogWrite(TFT_LED, 255);
  
  DEBUG_LOGLN("Display setup done");
}