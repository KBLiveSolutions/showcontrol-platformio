#pragma once

#include <Arduino.h>
#include "../sprites.h"
#include "../../osc/ethernetManager.h"
#include "../../core/settingsParser.h"

// Types et structures
enum ItemType { BOOL_TYPE, INT_TYPE, CHAR_PTR_TYPE };

struct ItemContent {
  ItemType type;
  union {
    bool boolValue;
    int intValue;
    const char* charPtrValue;
  };
};

class SettingsPage {
  public:

    ItemContent itemContent[ITEMSAMNT] = {
    {INT_TYPE, .intValue = settings.ledBrightness},
    {INT_TYPE, .intValue = settings.displayBrightness},
    {INT_TYPE, .intValue = manualIP[0]},
    {INT_TYPE, .intValue = manualIP[1]},
    {INT_TYPE, .intValue = manualIP[2]},
    {INT_TYPE, .intValue = manualIP[3]},
    {INT_TYPE, .intValue = showcontrolLocalPort},
    {CHAR_PTR_TYPE, .charPtrValue = "Save & Exit"},
    // {INT_TYPE, .intValue = subnet[1]},
    // {INT_TYPE, .intValue = subnet[2]},
    // {INT_TYPE, .intValue = subnet[3]}
  };

    // Variables globales
    int selectedItem = 0;
    bool isEditingValue = false;
    // ItemContent itemContent[ITEMSAMNT];
    
    // Fonctions d'affichage et de gestion des settings
    void showSettingsSprite(int i, const char* text, uint16_t txtColor);
    void updateSelection();
    void showPage();
    void clearPage();
    const char* toConstChar(ItemContent item, char* buffer, size_t bufSize);
    void changeSelectedItem(int value);
  private:
    void clearSettingsSprites();
    void showSetting(int selectedItem);
};

// Fonctions utilitaires

extern SettingsPage settingsPage;