#include "../config/jsonManager.h"
#include "settingsParser.h"
#include "pages.h"
#include "utils.h"

#include "../leds/leds.h"
#include "../display/pages/globalPage.h"
#include "../display/pages/settingsPage.h"
#include "../display/pages/splashPage.h"
#include "../display/colors.h"

#include "../osc/ethernetManager.h"
#include "../osc/OSCManager.h"
#include "../midi/midi_out.h"

Settings settings;

void Settings::configureDisplay(uint8_t user_mode, uint8_t display_num, uint8_t display_type){};
void Settings::storeActivePage(uint8_t pageNum){};
void Settings::parseDisplayItem(uint8_t itemType, char* strBuf, int arg2){};

void Settings::getItStarted(){
      DEBUG_LOGLN("Starting getItStarted sequence...");
      isRunning = true;  // Marquer comme en cours d'exécution
      
      // Nettoyage sécurisé de la page active
      if (activePage->pageType != NONE) {
        activePage->clearPage();
      }
      
      DEBUG_LOGLN("Handshake response received");
      
      // Validation et sécurisation de mainParser.selectedMode
      mainParser.selectedMode = jsonManager.getSelectedMode();
      
      // Vérification de validité du mode sélectionné
      if (mainParser.selectedMode >= 6) {
        DEBUG_LOG_VALUE("Invalid selectedMode, resetting to 0: ", mainParser.selectedMode);
        mainParser.selectedMode = 0;
      }
      
      // S'assurer que les pages sont correctement initialisées
      if (pages[mainParser.selectedMode].pageType == NONE) {
        DEBUG_LOGLN("Page not initialized, setting up pages...");
        setupPages();
        delay(50);
      }
      
      // Envoi des requêtes MIDI et OSC
      midi::sendLiveUpdateRequest();
      
      // Configuration globale sécurisée
      DEBUG_LOGLN("Setting up global page...");
      globalPage.setupGlobalPage();
      DEBUG_LOGLN("GlobalPage setup completed");
      
      // Changement de page sécurisé avec délai
      DEBUG_LOG_VALUE("Attempting to change to page: ", mainParser.selectedMode);
      delay(100);  // Délai important pour stabiliser
      
      // Vérification finale avant changement de page
      if (mainParser.selectedMode < 6 && pages[mainParser.selectedMode].pageType != NONE) {
        switchActivePage(pages[mainParser.selectedMode]);
        DEBUG_LOGLN("Page change successful");
      } else {
        DEBUG_LOGLN("Page change failed - falling back to splash page");
        switchActivePage(SPLASH_PAGE);
      }
      isRunning = true; // Set the running state to true
      delay(500);  // Délai pour stabiliser après le changement de page
      osc.getValues();
      DEBUG_LOGLN("getItStarted sequence completed");
      // Marquer comme terminé seulement à la fin
      // isRunning reste à true pour indiquer que l'application est active
}

void Settings::onIPReceived(int ip[4]){
      // Validation des composants IP
      for (int i = 0; i < 4; i++) {
        if (ip[i] < 0 || ip[i] > 255) {
          DEBUG_LOG_VALUE("Invalid IP component received: ", ip[i]);
          return;
        }
      }
      ethernet.setIp(ip);
      DEBUG_LOG("Valid IP address received and applied");
}; 

void Settings::onDNSReceived(int ip[4]){
      // Validation des composants DNS
      for (int i = 0; i < 4; i++) {
        if (ip[i] < 0 || ip[i] > 255) {
          DEBUG_LOG_VALUE("Invalid DNS component received: ", ip[i]);
          return;
        }
      }
      ethernet.setDns(ip);
      DEBUG_LOG("Valid DNS address received and applied");
}; 

void Settings::onGatewayReceived(int ip[4]){
      // Validation des composants Gateway
      for (int i = 0; i < 4; i++) {
        if (ip[i] < 0 || ip[i] > 255) {
          DEBUG_LOG_VALUE("Invalid Gateway component received: ", ip[i]);
          return;
        }
      }
      ethernet.setGateway(ip);
      DEBUG_LOG("Valid Gateway address received and applied");
}; 

