#define PIN_AIRLOCK 16
#define PIN_ENCODER_A 5
#define PIN_ENCODER_B 4

// analog bits
int lastA = -1;
int lastB = -1;

// airlock switch state
byte lastAirlock = 0;
long lastAirlockRead = 0;
boolean lockSent = false;

void setupAirlock() {
  pinMode(PIN_AIRLOCK, INPUT);
  digitalWrite(PIN_AIRLOCK, HIGH);
}

void readAnalog() {
  // do analog
  int valA = 11 - map(analogRead(PIN_ENCODER_A), 0, 1024, 10, 0);
  int valB = 11 - map(analogRead(PIN_ENCODER_B), 0, 1024, 10, 0);

  if (valA != lastA) {
    lastA = valA;
    Serial.print("A");
    Serial.print(valA);
    Serial.print(",");
  }
  if (valB != lastB) {
    lastB = valB;
    Serial.print("B");
    Serial.print(valB);
    Serial.print(",");
  }
}

void readAirlock() {
  byte b = digitalRead(PIN_AIRLOCK);
  if (b == lastAirlock) {
    return;
  }
  lastAirlock = b;
  if (b == 0) {
    Serial.print("L,");
  }
}
