
#include "encoder.h"
#include "actions.h"
#include "../core/utils.h"

// Pins
int rotA_pin = 22;
int rotB_pin = 23;

// Rotary Encoder
Rotary::Rotary(int num)
    : encoder(rotA_pin, rotB_pin, RotaryEncoder::LatchMode::TWO03) {
    number = num;
}

void Rotary::update_rotary() {
    encoder.tick();
    int newPos = encoder.getPosition();
    if (pos != newPos) {
        enc_state = HIGH;
        int value = (pos < newPos) ? -1 : 1;
        pos = newPos;
        onEncoderTurned(value);
    }
}

Rotary r(0);

void encoder::setup() {
    pinMode(rotA_pin, INPUT_PULLUP);
    pinMode(rotB_pin, INPUT_PULLUP);
    r = Rotary(0);
    DEBUG_LOGLN("Encoder setup done");
}

void encoder::read() {
    r.update_rotary();
    if (r.enc_state) {
        r.enc_state = LOW;
    }
}