#include "utils.h"

control_type_t controlType[4] = {NOTE, CC, PC, FIXED};

bool debugOn = true;

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
