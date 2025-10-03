#include "utils.h"
#include "pages.h"
#include "../config/consts.h"
#include "../leds/leds.h"
#include "../display/colors.h"

control_type_t controlType[4] = {NOTE, CC, PC, FIXED};

long unsigned _now = 0;

displayed_item_t displayedItemsArray[] = {
    Empty, //0
    SceneName, //1
    TrackName, //2
    LooperName, //3
    VariationName, //4
    MarkerName, //5
    ActiveSong, //6
    ActiveSection, //7
    NextSong, //8
    NextSection, //9
    ActiveSetlist, //10
    Tempo, //11
    ActiveSongPosition, //12
    ActiveSongIndex, //13
    Setlist_Array_Item, //14
    Song_Array_Item, //15
    Next_Scene_Name, //16
    BeatsPosition, //17
    Active_Song_Seconds //18
};


// Nombre réel d'entrées dans nos dictionnaires
#define ACTUAL_ENTRIES 74

// Dictionnaire des valeurs d'actions (optimisé avec const)
const int button_actions_values[ACTUAL_ENTRIES] = {
    0, 1, 100, 2, 4, 40, 5, 28, 29, 104, 41, 42, 75, 76, 0, 74, 103, 6, 7, 9, 8, 10, 102, 38, 39, 0, 11, 13, 14, 15, 105, 106, 107, 108, 109, 110, 16, 17, 43, 3, 0, 33, 34, 111, 112, 44, 45, 0, 18, 19, 22, 23, 24, 25, 26, 27, 55, 20, 21, 53, 48, 49, 128, 129, 130, 131, 132, 133, 30, 31, 32, 47, 36, 122
};

// Dictionnaire des clés d'actions (stocké en mémoire flash)
const char* const button_actions_keys[ACTUAL_ENTRIES] = {
    "--- Global ---",
    "Play Stop",
    "Pause ",
    "Metro ",
    "Undo ",
    "Redo ",
    "Capture",
    "BPM +1",
    "BPM -1",
    "MIDI Recording Quantization",
    "Re-enable Automation",
    "Back To Arrangement",
    "Arr./Session View",
    "Clip/Device View",
    "--- Arrangement ---",
    "Jump to 1.1.1",
    "Restart From Last Pos.",
    "Arr. Rec",
    "Arr. Loop",
    "Prev Marker",
    "Next Marker",
    "Add/Delete Marker",
    "Loop to Next Marker",
    "Punch In",
    "Punch Out",
    "--- Session ---",
    "Session Rec",
    "Launch Scene",
    "Prev Scene",
    "Next Scene",
    "Jump 4 Scenes Up",
    "Jump 4 Scenes Down",
    "Fixed Length Rec 1 Bars",
    "Fixed Length Rec 2 Bars",
    "Fixed Length Rec 4 Bars",
    "Fixed Length Rec 8 Bars",
    "Jump to Playing Scene",
    "Insert Scene",
    "Capture and Insert Scene",
    "Stop All Clips",
    "--- Setlist ---",
    "Prev Song",
    "Next Song",
    "Prev Section",
    "Next Section",
    "Launch Song",
    "Launch Song No Q",
    "--- Tracks ---",
    "Prev Track",
    "Next Track",
    "Launch Clip",
    "Find Empty Slot",
    "Mute",
    "Solo",
    "Arm",
    "Stop",
    "Fold/Unfold Track",
    "Add Audio Track",
    "Add MIDI Track",
    "Looper",
    "Prev Looper",
    "Next Looper",
    "MIDI Map 1 (Big Button)",
    "MIDI Map 2 (Clear)",
    "Looper",
    "Looper",
    "MIDI Map 5",
    "MIDI Map 6",
    "Arm Track",
    "Mute Track",
    "Show Looper",
    "Add Looper",
    "Clear All",
    "Custom MIDI"
};



