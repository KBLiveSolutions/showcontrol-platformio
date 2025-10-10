#pragma once
#include <Arduino.h>

class Action {
  public:
    Action();
    void onLockButtonPress();
    void onLockButtonLongPress();
    void onEncoderButtonPress();
    void onEncoderButtonLongPress();
    void onButtonShortPress(uint8_t idx);
    void onButtonLongPress(uint8_t idx);
    void onEncoderTurned(int value);
    void press_button(uint8_t idx);
    void release_button(uint8_t idx);
    void onLockButtonRelease();
    void onPedalPress(uint8_t idx);
    void onLongPedalPress(uint8_t idx);
};

extern Action actions;