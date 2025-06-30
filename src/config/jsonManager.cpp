#include "jsonManager.h"
#include "../osc/OSCManager.h"
#include "../core/utils.h"
#include <Arduino.h>

JSONManager jsonManager("/data.json");

JSONManager::JSONManager(const char* fname) : filename(fname) {
    options = doc["options"];
    userPagesArray = doc["userPages"];
}

bool JSONManager::begin() {
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount LittleFS");
        return false;
    }
    return true;
}

bool JSONManager::loadFile() {
    File file = LittleFS.open(filename, "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return false;
    }
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return false;
    }
    options = doc["options"];
    userPagesArray = doc["userPages"];
    return true;
}

bool JSONManager::saveFile() {
    File file = LittleFS.open(filename, "w");
    if (!file) {
        Serial.println("Failed to open file for writing");
        return false;
    }
    if (serializeJson(doc, file) == 0) {
        Serial.println(F("Failed to write to file"));
        file.close();
        return false;
    }
    file.close();
    return true;
}

bool JSONManager::writeJSONControl(int user_mode, int controlNum, int control_type, int control_cc, int control_ch, int isCustom, int toggled) {
    if (!loadFile()) return false;
    JsonObject userPage = userPagesArray[user_mode];
    userPage["control_custom"][controlNum] = isCustom;
    userPage["control_type"][controlNum] = control_type;
    userPage["control_cc"][controlNum] = control_cc;
    userPage["control_ch"][controlNum] = control_ch;
    userPage["control_toggle"][controlNum] = toggled;
    userPage["led_cc"][controlNum] = control_cc;
    userPage["led_ch"][controlNum] = control_ch;
    return saveFile();
}

// bool JSONManager::writeJSONPedal(int user_mode, int controlNum, int control_type, int control_cc, int control_ch, int isCustom, int toggled) {
//     if (!loadFile()) return false;
//     JsonObject userPage = userPagesArray[user_mode];
//     userPage["pedal_custom"][controlNum] = isCustom;
//     userPage["pedal_type"][controlNum] = control_type;
//     userPage["pedal_cc"][controlNum] = control_cc;
//     userPage["pedal_ch"][controlNum] = control_ch;
//     userPage["pedal_toggle"][controlNum] = toggled;
//     return saveFile();
// }

bool JSONManager::writeJSONDisplay(int user_mode, int display_num, int display_type) {
    if (!loadFile()) return false;
    JsonObject userPage = userPagesArray[user_mode];
    userPage["display"][display_num] = display_type;
    return saveFile();
}

bool JSONManager::writeOption(const char* key, int value) {
    if (!loadFile()) return false;
    options[key] = value;
    return saveFile();
}

