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
    JsonArray userPagesArray;

public:
    JSONManager(const char* fname);

    bool begin();
    bool loadFile();
    bool saveFile();

    bool writeJSONControl(int user_mode, int controlNum, int control_type, int control_cc, int control_ch, int isCustom, int toggled);
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
    bool getPages();

    // Envoi JSON
    void sendJSONConfig();
    void sendJSONConfigOSC();
    void sendJSONPage(int pageNum);
    void sendJSONPageOSC(int pageNum);
    void setup();
};

// Instance globale
extern JSONManager jsonManager;

// Fonctions utilitaires
int getArg(String arg);
void extractIPAddress(String input);
