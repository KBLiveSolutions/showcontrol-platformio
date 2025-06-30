
#include <Arduino.h>
#include "../config/consts.h"

extern const int b_pins[NUM_BUTTONS];

extern const int longPressDuration;

struct ButtonState {
  bool lastState = true; // true = relâché (pull-up)
  bool currentState = true;
  unsigned long pressedTime = 0;
  bool longPressEventFired = false;
  bool longPressDetected = false;
  bool shortPressEventPending = false;
  bool longPressHasBeenFired = false;
  bool longPressHandled;
};

extern ButtonState buttons[NUM_BUTTONS];
namespace buttns{
  void setup();
  void read();
  void onButtonPress(uint8_t);
  void onButtonRelease(uint8_t idx);
  void updateButton(uint8_t idx, bool reading);

  bool wasShortPressed(uint8_t idx);
  bool wasLongPressed(uint8_t idx);
  void onShortButtonPress(uint8_t idx);
  void onLongButtonPress(uint8_t idx);
  void onShortButtonPress(uint8_t p);
}
