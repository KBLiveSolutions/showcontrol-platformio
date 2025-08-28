#pragma once
#include <Arduino.h>
#include "../config/jsonManager.h"

class Settings{
    public:

        bool MIDIConnected = false;
        uint8_t ledBrightness = 1;
        uint8_t displayBrightness = 100;
        int audioInterfaceScene = 1;
        bool nightMode = false;
        uint8_t userPagesAmount = 0;
        bool isLocked = false;
        bool isRunning = false; 
        
        void configureDisplay(uint8_t user_mode, uint8_t display_num, uint8_t display_type);
        void storeActivePage(uint8_t pageNum);
        void parseDisplayItem(uint8_t itemType, char* strBuf, int arg2);

        void getItStarted();

        void onIPReceived(int ip[4]); 
        void onDNSReceived(int ip[4]); 
        void onGatewayReceived(int ip[4]); 
        void onSubnetReceived(int ip[4]);
        void onPortReceived(int32_t port);
        void onDHCPReceived(int data);
        void onPagesAmountReceived(int data);
        void onRemoveUserPage(int pageNum);

        void setLedBrightness(uint8_t value);
        void setDisplayBrightness(uint8_t value);
        void exitSettings();
        void changeSettingsValue(int value);
        void validateSettings();
        void setup();

    private:
        // Fonctions utilitaires pour la gestion des paramètres
        void clampSettingValue(int& currentValue, int minVal, int maxVal);
        void updateSettingValue(int selectedItem, int deltaValue, int minVal, int maxVal);
};

extern Settings settings;