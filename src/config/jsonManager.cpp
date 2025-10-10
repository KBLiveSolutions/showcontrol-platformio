#include "jsonManager.h"
#include "../osc/OSCManager.h"
#include "../core/utils.h"
#include <Arduino.h>

JSONManager jsonManager("/data.json");

JSONManager::JSONManager(const char* fname) : filename(fname) {
    options = doc["options"];
    pagesArray = doc["pages"];
}

bool JSONManager::begin() {
    DEBUG_LOGLN("[JSONManager] Initializing LittleFS...");
    if (!LittleFS.begin()) {
        DEBUG_LOGLN("[JSONManager] ERROR: Failed to mount LittleFS");
        return false;
    }
    DEBUG_LOGLN("[JSONManager] LittleFS mounted successfully");
    return true;
}

bool JSONManager::loadFile() {
    DEBUG_LOG("[JSONManager] Loading file: ");
    DEBUG_LOGLN(filename);
    
    File file = LittleFS.open(filename, "r");
    if (!file) {
        DEBUG_LOGLN("[JSONManager] ERROR: Failed to open file for reading");
        return false;
    }
    
    size_t fileSize = file.size();
    DEBUG_LOG("[JSONManager] File size: ");
    DEBUG_LOG(fileSize);
    DEBUG_LOGLN(" bytes");
    
    if (fileSize == 0) {
        DEBUG_LOGLN("[JSONManager] WARNING: File is empty");
        file.close();
        return false;
    }
    
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    
    if (error) {
        DEBUG_LOG("[JSONManager] ERROR: deserializeJson() failed: ");
        DEBUG_LOGLN(error.f_str());
        DEBUG_LOG("[JSONManager] Error code: ");
        DEBUG_LOGLN(error.code());
        return false;
    }
    
    DEBUG_LOGLN("[JSONManager] JSON deserialized successfully");
    DEBUG_LOG("[JSONManager] Document size: ");
    DEBUG_LOGLN(doc.size());
    
    options = doc["options"];
    pagesArray = doc["pages"];
    
    DEBUG_LOG("[JSONManager] Options found: ");
    DEBUG_LOGLN(options.size());
    DEBUG_LOG("[JSONManager] User pages found: ");
    DEBUG_LOGLN(pagesArray.size());
    
    return true;
}

bool JSONManager::saveFile() {
    DEBUG_LOG("[JSONManager] Saving file: ");
    DEBUG_LOGLN(filename);
    
    File file = LittleFS.open(filename, "w");
    if (!file) {
        DEBUG_LOGLN("[JSONManager] ERROR: Failed to open file for writing");
        return false;
    }
    
    size_t bytesWritten = serializeJson(doc, file);
    file.close();
    
    if (bytesWritten == 0) {
        DEBUG_LOGLN("[JSONManager] ERROR: Failed to write to file");
        return false;
    }
    
    DEBUG_LOG("[JSONManager] File saved successfully, ");
    DEBUG_LOG(bytesWritten);
    DEBUG_LOGLN(" bytes written");
    return true;
}

bool JSONManager::writeJSONControl(int user_mode, int controlNum, int type, int cc, int ch, int isCustom, int toggled, const char* actionName, int color, int luminance) {
    
    if (!loadFile()) {
        DEBUG_LOGLN("[JSONManager] ERROR: Failed to load file before writing control");
        return false;
    }
    
    JsonObject userPage = pagesArray[user_mode];
    if (userPage.isNull()) {
        DEBUG_LOG("[JSONManager] ERROR: User page ");
        DEBUG_LOG(user_mode);
        DEBUG_LOGLN(" not found");
        return false;
    }
    
    userPage["custom"][controlNum] = isCustom;
    userPage["type"][controlNum] = type;
    userPage["cc"][controlNum] = cc;
    userPage["ch"][controlNum] = ch;
    userPage["toggle"][controlNum] = toggled;
    userPage["led_cc"][controlNum] = cc;
    userPage["led_ch"][controlNum] = ch;
    userPage["actionNames"][controlNum] = String(actionName ? actionName : "");
    userPage["led_color"][controlNum] = color;
    userPage["luminance"][controlNum] = luminance;
    
    bool result = saveFile();
    if (result) {
        DEBUG_LOGLN("[JSONManager] Control written successfully");
    } else {
        DEBUG_LOGLN("[JSONManager] ERROR: Failed to save control");
    }
    return result;
}

