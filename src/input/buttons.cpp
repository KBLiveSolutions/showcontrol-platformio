// Durée (en ms) pendant laquelle le bouton 0 doit rester LOW pour valider l'appui
#define BUTTON0_CONFIRM_MS 100
#include "buttons.h"
#include "mux.h"
#include "actions.h"
#include "../core/pages.h"

const int b_pins[NUM_BUTTONS] = {2, 30, 30, 30, 30, 30, 30, 24, 25};

const int longPressDuration = 1000; // ms, à ajuster
ButtonState buttons[NUM_BUTTONS];


void buttns::read()
{
  // Pour les 2 premiers boutons sur b_pins

  for (uint8_t p = 0; p < 1; p++)
  {
    bool reading = digitalRead(b_pins[p]);
  // ...
    updateButton(p, reading);
    if (wasShortPressed(p))
    {
      onShortButtonPress(p);
    }
    if (wasLongPressed(p))
    {
      onLongButtonPress(p);
    }
  }

  // Pour les 6 suivants sur le PCF
  for (uint8_t p = 0; p < 7; p++)
  {
    bool reading = readSensor(p);
    uint8_t idx = p + 1;
  // ...
    updateButton(idx, reading);
    if (wasShortPressed(idx))
    {
      onShortButtonPress(idx);
    }
    if (wasLongPressed(idx))
    {
      onLongButtonPress(idx);
    }
  }
}






void buttns::updateButton(uint8_t idx, bool reading)
{
  // Log de diagnostic pour traquer les appels hors limites
  if (idx >= NUM_BUTTONS) {
    return;
  }

  unsigned long now = millis();

  buttons[idx].lastState = buttons[idx].currentState;
  buttons[idx].currentState = reading;

  // Filtrage logiciel strict pour le bouton 0 : n'accepter un appui que si la pin reste LOW pendant 100 ms consécutives
  static unsigned long lowStartTime = 0;
  if (idx == 0) {
    if (!buttons[idx].currentState) { // LOW détecté
      if (lowStartTime == 0) lowStartTime = now;
      if ((now - lowStartTime) >= BUTTON0_CONFIRM_MS && buttons[idx].pressedTime == 0) {
        Serial.print("Button pressed: ");
        Serial.println(idx);
        onButtonPress(idx);
        buttons[idx].pressedTime = now;
        buttons[idx].longPressEventFired = false;
        buttons[idx].longPressHandled = false;
        buttons[idx].shortPressEventPending = false;
      }
    } else { // Repassé HIGH
      lowStartTime = 0;
      if (buttons[idx].pressedTime != 0) {
        onButtonRelease(idx);
        if (!buttons[idx].longPressHandled) {
          buttons[idx].shortPressEventPending = true;
        }
      }
      buttons[idx].pressedTime = 0;
      buttons[idx].longPressEventFired = false;
      buttons[idx].longPressHandled = false;
    }
    return;
  }

  if (!buttons[idx].currentState) // Bouton appuyé (active bas)
  {
    if (buttons[idx].pressedTime == 0)
    {
      Serial.print("Button pressed: ");
      Serial.println(idx);
      onButtonPress(idx);
      buttons[idx].pressedTime = now;
      buttons[idx].longPressEventFired = false;
      buttons[idx].longPressHandled = false;
      buttons[idx].shortPressEventPending = false;
    }
    else if (!buttons[idx].longPressHandled &&
         (now - buttons[idx].pressedTime) >= longPressDuration)
    {
      buttons[idx].longPressEventFired = true;
      buttons[idx].longPressHandled = true;
    }
  }
  else // Bouton relâché
  {
    if (buttons[idx].pressedTime != 0)
    {
      onButtonRelease(idx);
      if (!buttons[idx].longPressHandled)
      {
        buttons[idx].shortPressEventPending = true;
      }
    }
    buttons[idx].pressedTime = 0;
    buttons[idx].longPressEventFired = false;
    buttons[idx].longPressHandled = false;
  }
}

bool buttns::wasShortPressed(uint8_t idx)
{
    if (buttons[idx].shortPressEventPending)
    {
        buttons[idx].shortPressEventPending = false;
        return true;
    }
    return false;
}

bool buttns::wasLongPressed(uint8_t idx)
{
    if (buttons[idx].longPressEventFired)
    {
        buttons[idx].longPressEventFired = false;
        return true;
    }
    return false;
}

void buttns::onShortButtonPress(uint8_t p)
{
  Serial.print("Short button pressed: ");
  Serial.println(p);
  if (p >= NUM_BUTTONS) return;
  if (p == 6) onEncoderButtonPress();
  // else if (p > 0) onButtonShortPress(p - 1); // p-1 pour les boutons PCF, mais jamais pour p==0
}

void buttns::onLongButtonPress(uint8_t p)
{
  if (p >= NUM_BUTTONS) return;
  // if (p == 0) onLockButtonLongPress();
  if (p == 6) onEncoderButtonLongPress();
  else onButtonLongPress(p - 1); // p-1 pour les boutons PCF
}

void buttns::onButtonPress(uint8_t idx)
{
  if (idx >= NUM_BUTTONS) return;
  press_button(idx);
}

void buttns::onButtonRelease(uint8_t idx)
{
  if (idx >= NUM_BUTTONS) return;
  release_button(idx);
}

void buttns::setup()
{
  for (uint8_t p = 0; p < 1; p++)
  {
    pinMode(b_pins[p], INPUT_PULLUP);
  }
  setupMUX();
}
