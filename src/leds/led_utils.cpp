#include "led_utils.h"
#include "../config/consts.h"

/**
 * @file led_utils.cpp
 * @brief Implementation of optimised LED utility functions
 * @details Memory-efficient color management with PROGMEM storage
 */

// Configuration des rangées de LEDs (plus efficace et lisible)
const LedRow LedConfig::LED_ROWS[6] PROGMEM = {
    LedRow(0),      // Rangée 0: LEDs 0, 1
    LedRow(1),      // Rangée 1: LEDs 2, 3  
    LedRow(2),      // Rangée 2: LEDs 4, 5
    LedRow(3),      // Rangée 3: LEDs 6, 7
    LedRow(4),      // Rangée 4: LEDs 8, 9
    LedRow(5)     // Rangée 5: LEDs 10, 11
};

// Couleurs initiales optimisées (structure plus claire)
const RGBColor LedConfig::INIT_LED_COLORS[NUM_LEDS] PROGMEM = {
    RGBColor(160, 0, 200),   // LED 0: Rouge-Magenta (80*2, 0*2, 100*2)
    RGBColor(0, 156, 92),    // LED 1: Vert-Cyan (0*2, 78*2, 46*2)
    RGBColor(34, 46, 160),   // LED 2: Bleu-Indigo (17*2, 23*2, 80*2)
    RGBColor(200, 200, 0),   // LED 3: Jaune (100*2, 100*2, 0*2)
    RGBColor(248, 98, 0),    // LED 4: Orange (124*2, 49*2, 0*2)
    RGBColor(180, 0, 24)     // LED 5: Rouge sombre (90*2, 0*2, 12*2)
};