// bool JSONManager::writeJSONPedal(int user_mode, int controlNum, int type, int cc, int ch, int isCustom, int toggled) {
//     if (!loadFile()) return false;
//     JsonObject userPage = pagesArray[user_mode];
//     userPage["pedal_custom"][controlNum] = isCustom;
//     userPage["pedal_type"][controlNum] = type;
//     userPage["pedal_cc"][controlNum] = cc;
//     userPage["pedal_ch"][controlNum] = ch;
//     userPage["pedal_toggle"][controlNum] = toggled;
//     return saveFile();
// }

bool JSONManager::writeJSONDisplay(int user_mode, int display_num, int display_type) {
    if (!loadFile()) return false;
    JsonObject userPage = pagesArray[user_mode];
    userPage["display"][display_num] = display_type;
    return saveFile();
}

bool JSONManager::writeOption(const char* key, int value) {
    DEBUG_LOG("[JSONManager] Writing option - Key:'");
    DEBUG_LOG(key);
    DEBUG_LOG("', Value:");
    DEBUG_LOGLN(value);
    
    if (!loadFile()) {
        DEBUG_LOGLN("[JSONManager] ERROR: Failed to load file before writing option");
        return false;
    }
    
    options[key] = value;
    
    bool result = saveFile();
    if (result) {
        DEBUG_LOGLN("[JSONManager] Option written successfully");
    } else {
        DEBUG_LOGLN("[JSONManager] ERROR: Failed to save option");
    }
    return result;
}

