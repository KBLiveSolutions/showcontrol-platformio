

#include <Arduino.h>
#include <cstdint>
#include <pico/multicore.h>  // Pour le contrôle manuel du second cœur
#include "config/consts.h"
#include "config/jsonManager.h"
#include "core/mainPageParser.h"
#include "core/settingsParser.h"
#include "core/globalParser.h"
#include "core/pages.h"
#include "core/utils.h"

// Variable partagée pour synchroniser les cores
volatile bool core0_ready = false;

#include "leds/leds.h"
#include "display/displaySetup.h"
#include "display/sprites.h"
#include "display/pages/mainPage.h"
#include "display/pages/settingsPage.h"
#include "display/pages/globalPage.h"
#include "display/pages/splashPage.h"

#include "input/buttons.h"
#include "input/encoder.h"
#include "input/expressionPedal.h"

#include "midi/midi.h"
#include "midi/midi_in.h"
#include "midi/midi_out.h"
#include "midi/midi_host.h"

#include "osc/ethernetManager.h"
#include "osc/OSCManager.h"
#include "osc/OSCReceive.h"

void setup() {
  Serial.begin(115200);
  // ATTENTION: Ne pas attendre Serial sur ESP32/embedded - cause des blocages !
  while (!Serial) ; // Attente de la connexion série
  
  // Délai de stabilisation au démarrage - CRITIQUE pour éviter les crashes
  delay(200);
  
  DEBUG_LOGLN("ShowControl setup started...");
  
  // Ordre d'initialisation critique - les dépendances doivent être respectées
  DEBUG_LOGLN("Initializing JSON manager...");
  jsonManager.setup();
  delay(100);  // Stabilisation après init JSON
  
  DEBUG_LOGLN("Initializing settings...");
  settings.setup();
  delay(100);  // Stabilisation après init settings
  
  // Initialisation hardware (doit être après settings)
  DEBUG_LOGLN("Initializing display...");
  
  // Délai important avant display pour éviter les conflits USB/SPI
  delay(500);  // Délai plus long pour stabiliser
  
  displayManager::setup();
  delay(200);  // Délai après display setup
  
  DEBUG_LOGLN("Display setup done");
  
  DEBUG_LOGLN("Initializing LEDs...");
  leds::setup();
  delay(100);
  DEBUG_LOGLN("Setup LEDs done");
  
  // Initialisation des pages (dépend de display et leds)
  DEBUG_LOGLN("Setting up pages...");
  setupPages();
  delay(100);
  
  // Affichage initial seulement après tout setup hardware
  DEBUG_LOGLN("Showing splash page...");
  splashPage.showPage();
  delay(200);
  
  // Initialisation des entrées (après display)
  DEBUG_LOGLN("Initializing inputs...");
  buttns::setup();
  encoder::setup();
  delay(100);
  
  // Initialisation MIDI et réseau en dernier
  DEBUG_LOGLN("Initializing MIDI...");
  midi::setup();
  delay(100);

  DEBUG_LOGLN("Initializing ethernet...");
  ethernet.setup();
  delay(200);  // Délai important pour l'ethernet
  
  // Marquer le système comme prêt pour Core1
  core0_ready = true;
  DEBUG_LOGLN("Core0 setup complete - signaling Core1");
  
  // Marquer le système comme prêt
  DEBUG_LOGLN("ShowControl setup complete - system ready for network events");
}


void loop() {
  buttns::read();
  encoder::read();
  ethernet.read();
  midiHost.read();
  // midi::read();
  // midihost::loopMIDIHost();
  delay(2);  // Délai court pour éviter les blocages, mais pas trop long pour la réactivité
}

void setup1() {
  midiHost.begin();
}

void loop1() {
  midiHost.task();
}