bool JSONManager::writeManualIP(int* ip) {
    if (!loadFile()) {
        Serial.println("Failed to load JSON file");
        return false;
    }
    for(int i=0; i<4; i++){
        options["manualIP"][i] = ip[i];
    }
    Serial.print("Manual IP set to: ");
    Serial.printf("%d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
    return saveFile();
}

bool JSONManager::getUseDHCP() const { return options["ethernet. useDHCP"].as<bool>(); }
bool JSONManager::getNightMode() const { return options["nightMode"].as<bool>(); }
int JSONManager::getSelectedMode() const { return options["selectedMode"].as<int>(); }
int JSONManager::getUserPagesAmount() const { return options["userPagesAmount"].as<int>(); }
int JSONManager::getLedBrightness() const { return options["ledBrightness"].as<int>(); }
int JSONManager::getIPNum(int i) const { return options["manualIP"][i].as<int>(); }
int JSONManager::getDisplayBrightness() const { return options["displayBrightness"].as<int>(); }
int JSONManager::getPort() const { return options["port"].as<int>(); }
// pedal_type JSONManager::getPedal(int index) const {
//     String key = "pedal" + String(index);
//     return options[key].as<pedal_type>();
// }

bool JSONManager::getPages() {
    for (size_t i = 1; i <= MAX_NUM_USERS; ++i) {
        JsonObject controlJson = userPagesArray[i];
        for (size_t j = 0; j < NUM_CONTROLS; ++j) {
            control_type_t type = controlJson["control_type"][j].as<control_type_t>();
            int custom = controlJson["control_custom"][j].as<int>();
            int cc = controlJson["control_cc"][j].as<int>();
            int channel = controlJson["control_ch"][j].as<int>();
            int toggled = controlJson["control_toggle"][j].as<int>();
            pages[i].setButtonControl(j, type, cc, channel, custom, toggled);
            cc = controlJson["led_cc"][j].as<int>();
            channel = controlJson["led_ch"][j].as<int>();
            pages[i].setLedControl(j, type, cc, channel);
        }
        JsonArray displayArray = controlJson["display"];
        // for (size_t j = 0; j < 2; ++j) {
        //     pages[i].setDisplay(j, displayArray[j]);
        //     control_type_t type = controlJson["pedal_type"][j].as<control_type_t>();
        //     int custom = controlJson["pedal_custom"][j].as<int>();
        //     int cc = controlJson["pedal_cc"][j].as<int>();
        //     int channel = controlJson["pedal_ch"][j].as<int>();
        //     int toggled = controlJson["pedal_toggle"][j].as<int>();
        //     pages[i].setPedal(j, type, cc, channel, custom, toggled);
        // }
    }
    return true;
}

void JSONManager::sendJSONConfig() {
    if (!doc.containsKey("options")) {
        Serial.println("Options not found in JSON document");
        return;
    }
    JsonDocument optionsDoc;
    optionsDoc["options"] = options;
    serializeJson(optionsDoc, Serial);
    Serial.println("");
}

void JSONManager::sendJSONConfigOSC() {
    if (!doc.containsKey("options")) {
        Serial.println("Options not found in JSON document");
        return;
    }
    JsonDocument optionsDoc;
    optionsDoc["options"] = options;
    char jsonBuffer[248];
    serializeJson(optionsDoc, jsonBuffer);
    sendOSCShowControl("/showcontrol/serialMessage", jsonBuffer);
}

void JSONManager::sendJSONPage(int pageNum) {
    JsonObject page = userPagesArray[pageNum];
    JsonDocument controlsDoc;
    controlsDoc["userPage"] = page;
    serializeJson(controlsDoc, Serial);
    Serial.println("");
}

void JSONManager::sendJSONPageOSC(int pageNum) {
    JsonObject page = userPagesArray[pageNum];
    JsonDocument controlsDoc;
    controlsDoc["userPage"] = page;
    sendOSCShowControl("/showcontrol/serialMessage", "page", pageNum);
    char jsonBuffer[512];
    serializeJson(controlsDoc, jsonBuffer);
    sendOSCShowControl("/showcontrol/serialMessage", jsonBuffer);
}

void JSONManager::setup() {
    Serial.println(F("Inizializing FS..."));
    if (LittleFS.begin()) {
        Serial.println(F("done."));
    } else {
        Serial.println(F("fail."));
    }
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount LittleFS");
        return;
    }
    if (!jsonManager.begin()) return;
    if (!jsonManager.loadFile()) return;

    ethernet.useDHCP = jsonManager.getUseDHCP();
    settings.nightMode = jsonManager.getNightMode();
    _main.selectedMode = jsonManager.getSelectedMode();
    settings.userPagesAmount = jsonManager.getUserPagesAmount();
    Serial.print("Selected mode: ");
    Serial.println(_main.selectedMode);
    Serial.print("User pages amount: ");
    Serial.println(settings.userPagesAmount);
    showcontrolLocalPort = jsonManager.getPort();
    settings.ledBrightness = jsonManager.getLedBrightness();
    settings.displayBrightness = jsonManager.getDisplayBrightness();
    // pedal[0] = jsonManager.getPedal(0);
    // pedal[1] = jsonManager.getPedal(1);

    IPAddress ipAdr(jsonManager.getIPNum(0), jsonManager.getIPNum(1), jsonManager.getIPNum(2), jsonManager.getIPNum(3));
    manualIP = ipAdr;
    Serial.print("stored ip: ");
    Serial.print(manualIP[0]); Serial.print(".");
    Serial.print(manualIP[1]); Serial.print(".");
    Serial.print(manualIP[2]); Serial.print(".");
    Serial.print(manualIP[3]); Serial.println();
    jsonManager.getPages();
}


// Fonctions utilitaires hors classe

int getArg(String arg){  
    int value = -1;    
    int sep = arg.indexOf('=');
    if (sep != -1) {
        value = arg.substring(sep + 1).toInt();
    }
    return value;
}

void extractIPAddress(String input) {
    int ip[4];
    int equalsIndex = input.indexOf('=');
    if (equalsIndex != -1) {
        String ipString = input.substring(equalsIndex + 1);
        int octetIndex = 0;
        int startIndex = 0;
        int dotIndex = ipString.indexOf('.');
        while (dotIndex != -1 && octetIndex < 4) {
            ip[octetIndex++] = ipString.substring(startIndex, dotIndex).toInt();
            startIndex = dotIndex + 1;
            dotIndex = ipString.indexOf('.', startIndex);
        }
        if (octetIndex < 4) {
            ip[octetIndex] = ipString.substring(startIndex).toInt();
        }
    }
    ethernet.setIp(ip);
}

