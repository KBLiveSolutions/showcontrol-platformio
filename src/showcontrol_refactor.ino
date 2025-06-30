

#include <Arduino.h>
#include <cstdint>
#include "config/consts.h"
#include "config/jsonManager.h"
#include "core/mainPageParser.h"
#include "core/settingsParser.h"
#include "core/globalParser.h"
#include "core/pages.h"
#include "core/utils.h"

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

#include "osc/ethernetManager.h"
#include "osc/OSCManager.h"
#include "osc/OSCReceive.h"

void setup() {
  midi::setup();
  Serial.begin(115200);
  // while (!Serial) ; // Attente de la connexion série
  
  DEBUG_LOGLN("ShowControl setup started...");
  jsonManager.setup();
  delay(1000);
  settings.setup();
  buttns::setup();
  encoder::setup();
  displayManager::setup();
  leds::setup();
  setupPages();
  splashPage.showPage();
  ethernet.setup();
  DEBUG_LOGLN("ShowControl setup complete.");
}

void loop() {
  buttns::read();
  encoder::read();
  ethernet.read();
  midi::read();
}
