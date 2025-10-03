#include "expressionPedal.h"
#include "../core/utils.h"


// ANALOG
const int a_pins[NUM_SLIDERS] = { 28, 29 };
bool pedal_state[NUM_SLIDERS] = { LOW, LOW };

AnalogPot::AnalogPot(int num) {
    number = num;
    pin = a_pins[number];
    if (number == 0) other_pedal = 1;
    else other_pedal = 0;
    NextRunningAverage = 0;
    for (int i = 0; i < RunningAverageCount; ++i) RunningAverageBuffer[i] = 0;
}

int AnalogPot::analog_to_MIDI(int analog_in) {
    int _val = constrain(map(analog_in, pedal_min, pedal_max, 0, 128), 0, 127);
    return _val;
}

void AnalogPot::check_pot() {
    int reading = average(analogRead(pin));
    if ((millis() - lastDebounceTime) > debounceDelayAnalog) {
        if (reading > lastValue + margin || reading < lastValue - margin) {
            lastDebounceTime = millis();
            if (!slider_state) {
                if (abs(analog_to_MIDI(lastValue) - _value) < 5) {
                    slider_state = HIGH;
                    slider_on_time = millis();
                }
            }
            lastValue = reading;
        }
    }
    if (reading > lastReading + margin || reading < lastReading - margin) {
        DEBUG_LOG("Pedal ");
        DEBUG_LOG(number);
        DEBUG_LOG("  ");
        DEBUG_LOGLN(reading);
        // activePage->expressionPedal(number, int(reading/2) );
        lastReading = reading;
    }
}

void AnalogPot::process_analog(int value_MIDI) {
    if ((lastMIDIValue != value_MIDI)) {
        lastMIDIValue = value_MIDI;
        DEBUG_LOGLN(value_MIDI);
        // USB_MIDI.sendControlChange(control[current_layout], value_MIDI, channel[current_layout]);
    }
}

void AnalogPot::_calibrate(int num, int min_or_max) {
    int value = analogRead(pin) / 16;
    value = constrain(value, 0, 255);
    if (min_or_max == 0) {
        pedal_min = value;
    } else {
        pedal_max = value;
    }
}

int AnalogPot::average(int value) {
    float RawValue = value;
    RunningAverageBuffer[NextRunningAverage++] = RawValue;
    if (NextRunningAverage >= RunningAverageCount) {
        NextRunningAverage = 0;
    }
    float RunningAverageValue = 0;
    for (int i = 0; i < RunningAverageCount; ++i) {
        RunningAverageValue += RunningAverageBuffer[i];
    }
    RunningAverageValue /= RunningAverageCount;
    return RunningAverageValue;
}

AnalogPot a[NUM_SLIDERS] = { AnalogPot(0), AnalogPot(1) };

void setupPedals() {
    // setPedalType(0, pedal[0]);
    // setPedalType(1, pedal[1]);
}