void Settings::onSubnetReceived(int ip[4]){
      // Validation des composants Subnet
      for (int i = 0; i < 4; i++) {
        if (ip[i] < 0 || ip[i] > 255) {
          DEBUG_LOG_VALUE("Invalid Subnet component received: ", ip[i]);
          return;
        }
      }
      ethernet.setSubnet(ip);
      DEBUG_LOG("Valid Subnet mask received and applied");
};

void Settings::onPortReceived(int32_t port){
      // Validation du port (plage valide pour les ports utilisateur)
      if (port < 1024 || port > 65535) {
        DEBUG_LOG_VALUE("Invalid port received: ", port);
        return;
      }
      ethernet.setPort(port);
      DEBUG_LOG_VALUE("Valid port received and applied: ", port);
};

void Settings::onDHCPReceived(int data){
      // Validation de la valeur DHCP (généralement 0 ou 1)
      if (data < 0 || data > 1) {
        DEBUG_LOG_VALUE("Invalid DHCP value received: ", data);
        return;
      }
      ethernet.setDHCP(data);
      DEBUG_LOG_VALUE("DHCP setting applied: ", data);
};

void Settings::onPagesAmountReceived(int data){
      // Validation du nombre de pages (limite raisonnable)
      if (data < 1 || data > 10) {
        DEBUG_LOG_VALUE("Invalid pages amount received: ", data);
        return;
      }
      userPagesAmount = data;
      DEBUG_LOG_VALUE("userPagesAmount RCV ", userPagesAmount);
      jsonManager.writeOption("userPagesAmount", userPagesAmount);
};
void Settings::onRemoveUserPage(int pageNum){
  // Validation du numéro de page
  if (pageNum < 0 || pageNum >= userPagesAmount) {
    DEBUG_LOG_VALUE("Invalid page number for removal: ", pageNum);
    return;
  }
  
  // TODO: Implémenter la logique de suppression de page utilisateur
  DEBUG_LOG_VALUE("Page removal requested for page: ", pageNum);
  // Logique de suppression à implémenter
}


void Settings::setLedBrightness(uint8_t value) {
  // Validation des paramètres d'entrée
  if (value == 0 || value > 10) {
    DEBUG_LOG_VALUE("Invalid LED brightness value: ", value);
    return;
  }
  
  ledBrightness = value;
  DEBUG_LOG_VALUE("LED brightness set to: ", ledBrightness);
  
  // Mise à jour de tous les LEDs
  for (uint8_t i = 0; i < NUM_LEDS; ++i) {
    l[i].show_color();
  }
  jsonManager.writeOption("ledBrightness", value);
}

void Settings::setDisplayBrightness(uint8_t value) {
  // Validation des paramètres d'entrée
  if (value == 0 || value > 10) {
    DEBUG_LOG_VALUE("Invalid display brightness value: ", value);
    return;
  }
  
  displayBrightness = value;
  DEBUG_LOG_VALUE("Display brightness set to: ", displayBrightness);
  
  // Application de la luminosité (multiplication par 20 pour étendre la plage)
  const uint8_t pwmValue = displayBrightness * 25;
  analogWrite(TFT_LED, pwmValue);
  jsonManager.writeOption("displayBrightness", displayBrightness);
}


void Settings::exitSettings(){
  DEBUG_LOGLN("Exiting settings and saving configuration");
  
  switchActivePage(previousActivePage);
  // Nettoyage de la page des paramètres
  // Application des nouvelles configurations réseau
  const int newPort = settingsPage.itemContent[6].intValue;
  if(newPort != showcontrolLocalPort) ethernet.setPort(newPort);
  DEBUG_LOG_VALUE("Applied new port: ", newPort);
  
  // Configuration de l'adresse IP
  const int newIP[4] = {
    settingsPage.itemContent[2].intValue, 
    settingsPage.itemContent[3].intValue, 
    settingsPage.itemContent[4].intValue, 
    settingsPage.itemContent[5].intValue
  };
  
  // Comparer avec l'IP actuelle (manualIP déjà déclaré extern dans ethernetManager.h)
  bool ipChanged = (newIP[0] != manualIP[0] || 
                    newIP[1] != manualIP[1] || 
                    newIP[2] != manualIP[2] || 
                    newIP[3] != manualIP[3]);
  
  if (ipChanged) {
    ethernet.setIp(const_cast<int*>(newIP));
    DEBUG_LOG("Applied new IP address: ");
    DEBUG_LOG(newIP[0]); DEBUG_LOG(".");
    DEBUG_LOG(newIP[1]); DEBUG_LOG(".");
    DEBUG_LOG(newIP[2]); DEBUG_LOG(".");
    DEBUG_LOGLN(newIP[3]);
  } else {
    DEBUG_LOGLN("IP address unchanged, skipping update");
  }
  
  // Restauration de la page précédente
  
  
  // TODO: Implement subnet mask configuration if needed
  // int submask[4] = {settingsPage.itemContent[7].intValue, settingsPage.itemContent[8].intValue, settingsPage.itemContent[9].intValue, settingsPage.itemContent[10].intValue};
  // ethernet.setSubnet(submask);
}