// Palette de couleurs stockée en PROGMEM pour économiser la RAM
const RGBColor LedConfig::COLOR_PALETTE[128] PROGMEM = {
    // Grays and whites (0-9)
    RGBColor(100, 100, 100),    // 0 - Gris moyen
    RGBColor(228, 78, 0),       // 1 - Orange foncé
    RGBColor(170, 88, 16),      // 2 - Brun-orange
    RGBColor(152, 98, 18),      // 3 - Brun clair
    RGBColor(104, 124, 2),      // 4 - Vert olive
    RGBColor(110, 176, 0),      // 5 - Vert lime
    RGBColor(110, 154, 32),     // 6 - Vert-jaune
    RGBColor(60, 122, 58),      // 7 - Vert forêt
    RGBColor(38, 42, 100),      // 8 - Bleu nuit
    RGBColor(28, 6, 132),       // 9 - Bleu indigo
    
    // Vibrant colors (10-19)
    RGBColor(86, 48, 132),      // 10 - Violet
    RGBColor(214, 12, 118),     // 11 - Magenta foncé
    RGBColor(254, 12, 56),      // 12 - Rouge carmin
    RGBColor(254, 254, 254),    // 13 - Blanc pur
    RGBColor(254, 0, 0),        // 14 - Rouge pur
    RGBColor(254, 56, 0),       // 15 - Rouge-orange
    RGBColor(118, 44, 2),       // 16 - Brun roux
    RGBColor(162, 112, 0),      // 17 - Ocre
    RGBColor(92, 120, 4),       // 18 - Vert olive clair
    RGBColor(102, 196, 10),     // 19 - Vert lime vif
    
    // Nature colors (20-39)
    RGBColor(80, 172, 72),      // 20 - Vert prairie
    RGBColor(80, 152, 128),     // 21 - Vert-bleu
    RGBColor(58, 104, 198),     // 22 - Bleu royal
    RGBColor(24, 24, 198),      // 23 - Bleu électrique
    RGBColor(150, 0, 198),      // 24 - Violet pourpre
    RGBColor(224, 2, 190),      // 25 - Magenta
    RGBColor(224, 2, 58),       // 26 - Rouge vermillon
    RGBColor(184, 140, 60),     // 27 - Or
    RGBColor(200, 64, 24),      // 28 - Orange brûlé
    RGBColor(212, 78, 0),       // 29 - Orange foncé
    RGBColor(148, 78, 8),       // 30 - Brun
    RGBColor(144, 134, 22),     // 31 - Olive
    RGBColor(148, 160, 22),     // 32 - Vert-jaune
    RGBColor(152, 200, 0),      // 33 - Vert lime
    RGBColor(126, 200, 36),     // 34 - Vert pomme
    RGBColor(118, 154, 70),     // 35 - Vert mousse
    RGBColor(116, 172, 132),    // 36 - Vert-bleu pastel
    RGBColor(144, 106, 132),    // 37 - Lavande
    RGBColor(144, 76, 132),     // 38 - Prune
    RGBColor(170, 22, 162),     // 39 - Magenta foncé
    
    // Pastel and light colors (40-79)
    RGBColor(194, 128, 124),    // 40 - Rose poudré
    RGBColor(180, 128, 96),     // 41 - Beige rosé
    RGBColor(228, 84, 64),      // 42 - Terra cotta
    RGBColor(254, 106, 0),      // 43 - Orange vif
    RGBColor(202, 96, 0),       // 44 - Orange foncé
    RGBColor(98, 98, 0),        // 45 - Jaune olive
    RGBColor(94, 138, 0),       // 46 - Vert jaunâtre
    RGBColor(102, 152, 14),     // 47 - Vert lime
    RGBColor(102, 180, 88),     // 48 - Vert menthe
    RGBColor(92, 106, 88),      // 49 - Vert grisâtre
    RGBColor(84, 108, 134),     // 50 - Bleu-gris
    RGBColor(106, 72, 176),     // 51 - Violet
    RGBColor(148, 52, 174),     // 52 - Magenta
    RGBColor(174, 64, 98),      // 53 - Bordeaux
    RGBColor(186, 34, 50),      // 54 - Rouge grenat
    RGBColor(94, 80, 50),       // 55 - Brun olive
    RGBColor(102, 0, 0),        // 56 - Rouge foncé
    RGBColor(102, 18, 0),       // 57 - Rouge-brun
    RGBColor(32, 12, 0),        // 58 - Brun très foncé
    RGBColor(94, 76, 0),        // 59 - Ocre foncé
    RGBColor(120, 158, 0),      // 60 - Vert-jaune
    RGBColor(80, 158, 0),       // 61 - Vert lime
    RGBColor(80, 188, 66),      // 62 - Vert menthe
    RGBColor(0, 50, 86),        // 63 - Bleu marine
    RGBColor(0, 0, 108),        // 64 - Bleu foncé
    RGBColor(0, 110, 0),        // 65 - Vert pur
    RGBColor(84, 0, 104),       // 66 - Violet foncé
    RGBColor(128, 0, 104),      // 67 - Magenta foncé
    RGBColor(166, 0, 70),       // 68 - Rouge-violet
    RGBColor(24, 20, 16),       // 69 - Brun très foncé
    RGBColor(186, 46, 0),       // 70 - Orange-rouge
    RGBColor(64, 26, 0),        // 71 - Brun-rouge
    RGBColor(142, 24, 0),       // 72 - Rouge foncé
    RGBColor(56, 16, 0),        // 73 - Brun foncé
    RGBColor(118, 86, 40),      // 74 - Brun clair
    RGBColor(56, 38, 20),       // 75 - Brun moyen
    RGBColor(74, 28, 10),       // 76 - Brun foncé
    RGBColor(26, 12, 4),        // 77 - Sépia
    RGBColor(200, 176, 46),     // 78 - Jaune doré
    RGBColor(64, 56, 14),       // 79 - Olive foncé
    
    // Bright and saturated (80-119)
    RGBColor(255, 240, 0),     // 80 - Or brillant
    RGBColor(50, 40, 4),        // 81 - Brun doré
    RGBColor(144, 204, 10),     // 82 - Vert-jaune vif
    RGBColor(46, 66, 2),        // 83 - Vert olive foncé
    RGBColor(96, 204, 18),      // 84 - Vert lime vif
    RGBColor(30, 66, 6),        // 85 - Vert foncé
    RGBColor(40, 154, 16),      // 86 - Vert herbe
    RGBColor(12, 50, 4),        // 87 - Vert très foncé
    RGBColor(62, 110, 2),       // 88 - Vert olive
    RGBColor(20, 34, 0),        // 89 - Vert très sombre
    RGBColor(78, 204, 68),      // 90 - Vert menthe vif
    RGBColor(24, 66, 22),       // 91 - Vert forêt
    RGBColor(40, 124, 82),      // 92 - Vert-bleu
    RGBColor(16, 50, 32),       // 93 - Vert foncé
    RGBColor(0, 48, 28),        // 94 - Vert très foncé
    RGBColor(38, 138, 204),     // 95 - Bleu-vert vif
    RGBColor(12, 44, 66),       // 96 - Bleu-vert foncé
    RGBColor(42, 78, 200),      // 97 - Bleu royal
    RGBColor(14, 24, 64),       // 98 - Bleu marine
    RGBColor(20, 40, 204),      // 99 - Bleu électrique
    RGBColor(6, 12, 66),        // 100 - Bleu très foncé
    RGBColor(22, 8, 184),       // 101 - Bleu-violet
    RGBColor(6, 2, 58),         // 102 - Bleu indigo foncé
    RGBColor(20, 56, 152),      // 103 - Bleu moyen
    RGBColor(8, 22, 60),        // 104 - Bleu foncé
    RGBColor(44, 44, 204),      // 105 - Bleu-violet clair
    RGBColor(68, 26, 204),      // 106 - Violet
    RGBColor(30, 0, 0),         // 107 - Rouge très foncé
    RGBColor(120, 34, 204),     // 108 - Magenta-violet
    RGBColor(38, 10, 66),       // 109 - Violet foncé
    RGBColor(106, 26, 96),      // 110 - Magenta foncé
    RGBColor(34, 8, 30),        // 111 - Violet très foncé
    RGBColor(204, 12, 40),      // 112 - Rouge carmin
    RGBColor(66, 4, 12),        // 113 - Rouge foncé
    RGBColor(204, 34, 168),     // 114 - Magenta
    RGBColor(66, 10, 54),       // 115 - Rouge-violet foncé
    
    // Special colors (116-127)
    RGBColor(0, 0, 0),          // 116 - Noir
    RGBColor(0, 0, 0),          // 117 - Noir (dupliqué)
    RGBColor(178, 178, 178),    // 118 - Gris clair
    RGBColor(52, 52, 52),       // 119 - Gris foncé
    RGBColor(255, 255, 255),    // 120 - Blanc pur (déjà à 255)
    RGBColor(178, 178, 178),    // 121 - Gris clair (dupliqué)
    RGBColor(255, 255, 255),    // 122 - Gris moyen (était 153*2=306, plafonné à 255)
    RGBColor(128, 128, 128),    // 123 - Gris foncé
    RGBColor(40, 40, 40),       // 124 - Gris très foncé
    RGBColor(0, 0, 255),        // 125 - Bleu pur (déjà à 255)
    RGBColor(0, 255, 0),        // 126 - Vert pur (déjà à 255)
    RGBColor(255, 0, 0)         // 127 - Rouge pur (déjà à 255)
};

