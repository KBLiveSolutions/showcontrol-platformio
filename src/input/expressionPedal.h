

#pragma once
#include <Arduino.h>
#include "../config/consts.h"

extern const int a_pins[NUM_SLIDERS];
extern bool pedal_state[NUM_SLIDERS];

class AnalogPot {
private:
    int number;
    int pin;
    static const int RunningAverageCount = 30;
    float RunningAverageBuffer[RunningAverageCount];
    int NextRunningAverage;
    int other_pedal;

public:
    AnalogPot(int num);
    int value_MIDI = 0;
    uint8_t lastValue = 0;
    int pedal_min = 0;
    int pedal_max = 255;
    int lastReading = 0;
    int lastMIDIValue = 0;
    int margin = 4;
    int _value = 0;
    bool slider_state = LOW;
    unsigned long lastDebounceTime = 10;
    unsigned long slider_on_time = 0;
    unsigned long debounceDelayAnalog = 50;

    int analog_to_MIDI(int analog_in);
    void check_pot();
    void process_analog(int value_MIDI);
    void _calibrate(int num, int min_or_max);
    int average(int value);
};

extern AnalogPot a[NUM_SLIDERS];

void setupPedals();