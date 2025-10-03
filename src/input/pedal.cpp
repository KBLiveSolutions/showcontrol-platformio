#include "pedal.h"


Pedal pedals[] = {
    Pedal(0, 28, 24),
    Pedal(1, 29, 25)
};

Pedal::Pedal(uint8_t num_, uint8_t a_pin_, uint8_t b_pin_)
    : num(num_), a_pin(a_pin_), b_pin(b_pin_), type(DUAL) {}

void Pedal::setType(pedal_type type_) {
    type = type_;
    if (type_ == FOOTSWITCH) {
        pinMode(a_pin, OUTPUT);
        digitalWrite(a_pin, LOW);
        pinMode(b_pin, INPUT_PULLUP);
    } 
    else if (type == EXPRESSION) {
        pinMode(b_pin, OUTPUT);
        digitalWrite(b_pin, HIGH);
        pinMode(a_pin, INPUT);
    }
    else if (type == DUAL) {
        pinMode(a_pin, INPUT_PULLUP);
        pinMode(b_pin, INPUT_PULLUP);
    }
    String key_str = "pedal" + String(num);
    const char* key = key_str.c_str();
    jsonManager.writeOption(key, type);
}

void Pedal::begin() {
    // Initialisation par défaut en mode FOOTSWITCH
    setType(DUAL);
}

pedal_type Pedal::getType() const {
    return type;
}