// Fonction utilitaire pour limiter une valeur dans une plage
void Settings::clampSettingValue(int& currentValue, int minVal, int maxVal) {
  currentValue = max(currentValue, minVal);
  currentValue = min(currentValue, maxVal);
}

// Fonction utilitaire pour mettre à jour un paramètre de configuration
void Settings::updateSettingValue(int selectedItem, int deltaValue, int minVal, int maxVal) {
  settingsPage.itemContent[selectedItem].intValue += deltaValue;
  clampSettingValue(settingsPage.itemContent[selectedItem].intValue, minVal, maxVal);
}

void Settings::changeSettingsValue(int value){
  const int selectedItem = settingsPage.selectedItem;
  
  switch (selectedItem) {
    case 0: // LED brightness
      updateSettingValue(selectedItem, value, 1, 10);
      setLedBrightness(settingsPage.itemContent[selectedItem].intValue);
      break;
      
    case 1: // Display brightness
      updateSettingValue(selectedItem, value, 1, 10);
      setDisplayBrightness(settingsPage.itemContent[selectedItem].intValue);
      break;
      
    case 2: // IP address components
    case 3:
    case 4:
    case 5:
      updateSettingValue(selectedItem, value, 0, 255);
      break;
      
    case 6: // Port
      updateSettingValue(selectedItem, value, 1024, 49151);
      break;
      
    default:
      DEBUG_LOG_VALUE("Unknown settings item: ", selectedItem);
      return;
  }
  
  // Mise à jour de l'affichage
  char buf[16];
  const char* displayValue = settingsPage.toConstChar(
    settingsPage.itemContent[selectedItem], buf, sizeof(buf)
  );
  settingsPage.showSettingsSprite(selectedItem, displayValue, _Emerald);
}

void Settings::validateSettings(){
  // Si on est sur le dernier élément (Save & Exit)
  if(settingsPage.selectedItem == ITEMSAMNT - 1) {
    exitSettings();
    return;
  }
  
  // Basculer le mode d'édition pour l'élément actuel
  const int selectedItem = settingsPage.selectedItem;
  char buf[16];
  const char* displayValue = settingsPage.toConstChar(
    settingsPage.itemContent[selectedItem], buf, sizeof(buf)
  );
  
  if(!settingsPage.isEditingValue) {
    // Entrer en mode édition
    settingsPage.showSettingsSprite(selectedItem, displayValue, _Emerald);
    settingsPage.isEditingValue = true;
    DEBUG_LOG_VALUE("Editing setting item: ", selectedItem);
  } else {  
    // Sortir du mode édition
    settingsPage.showSettingsSprite(selectedItem, displayValue, defaultTxtColor);
    settingsPage.isEditingValue = false;
    DEBUG_LOG_VALUE("Finished editing setting item: ", selectedItem);
  }
}

void Settings::setup(){
  userPagesAmount = jsonManager.getUserPagesAmount();
  
  // Validation et limite de sécurité
  // if (userPagesAmount < 1) {
  //   DEBUG_LOGLN("Invalid userPagesAmount from JSON, setting to default (1)");
  //   userPagesAmount = 1;
  // } else 
  if (userPagesAmount > 5) {
    DEBUG_LOGLN("userPagesAmount too high, limiting to 10");
    userPagesAmount = 5;
  }
  
  DEBUG_LOG_VALUE("Settings initialized with userPagesAmount: ", userPagesAmount);
}