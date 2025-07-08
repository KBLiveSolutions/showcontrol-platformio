#include "displaySetup.h"
#include "colors.h"
#include "../config/consts.h"
#include "../core/utils.h"

TFT_eSPI display; // = TFT_eSPI();

void displayManager::setup() {

  DEBUG_LOGLN("Initialisation de l'écran TFT...");
  
  pinMode(TFT_LED, OUTPUT);
  // Initialisation de l'écran
  display.init();
  
  // Petit délai pour s'assurer que l'écran est prêt
  
  // Configuration de la rotation
  display.setRotation(3);
  
  // Test de base - remplir l'écran avec une couleur
  display.fillScreen(defaultBgColor);
  
  analogWrite(TFT_LED, 255); // Allumer le rétroéclairage d'abord
  
  DEBUG_LOGLN("Display setup done");
}