bool JSONManager::writeManualIP(int* ip) {
    if (!loadFile()) {
        DEBUG_LOGLN("[JSONManager] Failed to load JSON file");
        return false;
    }
    for(int i=0; i<4; i++){
        options["manualIP"][i] = ip[i];
    }
    DEBUG_LOG("[JSONManager] Manual IP set to: ");
    DEBUG_LOG(ip[0]); DEBUG_LOG(".");
    DEBUG_LOG(ip[1]); DEBUG_LOG(".");
    DEBUG_LOG(ip[2]); DEBUG_LOG(".");
    DEBUG_LOGLN(ip[3]);
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
control_type_t JSONManager::getControlType(int page, int control) const {
    return pagesArray[page]["type"][control].as<control_type_t>();
}
int JSONManager::getControlCC(int page, int control) const {
    return pagesArray[page]["cc"][control].as<int>();
}
int JSONManager::getControlChannel(int page, int control) const {
    return pagesArray[page]["ch"][control].as<int>();
}
int JSONManager::getControlToggle(int page, int control) const {
    return pagesArray[page]["toggle"][control].as<int>();
}
int JSONManager::getControlLedCC(int page, int control) const {
    return pagesArray[page]["led_cc"][control].as<int>();
}
int JSONManager::getControlLedChannel(int page, int control) const {
    return pagesArray[page]["led_ch"][control].as<int>();
}
int JSONManager::getControlCustom(int page, int control) const {
    return pagesArray[page]["custom"][control].as<int>();
}   
const char* JSONManager::getActionName(int page, int control, char* buffer, size_t bufferSize) const {
    const char* name = pagesArray[page]["actionNames"][control].as<const char*>();
    if (name) {
        strncpy(buffer, name, bufferSize - 1);
        buffer[bufferSize - 1] = '\0';
    } else {
        buffer[0] = '\0';
    }
    return buffer;
}
int JSONManager::getLedColorIndex(int page, int control) const {
    return pagesArray[page]["led_color"][control].as<int>();
}
bool JSONManager::getLuminance(int page, int control) const {
    return pagesArray[page]["luminance"][control].as<bool>();
}
displayed_item_t JSONManager::getDisplayType(int page, int displayNum) const {
    return pagesArray[page]["display"][displayNum].as<displayed_item_t>();
}

// bool JSONManager::getPages() {
//     DEBUG_LOGLN("[JSONManager] Loading pages from JSON...");
    
//     for (size_t i = 0; i <= MAX_NUM_USERS; ++i) {
//         JsonObject controlJson = pagesArray[i];
//         if (controlJson.isNull()) {
//             DEBUG_LOG("[JSONManager] WARNING: Page ");
//             DEBUG_LOG(i);
//             DEBUG_LOGLN(" not found, skipping");
//             continue;
//         }
        
//         DEBUG_LOG("[JSONManager] Processing page ");
//         DEBUG_LOGLN(i);
        
//         for (size_t j = 0; j < NUM_CONTROLS; ++j) {
//             control_type_t type = controlJson["type"][j].as<control_type_t>();
//             int custom = controlJson["custom"][j].as<int>();
//             int cc = controlJson["cc"][j].as<int>();
//             int channel = controlJson["ch"][j].as<int>();
//             int toggled = controlJson["toggle"][j].as<int>();
//             DEBUG_LOG("[JSONManager] Control ");
//             DEBUG_LOG_CONT(j);   
//             DEBUG_LOG_CONT(" - channel: ");
//             DEBUG_LOG_CONT(channel);
//             DEBUG_LOG_CONT(", cc: ");
//             DEBUG_LOG_CONT_LN(cc); 
//             // pages[i+1].setButtonControl(j, type, cc, channel, custom, toggled);
            
//             cc = controlJson["led_cc"][j].as<int>();
//             channel = controlJson["led_ch"][j].as<int>();
//             pages[i].setLedControl(j, type, cc, channel);
//         }
        
//         JsonArray displayArray = controlJson["display"];
//         DEBUG_LOG("[JSONManager] Page ");
//         DEBUG_LOG(i);
//         DEBUG_LOG(" loaded with ");
//         DEBUG_LOG(displayArray.size());
//         DEBUG_LOGLN(" display elements");
//         for (size_t j = 0; j < displayArray.size() && j < 3; ++j) {
//             int dispType = displayArray[j].as<int>();
//             pages[i].setDisplay(j, dispType);
//             DEBUG_LOG("[JSONManager] Display ");
//             DEBUG_LOG(j);
//             DEBUG_LOG(" set to type ");
//             DEBUG_LOGLN(dispType);
//         }
//     }
    
//     DEBUG_LOGLN("[JSONManager] All pages loaded successfully");
//     return true;
// }

void JSONManager::sendJSONConfig() {
    if (!doc["options"].is<JsonObject>()) {
        DEBUG_LOGLN("[JSONManager] Options not found in JSON document");
        return;
    }
    JsonDocument optionsDoc;
    optionsDoc["options"] = options;
    serializeJson(optionsDoc, Serial);
    Serial.println("");
}

void JSONManager::sendJSONConfigOSC() {
    if (!doc["options"].is<JsonObject>()) {
        DEBUG_LOGLN("[JSONManager] Options not found in JSON document");
        return;
    }
    JsonDocument optionsDoc;
    optionsDoc["options"] = options;
    char jsonBuffer[248];
    serializeJson(optionsDoc, jsonBuffer);
    osc.sendOSC("/showcontrol/serialMessage", jsonBuffer);
}

void JSONManager::sendJSONPage(int pageNum) {
    JsonObject page = pagesArray[pageNum];
    JsonDocument controlsDoc;
    controlsDoc["userPage"] = page;
    serializeJson(controlsDoc, Serial);
    Serial.println("");
}

void JSONManager::sendJSONPageOSC(int pageNum) {
    JsonObject page = pagesArray[pageNum];
    JsonDocument controlsDoc;
    controlsDoc["userPage"] = page;
    osc.sendOSC("/showcontrol/serialMessage", "page", pageNum);
    char jsonBuffer[512];
    serializeJson(controlsDoc, jsonBuffer);
    osc.sendOSC("/showcontrol/serialMessage", jsonBuffer);
}

void JSONManager::setup() {
    DEBUG_LOGLN("[JSONManager] === JSONManager Setup Starting ===");
    DEBUG_LOGLN("[JSONManager] Initializing filesystem...");
    
    if (LittleFS.begin()) {
        DEBUG_LOGLN("[JSONManager] Filesystem initialized successfully");
    } else {
        DEBUG_LOGLN("[JSONManager] ERROR: Filesystem initialization failed");
        return;
    }
    
    if (!LittleFS.begin()) {
        DEBUG_LOGLN("[JSONManager] ERROR: Failed to mount LittleFS on second attempt");
        return;
    }
    
    // Afficher les fichiers présents
    debugListFiles();
    
    DEBUG_LOGLN("[JSONManager] Beginning JSONManager...");
    if (!jsonManager.begin()) {
        DEBUG_LOGLN("[JSONManager] ERROR: JSONManager begin() failed");
        return;
    }
    
    DEBUG_LOGLN("[JSONManager] Loading configuration file...");
    if (!jsonManager.loadFile()) {
        DEBUG_LOGLN("[JSONManager] ERROR: Failed to load configuration file");
        // Afficher le contenu brut du fichier pour diagnostic
        debugPrintJSON();
        return;
    }
    
    DEBUG_LOGLN("[JSONManager] Reading configuration values...");
    ethernet.useDHCP = jsonManager.getUseDHCP();
    DEBUG_LOG("[JSONManager] DHCP enabled: ");
    DEBUG_LOGLN(ethernet.useDHCP ? "true" : "false");
    
    settings.nightMode = jsonManager.getNightMode();
    DEBUG_LOG("[JSONManager] Night mode: ");
    DEBUG_LOGLN(settings.nightMode ? "true" : "false");
    
    mainParser.selectedMode = jsonManager.getSelectedMode();
    DEBUG_LOG("[JSONManager] Selected mode: ");
    DEBUG_LOGLN(mainParser.selectedMode);
    
    settings.userPagesAmount = jsonManager.getUserPagesAmount();
    DEBUG_LOG("[JSONManager] User pages amount: ");
    DEBUG_LOGLN(settings.userPagesAmount);
    
    showcontrolLocalPort = jsonManager.getPort();
    DEBUG_LOG("[JSONManager] OSC Port: ");
    DEBUG_LOGLN(showcontrolLocalPort);
    
    settings.ledBrightness = jsonManager.getLedBrightness();
    DEBUG_LOG("[JSONManager] LED brightness: ");
    DEBUG_LOGLN(settings.ledBrightness);
    
    settings.displayBrightness = jsonManager.getDisplayBrightness();
    DEBUG_LOG("[JSONManager] Display brightness: ");
    DEBUG_LOGLN(settings.displayBrightness);
    
    IPAddress ipAdr(jsonManager.getIPNum(0), jsonManager.getIPNum(1), jsonManager.getIPNum(2), jsonManager.getIPNum(3));
    manualIP = ipAdr;
    DEBUG_LOG("[JSONManager] Manual IP: ");
    DEBUG_LOG(manualIP[0]); DEBUG_LOG(".");
    DEBUG_LOG(manualIP[1]); DEBUG_LOG(".");
    DEBUG_LOG(manualIP[2]); DEBUG_LOG(".");
    DEBUG_LOGLN(manualIP[3]);
    
    DEBUG_LOGLN("[JSONManager] Loading user pages...");
    // jsonManager.getPages();
    
    DEBUG_LOGLN("[JSONManager] === JSONManager Setup Complete ===");
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

// Fonction de débogage pour lister les fichiers
void JSONManager::debugListFiles() {
    DEBUG_LOGLN("[JSONManager] === LittleFS File List ===");
    
    Dir root = LittleFS.openDir("/");
    int fileCount = 0;
    size_t totalSize = 0;
    
    while (root.next()) {
        File file = root.openFile("r");
        if (file) {
            size_t size = file.size();
            DEBUG_LOG("[JSONManager] FILE: ");
            DEBUG_LOG(root.fileName().c_str());
            DEBUG_LOG(" (");
            DEBUG_LOG(size);
            DEBUG_LOGLN(" bytes)");
            totalSize += size;
            fileCount++;
            file.close();
        }
    }
    
    // Informations basiques
    DEBUG_LOG("[JSONManager] Total files: ");
    DEBUG_LOGLN(fileCount);
    DEBUG_LOG("[JSONManager] Total file size: ");
    DEBUG_LOG(totalSize);
    DEBUG_LOGLN(" bytes");
    DEBUG_LOGLN("[JSONManager] === End File List ===");
}

// Fonction de débogage pour afficher le contenu brut du JSON
void JSONManager::debugPrintJSON() {
    DEBUG_LOGLN("[JSONManager] === Raw JSON Content ===");
    
    File file = LittleFS.open(filename, "r");
    if (!file) {
        DEBUG_LOGLN("[JSONManager] ERROR: Cannot open JSON file for debug reading");
        return;
    }
    
    DEBUG_LOGLN("[JSONManager] JSON File Content:");
    while (file.available()) {
        DEBUG_LOG((char)file.read());
    }
    file.close();
    
    DEBUG_LOGLN("\n[JSONManager] === End Raw JSON ===");
}

