#include "buttons.h"
#include "mux.h"
#include "actions.h"

const int b_pins[NUM_BUTTONS] = {2, 30, 30, 30, 30, 30, 30, 24, 25};

const int longPressDuration = 1000; // ms, à ajuster
ButtonState buttons[NUM_BUTTONS];


void buttns::read()
{
  // Pour les 2 premiers boutons sur b_pins
  for (uint8_t p = 0; p < 1; p++)
  {
    bool reading = digitalRead(b_pins[p]);
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
    buttons[idx].lastState = buttons[idx].currentState;
    buttons[idx].currentState = reading;
    unsigned long now = millis();

    if (!buttons[idx].currentState) // Bouton appuyé (active bas)
    {
        if (buttons[idx].pressedTime == 0)
        {
            onButtonPress(idx); // <-- Appel de la fonction de pression de 
            // Vient d'être pressé
            buttons[idx].pressedTime = now;
            buttons[idx].longPressEventFired = false;
            buttons[idx].longPressHandled = false; // <-- Ajouté
            buttons[idx].shortPressEventPending = false;
        }
        else if (!buttons[idx].longPressHandled &&
                 (now - buttons[idx].pressedTime) >= longPressDuration)
        {
            // Long press à déclencher une seule fois
            buttons[idx].longPressEventFired = true;
            buttons[idx].longPressHandled = true; // <-- Ajouté
        }
    }
    else // Bouton relâché
    {
        if (buttons[idx].pressedTime != 0)
        {
            onButtonRelease(idx); // <-- Appel de la fonction de relâchement
            if (!buttons[idx].longPressHandled) // <-- Vérifie ce flag
            {
                // Relâché avant longPressDuration → short press
                buttons[idx].shortPressEventPending = true;
            }
            // Sinon, c'était un long press, rien à faire ici
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
  if (p == 0) onLockButtonPress();
  else if (p == 6) onEncoderButtonPress();
  else onButtonShortPress(p - 1); // p-1 pour les boutons PCF
}

void buttns::onLongButtonPress(uint8_t p)
{
  if (p == 0) onLockButtonLongPress();
  else if (p == 6) onEncoderButtonLongPress();
  else onButtonLongPress(p - 1); // p-1 pour les boutons PCF
}

void buttns::onButtonPress(uint8_t idx)
{
  press_button(idx);
}

void buttns::onButtonRelease(uint8_t idx)
{
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
