#pragma once
#include "../config/consts.h"
#include <Arduino.h>


#define DEBUG true
#define DEBUG_ETHERNET false
#define DEBUG_OSC true
#define DEBUG_SETUP false

// Macros de debug centralisées pour une utilisation dans tout le projet
#define DEBUG_LOG(message) if(DEBUG) { Serial.print("DEBUG:"); Serial.print(message); }
#define DEBUG_LOGLN(message) if(DEBUG) { Serial.print("DEBUG:"); Serial.println(message); }
#define DEBUG_LOG_CONT(message) if(DEBUG) { Serial.print(message); }
#define DEBUG_LOG_CONT_LN(message) if(DEBUG) { Serial.println(message); }
#define DEBUG_LOG_VALUE(label, value) if(DEBUG) { Serial.print("DEBUG:"); Serial.print(label); Serial.println(value); }

#define DEBUG_ETHERNET_LOG(message) if(DEBUG_ETHERNET) { Serial.print("ETH:"); Serial.print(message); }
#define DEBUG_ETHERNET_LOGLN(message) if(DEBUG_ETHERNET) {  Serial.println(message); }
#define DEBUG_ETHERNET_LOG_VALUE(label, value) if(DEBUG_ETHERNET) { Serial.print("ETH:");  Serial.print(label); Serial.println(value); }

#define DEBUG_SETUP_LOG(message) if(DEBUG_SETUP) { Serial.print("SETUP:");  Serial.print(message); }
#define DEBUG_SETUP_LOGLN(message) if(DEBUG_SETUP) { Serial.print("SETUP:"); Serial.println(message); }
#define DEBUG_SETUP_LOG_VALUE(label, value) if(DEBUG_SETUP) { Serial.print("SETUP:"); Serial.print(label); Serial.println(value); }


#define DEBUG_OSC_LOG(message) if(DEBUG_OSC) { Serial.print("OSC:"); Serial.print(message); }
#define DEBUG_OSC_LOGLN(message) if(DEBUG_OSC) { Serial.print("OSC:"); Serial.println(message); }
#define DEBUG_OSC_LOG_CONT(message) if(DEBUG_OSC) { Serial.print(message); }
#define DEBUG_OSC_LOG_CONT_LN(message) if(DEBUG_OSC) { Serial.println(message); }
#define DEBUG_OSC_LOG_VALUE(label, value) if(DEBUG_OSC) { Serial.print("OSC:"); Serial.print(label); Serial.println(value); }

extern long unsigned _now;

typedef enum { NOTE, CC, PC, FIXED } control_type_t; 
extern control_type_t controlType[4];
enum menu_type_t { MAIN_MENU, SONG_MENU, SETLIST_MENU, MENU_OFF };

enum displayed_item_t { 
    Empty,  
    SceneName,
    TrackName,
    LooperName,
    VariationName,
    LeftMarkerName,
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
    Active_Song_Seconds,
    RightMarkerName
};

extern displayed_item_t displayedItemsArray[20];

// Nombre réel d'entrées dans nos dictionnaires 
#define ACTUAL_ENTRIES 76

// Define the dictionary values
extern const int button_actions_values[ACTUAL_ENTRIES];
extern const char* const button_actions_keys[ACTUAL_ENTRIES];

void getNoteNameWithOctave(uint8_t noteNumber, char* buffer);
const char* getActionName(uint8_t pageNumber, uint8_t buttonNum);
uint16_t getActionColor(uint8_t pageNumber, uint8_t buttonNum);
