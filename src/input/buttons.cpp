// Durée (en ms) pendant laquelle le bouton 0 doit rester LOW pour valider l'appui
#define BUTTON0_CONFIRM_MS 100
#include "buttons.h"
#include "mux.h"
#include "actions.h"
#include "pedal.h"
#include "../core/pages.h"
#include "../osc/OSCReceive.h"

const int b_pins[NUM_BUTTONS] = {2, 30, 30, 30, 30, 30, 30, 24, 25};

const int longPressDuration = 1000; // ms, à ajuster
ButtonState b[NUM_BUTTONS];


void Buttons::read()
{
  
  updateButton0();

  // Pour les 6 suivants sur le PCF
  for (uint8_t p = 0; p < 7; p++)
  {
    bool reading = readSensor(p);
    uint8_t idx = p + 1;
    updateButton(idx, reading);
    if (wasShortPressed(idx)) onShortButtonPress(idx);
    if (wasLongPressed(idx)) onLongButtonPress(idx);
  }
  for (uint8_t i = 0; i < 2; i++)
  {
    if (pedals[i].getType() == FOOTSWITCH || pedals[i].getType() == DUAL) {
      bool reading = digitalRead(pedals[i].b_pin);
      uint8_t idx = i + 8; // Pedals start at index 8
      updateButton(idx, reading);
      if (wasShortPressed(idx)) actions.onPedalPress(i);
      if (wasLongPressed(idx)) actions.onLongPedalPress(i);
    }    
    if (pedals[i].getType() == DUAL) {
      bool reading = digitalRead(pedals[i].a_pin);
      uint8_t idx = i + 10; // Pedals start at index 8
      updateButton(idx, reading);
      if (wasShortPressed(idx)) actions.onPedalPress(i + 2);
      if (wasLongPressed(idx)) actions.onLongPedalPress(i + 2);
    }
  }
}




void Buttons::updateButton0()
{    
    static unsigned long lastDebounceTime = 0;
    static bool lastStableState = true; // true = relâché (pull-up)
    const unsigned long debounceDelay = 20; // ms

    bool reading = digitalRead(b_pins[0]);

    if (reading != lastStableState) {
        lastDebounceTime = millis();
        lastStableState = reading;
    }

    // Si l'état est stable depuis debounceDelay ms, on valide le changement
    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != b[0].currentState) {
            if (millis() - lastOscTime < 10) {
                DEBUG_LOGLN("Bouton 0 ignoré (protection post-OSC)");
                return;
            }
            if (reading) actions.onLockButtonPress();
            else actions.onLockButtonRelease();
            b[0].currentState = reading;
        }
    }
}

void Buttons::updateButton(uint8_t idx, bool reading)
{
  // Log de diagnostic pour traquer les appels hors limites
  if (idx >= NUM_BUTTONS) {
    return;
  }

  unsigned long now = millis();

  b[idx].lastState = b[idx].currentState;
  b[idx].currentState = reading;
  static unsigned long lowStartTime = 0;

  if (!b[idx].currentState) // Bouton appuyé (active bas)
  {
    if (b[idx].pressedTime == 0)
    {
      actions.press_button(idx);
      b[idx].pressedTime = now;
      b[idx].longPressEventFired = false;
      b[idx].longPressHandled = false;
      b[idx].shortPressEventPending = false;
    }
    else if (!b[idx].longPressHandled &&
         (now - b[idx].pressedTime) >= longPressDuration)
    {
      b[idx].longPressEventFired = true;
      b[idx].longPressHandled = true;
    }
  }
  else // Bouton relâché
  {
    if (b[idx].pressedTime != 0)
    {
      actions.release_button(idx);
      if (!b[idx].longPressHandled)
      {
        b[idx].shortPressEventPending = true;
      }
    }
    b[idx].pressedTime = 0;
    b[idx].longPressEventFired = false;
    b[idx].longPressHandled = false;
  }
}

bool Buttons::wasShortPressed(uint8_t idx)
{
    if (b[idx].shortPressEventPending)
    {
        b[idx].shortPressEventPending = false;
        return true;
    }
    return false;
}

bool Buttons::wasLongPressed(uint8_t idx)
{
    if (b[idx].longPressEventFired)
    {
        b[idx].longPressEventFired = false;
        return true;
    }
    return false;
}

void Buttons::onShortButtonPress(uint8_t p)
{
  if (p >= NUM_BUTTONS) return;
  if (p == 6) actions.onEncoderButtonPress();
  else if (p > 0) actions.onButtonShortPress(p - 1); // p-1 pour les boutons PCF, mais jamais pour p==0
}

void Buttons::onLongButtonPress(uint8_t p)
{
  if (p >= NUM_BUTTONS) return;
  // if (p == 0) onLockButtonLongPress();
  if (p == 6) actions.onEncoderButtonLongPress();
  else actions.onButtonLongPress(p - 1); // p-1 pour les boutons PCF
}
long unsigned int lastPressTime = 0;

void Buttons::setup()
{
  for (uint8_t p = 0; p < 1; p++)
  {
    pinMode(b_pins[p], INPUT_PULLUP);
  }
  setupMUX();
}

Buttons buttons;