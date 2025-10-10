#pragma once

#include "LittleFS.h"
#include <ArduinoJson.h>
// #include "../core/pages.h"
#include "consts.h"
#include "../core/settingsParser.h"
#include "../core/mainPageParser.h"
#include "../core/globalParser.h"
#include "../core/pages.h"

// Déclaration de la classe JSONManager
class JSONManager {
private:
    const char* filename;
    JsonDocument doc;
    JsonObject options;
    JsonArray pagesArray;

public:
    JSONManager(const char* fname);

    bool begin();
    bool loadFile();
    bool saveFile();

    bool writeJSONControl(int user_mode, int controlNum, int control_type, int control_cc, int control_ch, int isCustom, int toggled, const char* actionName, int color, int luminance);
    // bool writeJSONPedal(int user_mode, int controlNum, int control_type, int control_cc, int control_ch, int isCustom, int toggled);
    bool writeJSONDisplay(int user_mode, int display_num, int display_type);
    bool writeOption(const char* key, int value);
    bool writeManualIP(int* ip);

    // Getters
    bool getUseDHCP() const;
    bool getNightMode() const;
    int getSelectedMode() const;
    int getUserPagesAmount() const;
    int getLedBrightness() const;
    int getIPNum(int i) const;
    int getDisplayBrightness() const;
    int getPort() const;
    // pedal_type getPedal(int index) const;
    // bool getPages();
    control_type_t getControlType(int page, int control) const;
    int getControlCC(int page, int control) const;
    int getControlChannel(int page, int control) const;
    int getControlToggle(int page, int control) const;
    int getControlLedCC(int page, int control) const;
    int getControlLedChannel(int page, int control) const;
    int getControlCustom(int page, int control) const;
    const char* getActionName(int page, int control, char* buffer, size_t bufferSize) const;
    int getLedColorIndex(int page, int control) const;
    bool getLuminance(int page, int control) const;
    displayed_item_t getDisplayType(int page, int displayNum) const;

    // Envoi JSON
    void sendJSONConfig();
    void sendJSONConfigOSC();
    void sendJSONPage(int pageNum);
    void sendJSONPageOSC(int pageNum);
    void setup();
    
    // Fonctions de débogage
    void debugListFiles();
    void debugPrintJSON();
};

// Instance globale
extern JSONManager jsonManager;

// Fonctions utilitaires
int getArg(String arg);
void extractIPAddress(String input);