// Fonctions utilitaires
namespace LedUtils {
    
    // Correction gamma pour un rendu plus naturel des LEDs
    uint8_t gammaCorrect(uint8_t value) {
        // Table gamma 2.2 pour les LEDs - stockée en PROGMEM
        static const uint8_t PROGMEM gamma8[] = {
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
            1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
            2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
            5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
           10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
           17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
           25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
           37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
           51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
           69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
           90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
          115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
          144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
          177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
          215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
        };
        return pgm_read_byte(&gamma8[value]);
    }
    
    // Correction gamma pour une couleur complète
    RGBColor gammaCorrectColor(const RGBColor& color) {
        return RGBColor(
            gammaCorrect(color.r),
            gammaCorrect(color.g),
            gammaCorrect(color.b)
        );
    }
    
    // Interpolation linéaire entre deux couleurs
    RGBColor interpolateColor(const RGBColor& color1, const RGBColor& color2, uint8_t ratio) {
        // ratio: 0 = color1, 255 = color2
        uint16_t inv_ratio = 255 - ratio;
        
        return RGBColor(
            (color1.r * inv_ratio + color2.r * ratio) >> 8,
            (color1.g * inv_ratio + color2.g * ratio) >> 8,
            (color1.b * inv_ratio + color2.b * ratio) >> 8
        );
    }
    
    // Mélange de couleurs avec alpha blending
    RGBColor blendColors(const RGBColor& base, const RGBColor& overlay, uint8_t alpha) {
        uint16_t inv_alpha = 255 - alpha;
        
        return RGBColor(
            (base.r * inv_alpha + overlay.r * alpha) >> 8,
            (base.g * inv_alpha + overlay.g * alpha) >> 8,
            (base.b * inv_alpha + overlay.b * alpha) >> 8
        );
    }
    
    // Conversion HSV vers RGB optimisée
    RGBColor hsvToRgb(uint8_t hue, uint8_t saturation, uint8_t value) {
        uint8_t region, remainder, p, q, t;
        
        if (saturation == 0) {
            return RGBColor(value, value, value);
        }
        
        region = hue / 43;
        remainder = (hue - (region * 43)) * 6;
        
        p = (value * (255 - saturation)) >> 8;
        q = (value * (255 - ((saturation * remainder) >> 8))) >> 8;
        t = (value * (255 - ((saturation * (255 - remainder)) >> 8))) >> 8;
        
        switch (region) {
            case 0:  return RGBColor(value, t, p);
            case 1:  return RGBColor(q, value, p);
            case 2:  return RGBColor(p, value, t);
            case 3:  return RGBColor(p, q, value);
            case 4:  return RGBColor(t, p, value);
            default: return RGBColor(value, p, q);
        }
    }
    
    // Réduction de luminosité progressive
    RGBColor fadeColor(const RGBColor& color, uint8_t fadeAmount) {
        return RGBColor(
            color.r > fadeAmount ? color.r - fadeAmount : 0,
            color.g > fadeAmount ? color.g - fadeAmount : 0,
            color.b > fadeAmount ? color.b - fadeAmount : 0
        );
    }
}