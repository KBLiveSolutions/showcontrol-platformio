

// TO DO add the pedal type setting function

void setPedalType(uint8_t num, uint8_t type);


void setPedalType(uint8_t num, uint8_t type) {
  pedal[num] = (pedal_type)type;
  if (pedal[num] == FOOTSWITCH) {
    pinMode(a_pins[num], OUTPUT);
    digitalWrite(a_pins[num], LOW);
    pinMode(b_pins[num], INPUT_PULLUP);
  } else if (pedal[num] == EXPRESSION) {
    pinMode(b_pins[num], OUTPUT);
    digitalWrite(b_pins[num], HIGH);
    pinMode(a_pins[num], INPUT);
  }
  String key_str = "pedal" + String(num);
  const char* key = key_str.c_str();
  jsonManager.writeOption(key, type);
}