const char noteNames[12][3] PROGMEM = {
  "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

void getNoteNameWithOctave(uint8_t noteNumber, char* buffer) {
  if (buffer == nullptr) return;  // Protection contre pointeur null
  
  uint8_t noteIndex = noteNumber % 12;
  int8_t octave = (noteNumber / 12) - 2;
  strcpy_P(buffer, (PGM_P)noteNames[noteIndex]);
  char octaveStr[3];
  itoa(octave, octaveStr, 10);
  strcat(buffer, octaveStr);
}


const char* getActionName(uint8_t pageNumber, uint8_t buttonNum) {
  // Validation stricte des paramètres d'entrée
  if (pageNumber >= 6) {
    DEBUG_LOG_VALUE("getActionName: invalid pageNumber: ", pageNumber);
    return "ERR_PAGE";
  }
  
  if (buttonNum >= 5) {
    DEBUG_LOG_VALUE("getActionName: invalid buttonNum: ", buttonNum);
    return "ERR_BTN";
  }
  
  // Vérification que la page est initialisée
  const Page& page = pages[pageNumber];
  if (page.pageType == NONE) {
    DEBUG_LOG_VALUE("getActionName: page not initialized: ", pageNumber);
    return "UNINIT";
  }
  
  static char buffer[32];  // Buffer agrandi pour sécurité (était 20)
  static char formattedActionName[40];  // Buffer pour le résultat formaté
  
  // Protection contre les accès mémoire invalides
  int controlNum = 0;
  uint8_t custom = 0;
  control_type_t controlType = CC;
  uint8_t channel = 0;
  
  // Accès sécurisé aux tableaux de contrôle
  if (buttonNum < sizeof(page.control_cc)/sizeof(page.control_cc[0])) {
    controlNum = page.control_cc[buttonNum];
    custom = page.control_custom[buttonNum];
    controlType = page.control_type[buttonNum];
    channel = page.control_ch[buttonNum];
  } else {
    DEBUG_LOG_VALUE("getActionName: buttonNum out of control array bounds: ", buttonNum);
    return "OOB";
  }
  
  const char* actionName;

  if (custom == 0) {
    // Recherche sécurisée dans le dictionnaire
    actionName = "Unknown";  // Valeur par défaut
    
    // Protection contre ACTUAL_ENTRIES non défini ou invalide
    #ifndef ACTUAL_ENTRIES
    #define ACTUAL_ENTRIES 0
    #endif
    
    if (ACTUAL_ENTRIES > 0 && button_actions_values && button_actions_keys) {
      for (int i = 0; i < ACTUAL_ENTRIES; i++) {
        if (button_actions_values[i] == controlNum) {
          actionName = button_actions_keys[i];
          if (actionName) {  // Vérification que le pointeur n'est pas null
            break;
          }
        }
      }
    } else {
      DEBUG_LOGLN("getActionName: button_actions arrays not available");
    }
  } else {
    // Construction sécurisée du nom personnalisé
    uint8_t safeChannel = (channel < 16) ? channel + 1 : 1;  // Protection overflow
    
    switch (controlType) {
      case NOTE: {
        char noteNameWithOctave[8];  // Buffer agrandi
        memset(noteNameWithOctave, 0, sizeof(noteNameWithOctave));
        
        // Validation du controlNum pour les notes
        if (controlNum >= 0 && controlNum <= 127) {
          getNoteNameWithOctave(controlNum, noteNameWithOctave);
        } else {
          strcpy(noteNameWithOctave, "INV");
        }
        
        int result = snprintf(buffer, sizeof(buffer), "%s Ch.%d", noteNameWithOctave, safeChannel);
        if (result < 0 || result >= sizeof(buffer)) {
          strcpy(buffer, "NOTE_ERR");
        }
        break;
      }
      case CC:
        if (controlNum >= 0 && controlNum <= 127) {
          int result = snprintf(buffer, sizeof(buffer), "CC%d Ch.%d", controlNum, safeChannel);
          if (result < 0 || result >= sizeof(buffer)) {
            strcpy(buffer, "CC_ERR");
          }
        } else {
          strcpy(buffer, "CC_INV");
        }
        break;
      case PC:
        if (controlNum >= 0 && controlNum <= 127) {
          int result = snprintf(buffer, sizeof(buffer), "PC%d Ch.%d", controlNum, safeChannel);
          if (result < 0 || result >= sizeof(buffer)) {
            strcpy(buffer, "PC_ERR");
          }
        } else {
          strcpy(buffer, "PC_INV");
        }
        break;
      default:
        strcpy(buffer, "UNK_TYPE");
        break;
    }
    actionName = buffer;
  }

  // Validation finale de actionName
  if (!actionName) {
    DEBUG_LOGLN("getActionName: actionName is null");
    return "NULL_ACT";
  }

  // Optimisation: vérification rapide s'il n'y a pas d'espaces
  const char* spacePtr = strchr(actionName, ' ');
  if (spacePtr == nullptr) {
    return actionName;  // Pas d'espaces, retour direct
  }

  // Transformation sécurisée des espaces en sauts de ligne
  size_t len = strlen(actionName);
  if (len >= sizeof(formattedActionName)) {
    len = sizeof(formattedActionName) - 1;  // Évite le débordement
  }
  
  size_t j = 0;
  for (size_t i = 0; i < len && j < sizeof(formattedActionName) - 1; i++) {
    formattedActionName[j++] = (actionName[i] == ' ') ? '\n' : actionName[i];
  }
  formattedActionName[j] = '\0';

  return formattedActionName;
}

uint16_t getActionColor(uint8_t pageNumber, uint8_t buttonNum) {
  // Validation stricte des paramètres
  if (pageNumber >= 6) {
    DEBUG_LOG_VALUE("getActionColor: invalid pageNumber: ", pageNumber);
    return defaultBgColor;
  }
  
  if (buttonNum >= 5) {
    DEBUG_LOG_VALUE("getActionColor: invalid buttonNum: ", buttonNum);
    return defaultBgColor;
  }
  
  // Vérification de sécurité pour l'accès au tableau l[]
  if (buttonNum + 1 >= NUM_LEDS) {
    DEBUG_LOG_VALUE("getActionColor: LED index out of bounds: ", buttonNum + 1);
    return defaultBgColor;
  }
  
  // Vérification que le tableau l[] est initialisé
  if (!l) {
    DEBUG_LOGLN("getActionColor: LED array not initialized");
    return defaultBgColor;
  }
  
  // Accès sécurisé aux valeurs de couleur LED
  const auto& led = l[buttonNum + 1];
  
  // Validation des valeurs de couleur
  uint8_t r = led.r;
  uint8_t g = led.g;
  uint8_t b = led.b;
  
  // Protection contre les valeurs invalides
  if (r > 127) r = 127;
  if (g > 127) g = 127;
  if (b > 127) b = 127;
  
  // Conversion sécurisée des couleurs LED avec protection overflow
  uint16_t result = RRRGB888toRGB565(r << 1, g << 1, b << 1);
  
  // Validation du résultat
  if (result == 0 && (r > 0 || g > 0 || b > 0)) {
    DEBUG_LOGLN("getActionColor: color conversion resulted in black, using default");
    return defaultBgColor;
  }
  
  return result;
}
