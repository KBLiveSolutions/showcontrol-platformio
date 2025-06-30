#define DIn 14
#define S0 11
#define S1 10
#define S2 12

bool readSensor (uint8_t which)  {
  digitalWrite (S0, (which & 1) ? HIGH : LOW);  // low-order bit
  digitalWrite (S1, (which & 2) ? HIGH : LOW);
  digitalWrite (S2, (which & 4) ? HIGH : LOW);  // high-order bit
  delayMicroseconds(2);
  // delay(1);
  return digitalRead(DIn);
  }

void setupMUX() {
  pinMode(DIn, INPUT_PULLUP);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  Serial.println("Setup MUX done");
}