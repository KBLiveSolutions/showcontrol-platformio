#pragma once
#include <Arduino.h>
#include "config/jsonManager.h"

enum pedal_type { FOOTSWITCH, EXPRESSION, DUAL };

class Pedal {
public:
    Pedal(uint8_t num_, uint8_t a_pin_, uint8_t b_pin_);
    void begin();
    void setType(pedal_type type_);
    pedal_type getType() const;
    uint8_t num;
    uint8_t a_pin;
    uint8_t b_pin;
    pedal_type type;

private:
    // uint8_t num_;
    // uint8_t a_pin_;
    // uint8_t b_pin_;
    // pedal_type type_;
};

extern Pedal pedals[2];