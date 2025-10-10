#pragma once

#include <Arduino.h>
#include "../config/consts.h"

/**
 * @file led_utils.h
 * @brief Optimised LED utility module for show control system
 * @details This module provides efficient color management and LED configuration
 * with memory-optimised structures and PROGMEM storage
 */

// Structure pour représenter une couleur RGB de manière plus efficace
struct RGBColor {
    uint8_t r, g, b;
    
    constexpr RGBColor(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0) 
        : r(red), g(green), b(blue) {}
    
    // Opérateurs pour faciliter les manipulations de couleurs
    constexpr bool operator==(const RGBColor& other) const {
        return r == other.r && g == other.g && b == other.b;
    }
    
    constexpr bool operator!=(const RGBColor& other) const {
        return !(*this == other);
    }
    
    // Conversion vers uint32_t pour les bibliothèques de LEDs
    constexpr uint32_t toUint32() const {
        return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    }
    
    // Facteur d'échelle pour ajuster la luminosité
    constexpr RGBColor scale(uint8_t factor) const {
        return RGBColor(
            (r * factor) >> 8,
            (g * factor) >> 8,
            (b * factor) >> 8
        );
    }
};

// Structure pour la configuration des LEDs par rangée
struct LedRow {
    int8_t leds[5];  // Utiliser int8_t au lieu d'int pour économiser la mémoire
    uint8_t count;   // Nombre de LEDs actives dans cette rangée
    
    constexpr LedRow(int8_t led0 = -1, int8_t led1 = -1, int8_t led2 = -1, 
                     int8_t led3 = -1, int8_t led4 = -1) 
        : leds{led0, led1, led2, led3, led4}, count(0) {
        // Compter le nombre de LEDs valides à la compilation
        for (int i = 0; i < 5; i++) {
            if (leds[i] >= 0) count++;
        }
    }
    
    // Accès sécurisé aux LEDs de la rangée
    constexpr int8_t getLed(uint8_t index) const {
        return (index < count && index < 5) ? leds[index] : -1;
    }
    
    // Vérifie si une LED spécifique est dans cette rangée
    constexpr bool containsLed(int8_t ledIndex) const {
        for (int i = 0; i < 5; i++) {
            if (leds[i] == ledIndex) return true;
        }
        return false;
    }
};

// Constantes en PROGMEM pour économiser la RAM
namespace LedConfig {
    // Configuration des rangées de LEDs (plus efficace que le tableau 2D)
    extern const LedRow LED_ROWS[6] PROGMEM;
    
    // Couleurs initiales des LEDs (optimisées)
    extern const RGBColor INIT_LED_COLORS[NUM_LEDS] PROGMEM;
    
    // Palette de couleurs (stockée en PROGMEM)
    extern const RGBColor COLOR_PALETTE[128] PROGMEM;
    
    // Fonctions utilitaires inlines pour de meilleures performances
    inline RGBColor getInitColor(uint8_t ledIndex) {
        if (ledIndex >= NUM_LEDS) return RGBColor();
        RGBColor color;
        memcpy_P(&color, &INIT_LED_COLORS[ledIndex], sizeof(RGBColor));
        return color;
    }
    
    inline RGBColor getPaletteColor(uint8_t colorIndex) {
        if (colorIndex >= 128) return RGBColor();
        RGBColor color;
        memcpy_P(&color, &COLOR_PALETTE[colorIndex], sizeof(RGBColor));
        return color;
    }
    
    // Fonction pour obtenir les LEDs d'une rangée
    inline LedRow getLedRow(uint8_t rowIndex) {
        if (rowIndex >= 6) rowIndex = 0;
        LedRow row;
        memcpy_P(&row, &LED_ROWS[rowIndex], sizeof(LedRow));
        return row;
    }
    
    // Constantes pour l'efficacité
    static constexpr uint8_t NUM_ROWS = 6;
    static constexpr uint8_t PALETTE_SIZE = 128;
    static constexpr uint8_t MAX_LEDS_PER_ROW = 5;
}

// Fonctions utilitaires pour les couleurs
namespace LedUtils {
    // Correction gamma pour un rendu plus naturel
    uint8_t gammaCorrect(uint8_t value);
    
    // Correction gamma pour une couleur complète
    RGBColor gammaCorrectColor(const RGBColor& color);
    
    // Interpolation entre deux couleurs
    RGBColor interpolateColor(const RGBColor& color1, const RGBColor& color2, uint8_t ratio);
    
    // Mélange de couleurs avec alpha blending
    RGBColor blendColors(const RGBColor& base, const RGBColor& overlay, uint8_t alpha);
    
    // Conversion depuis HSV vers RGB
    RGBColor hsvToRgb(uint8_t hue, uint8_t saturation, uint8_t value);
    
    // Réduction de luminosité progressive (fade)
    RGBColor fadeColor(const RGBColor& color, uint8_t fadeAmount);
    
    // Validation des indices
    inline bool isValidLedIndex(uint8_t index) { return index < NUM_LEDS; }
    inline bool isValidColorIndex(uint8_t index) { return index < LedConfig::PALETTE_SIZE; }
    inline bool isValidRowIndex(uint8_t index) { return index < LedConfig::NUM_ROWS; }
    
    // Couleurs prédéfinies communes
    static constexpr RGBColor BLACK = RGBColor(0, 0, 0);
    static constexpr RGBColor WHITE = RGBColor(255, 255, 255);
    static constexpr RGBColor RED = RGBColor(255, 0, 0);
    static constexpr RGBColor GREEN = RGBColor(0, 255, 0);
    static constexpr RGBColor BLUE = RGBColor(0, 0, 255);
    static constexpr RGBColor YELLOW = RGBColor(255, 255, 0);
    static constexpr RGBColor CYAN = RGBColor(0, 255, 255);
    static constexpr RGBColor MAGENTA = RGBColor(255, 0, 255);
}

extern uint16_t getColorFromIndex(uint8_t index);