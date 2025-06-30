#pragma once
#include <Arduino.h>

class UUID {
private:
    char uuid_str[37]; // 36 caractères + null terminator
    
public:
    UUID() {
        memset(uuid_str, 0, sizeof(uuid_str));
    }
    
    void generate() {
        // Génération d'un UUID v4 simple (pseudo-aléatoire)
        randomSeed(micros() + analogRead(A0));
        
        // Générer les composants séparément pour éviter les overflow
        uint32_t part1 = ((uint32_t)random(0x10000) << 16) | random(0x10000);
        uint16_t part2 = random(0x10000);
        uint16_t part3 = (random(0x10000) & 0x0fff) | 0x4000; // Version 4
        uint16_t part4 = (random(0x10000) & 0x3fff) | 0x8000; // Variant bits
        
        // Générer la partie finale en deux morceaux
        uint32_t part5_high = random(0x10000);
        uint32_t part5_low = ((uint32_t)random(0x10000) << 16) | random(0x10000);
        
        sprintf(uuid_str, "%08lx-%04x-%04x-%04x-%04x%08lx",
            (unsigned long)part1,
            part2,
            part3,
            part4,
            part5_high,
            (unsigned long)part5_low
        );
    }
    
    const char* toCharArray() {
        if (strlen(uuid_str) == 0) {
            generate(); // Génère automatiquement si pas encore fait
        }
        return uuid_str;
    }
    
    String toString() {
        return String(toCharArray());
    }
};
