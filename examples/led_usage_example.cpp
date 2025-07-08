/**
 * @file led_usage_example.cpp
 * @brief Exemple d'utilisation du module LED optimisé
 * @details Démontre l'utilisation des nouvelles fonctionnalités LED
 */

#include "leds/led_utils.h"

// Exemple d'utilisation des fonctionnalités LED optimisées
void setupOptimizedLEDs() {
    // Utilisation des couleurs prédéfinies
    RGBColor red = LedUtils::RED;
    RGBColor customColor = RGBColor(127, 64, 255);
    
    // Accès aux couleurs initiales (depuis PROGMEM)
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        RGBColor initColor = LedConfig::getInitColor(i);
        Serial.printf("LED %d: R=%d G=%d B=%d\n", 
                     i, initColor.r, initColor.g, initColor.b);
    }
    
    // Utilisation de la palette de couleurs
    RGBColor paletteColor = LedConfig::getPaletteColor(42);
    Serial.printf("Couleur palette 42: R=%d G=%d B=%d\n", 
                 paletteColor.r, paletteColor.g, paletteColor.b);
    
    // Configuration des rangées de LEDs
    for (uint8_t row = 0; row < LedConfig::NUM_ROWS; row++) {
        LedRow ledRow = LedConfig::getLedRow(row);
        Serial.printf("Rangée %d: %d LEDs - ", row, ledRow.count);
        
        for (uint8_t i = 0; i < ledRow.count; i++) {
            int8_t ledIndex = ledRow.getLed(i);
            Serial.printf("LED%d ", ledIndex);
        }
        Serial.println();
    }
}

void demonstrateLEDEffects() {
    // Correction gamma
    RGBColor rawColor = RGBColor(128, 64, 192);
    RGBColor gammaCorrected = LedUtils::gammaCorrectColor(rawColor);
    
    // Interpolation de couleurs
    RGBColor color1 = LedUtils::RED;
    RGBColor color2 = LedUtils::BLUE;
    RGBColor interpolated = LedUtils::interpolateColor(color1, color2, 128);
    
    // Mélange de couleurs
    RGBColor baseColor = LedUtils::GREEN;
    RGBColor overlayColor = LedUtils::WHITE;
    RGBColor blended = LedUtils::blendColors(baseColor, overlayColor, 64);
    
    // Conversion HSV vers RGB
    RGBColor hsvColor = LedUtils::hsvToRgb(180, 255, 200); // Cyan
    
    // Fade progressif
    RGBColor originalColor = LedUtils::YELLOW;
    RGBColor fadedColor = LedUtils::fadeColor(originalColor, 50);
    
    // Utilisation des opérateurs de couleur
    bool colorsEqual = (color1 == color2);
    uint32_t colorValue = rawColor.toUint32();
    RGBColor scaledColor = rawColor.scale(128); // 50% luminosité
    
    // Validation des indices
    if (LedUtils::isValidLedIndex(5)) {
        Serial.println("Index LED valide");
    }
    
    if (LedUtils::isValidColorIndex(127)) {
        Serial.println("Index couleur palette valide");
    }
}

void advancedLEDPatterns() {
    // Pattern arc-en-ciel
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        uint8_t hue = (i * 255) / NUM_LEDS;
        RGBColor rainbowColor = LedUtils::hsvToRgb(hue, 255, 200);
        
        // Appliquer la correction gamma pour un rendu naturel
        RGBColor finalColor = LedUtils::gammaCorrectColor(rainbowColor);
        
        // Ici, vous appliqueriez la couleur à la LED physique
        // Par exemple avec FastLED ou NeoPixel:
        // leds[i] = CRGB(finalColor.r, finalColor.g, finalColor.b);
    }
    
    // Pattern de respiration (breathing)
    static uint8_t breathBrightness = 0;
    static int8_t breathDirection = 1;
    
    breathBrightness += breathDirection * 2;
    if (breathBrightness >= 255 || breathBrightness <= 0) {
        breathDirection = -breathDirection;
    }
    
    RGBColor baseColor = LedConfig::getPaletteColor(25); // Magenta
    RGBColor breathColor = baseColor.scale(breathBrightness);
    
    // Appliquer à toutes les LEDs
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        // Appliquer breathColor à la LED i
    }
}

void configureLEDRows() {
    // Configuration par rangées pour des effets organisés
    for (uint8_t row = 0; row < LedConfig::NUM_ROWS; row++) {
        LedRow ledRow = LedConfig::getLedRow(row);
        
        // Couleur différente par rangée
        RGBColor rowColor = LedConfig::getPaletteColor(row * 20);
        
        // Appliquer la couleur à tous les LEDs de la rangée
        for (uint8_t i = 0; i < ledRow.count; i++) {
            int8_t ledIndex = ledRow.getLed(i);
            if (ledIndex >= 0) {
                // Appliquer rowColor à la LED ledIndex
                Serial.printf("Rangée %d, LED %d: couleur définie\n", row, ledIndex);
            }
        }
    }
}

// Exemple d'intégration dans la boucle principale
void updateLEDs() {
    static unsigned long lastUpdate = 0;
    static uint8_t effectMode = 0;
    
    if (millis() - lastUpdate > 50) { // Mise à jour 20 FPS
        lastUpdate = millis();
        
        switch (effectMode) {
            case 0:
                advancedLEDPatterns();
                break;
            case 1:
                configureLEDRows();
                break;
            default:
                demonstrateLEDEffects();
                break;
        }
        
        // Changer d'effet toutes les 5 secondes
        if (millis() % 5000 < 50) {
            effectMode = (effectMode + 1) % 3;
        }
    }
}
