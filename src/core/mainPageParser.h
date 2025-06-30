#pragma once
#include <Arduino.h>
#include "../display/colors.h"
#include "../core/utils.h"
#include "../config/consts.h"

// void getItStarted();
class Main{
    public:
        uint16_t songColors[MAX_SONG], activeSongColor = _White, activeSongColorShade = _LightGray, nextSongColor = _LightGray, activeSectionColor = _LightGray;

        uint8_t selectedMode = 0, activeSectionIndex = 0;

        char activeSongName[MAX_SONG_NAME] = "Active Song";
        char activeSectionName[MAX_SONG_NAME] = "";
        char nextSectionName[MAX_SONG_NAME] = "Next Section";
        char nextSceneName[MAX_SONG_NAME] = "Next Scene";
        char sceneName[MAX_SONG_NAME] = "Scene Name";
        char trackName[MAX_SONG_NAME] = "Track Name";
        char looperName[MAX_SONG_NAME] = "Looper Name";
        char markerName[MAX_SONG_NAME] = "Marker Name";
        float songPercentage = 0.0f;
        int currentSeconds = 0;
        int previousSeconds = 0;
        char itemToDisplay[12][MAX_SONG_NAME]; 
        char sections[MAX_SECTIONS][MAX_SONG_NAME], songsList[MAX_SONG][MAX_SONG_NAME] = {"Song 1", "Song 2", "Song 3", "Song 4", "Song 5", "Song 6", "Song 7", "Song 8"},
        setlistsList[MAX_SONG][MAX_SONG_NAME] = {"Setlist 1", "Setlist 2", "Setlist 3", "Setlist 4", "Setlist 5", "Setlist 6", "Setlist 7", "Setlist 8"};
        char activeSetlistName[MAX_SONG_NAME] = "Setlist";
        int songDurations[MAX_SONG];
        float activeSongDuration;
        int setlistTotalTime = 0;
        char nextSongName[MAX_SONG_NAME] = "Next Song";
        uint8_t songsListSize = 8;
        uint8_t setlistsListSize = 8;
        float activeSongStart, activeSongEnd, activeSectionStart, activeSectionEnd;

        int activeSongIndex, lastProgressBar = 0, previous_sec = 0;
        float remainingTimeInSet = 0, remainingTimeInSong=0;

        void sendSetlistArrayRequest();
        void sendSongArrayRequest();
        void parseDisplayItem(uint8_t itemType, char* strBuf, int arg2);
        void parseArrayItem(uint8_t itemType, char* strBuf, uint8_t listIndex, uint8_t listLength);
        void setActiveSongName(char* );
        void setActiveSongColor(uint16_t );
        void setActiveSongStart(float);
        void setActiveSongEnd(float);
        void setActiveSongIndex(int);
        void setActiveSongDuration(float);
        void setActiveSongProgress(float);
        void setRemainingTimeInSet(float);
        void setRemainingTimeInSong(float);
        void setNextSongName(char* );
        void setNextSongColor(uint16_t );
        void setActiveSectionName(char* );
        void setActiveSectionIndex(int);
        void setActiveSectionStart(float);
        void setActiveSectionEnd(float);
        void setActiveSectionColor(uint16_t );
        void setActiveSetlistName(char* );
        void setSections(char sections[][MAX_SONG_NAME], int count);
        void setSongs(char songs[][MAX_SONG_NAME], int count);
        void setSongDurations(int durations[], int count);
        void setSongColors(char colors[][MAX_SONG_NAME], int count);
        void setCCReceived(uint8_t channel,uint8_t control,uint8_t value);

        void configureButton(uint8_t user_mode, uint8_t controlNum, 
            uint8_t control_type, uint8_t control_cc, uint8_t control_ch, 
            uint8_t isCustom, uint8_t toggled);
        void configureLed(uint8_t user_mode, uint8_t controlNum, 
            uint8_t control_type, uint8_t control_led_cc, 
            uint8_t control_led_ch);
        void configureDisplay(uint8_t user_mode, uint8_t display_num, 
            uint8_t display_type);
        void configurePedal(uint8_t user_mode, uint8_t controlNum,
            uint8_t control_type, uint8_t control_cc, 
            uint8_t control_ch, uint8_t isCustom, uint8_t toggled);

};

extern Main _main;


int indexOfItem(displayed_item_t itemToFind);