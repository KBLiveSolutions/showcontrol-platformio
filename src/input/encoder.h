
#pragma once
#include <Arduino.h>
#include <RotaryEncoder.h>

extern int rotA_pin;
extern int rotB_pin;

class Rotary {
private:
    RotaryEncoder encoder;
    uint8_t number;
    int pin_A;
    int pin_B;
    int pos = 0;

public:
    Rotary(int num);
    int _value = 64;
    int _value_hold = 64;
    bool enc_state = LOW;
    void update_rotary();
};

extern Rotary r;
namespace encoder{
    void setup();
    void read();
}