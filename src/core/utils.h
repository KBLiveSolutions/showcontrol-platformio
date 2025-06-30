#pragma once
#include "../config/consts.h"

// Macros de debug centralisées pour une utilisation dans tout le projet
#define DEBUG_LOG(message) if(oscDebug) { Serial.print(message); }
#define DEBUG_LOGLN(message) if(oscDebug) { Serial.println(message); }
#define DEBUG_LOG_VALUE(label, value) if(oscDebug) { Serial.print(label); Serial.println(value); }

// Macros spécifiques pour le debug ethernet
#define ETHERNET_DEBUG_LOG(message) if(ethernetDebug) { Serial.print(message); }
#define ETHERNET_DEBUG_LOGLN(message) if(ethernetDebug) { Serial.println(message); }
#define ETHERNET_DEBUG_LOG_VALUE(label, value) if(ethernetDebug) { Serial.print(label); Serial.println(value); }

extern long unsigned _now;

// Variables de debug globales
extern bool ethernetDebug;
extern bool oscDebug;

typedef enum { NOTE, CC, PC, FIXED } control_type_t; 
extern control_type_t controlType[4];
enum menu_type_t { MAIN_MENU, SONG_MENU, SETLIST_MENU, MENU_OFF };

enum displayed_item_t { 
    Empty,  
    SceneName,
    TrackName,
    LooperName,
    VariationName,
    MarkerName,
    ActiveSong,
    ActiveSection,
    NextSong,
    NextSection,
    ActiveSetlist,
    Tempo,
    ActiveSongPosition,
    ActiveSongIndex,
    Setlist_Array_Item,
    Song_Array_Item,
    Next_Scene_Name,
    BeatsPosition,
    Active_Song_Seconds
};

extern displayed_item_t displayedItemsArray[19];

// Nombre réel d'entrées dans nos dictionnaires 
#define ACTUAL_ENTRIES 74

// Define the dictionary values
extern const int button_actions_values[ACTUAL_ENTRIES];
extern const char* const button_actions_keys[ACTUAL_ENTRIES];