#define PIN_AIRLOCK 53
#define PIN_ENCODER_A A1
#define PIN_ENCODER_B A0

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
  // do analog - old version supplies game with 0 - 10 values.
  //int valA = 11 - map(analogRead(PIN_ENCODER_A), 0, 1024, 10, 0);
  //int valB = 11 - map(analogRead(PIN_ENCODER_B), 0, 1024, 10, 0);

  int valA = analogRead(PIN_ENCODER_A);
  int valB = analogRead(PIN_ENCODER_B);

  if (abs(valA - lastA) > 10) {
    lastA = valA;
    Serial.print("A");
    Serial.print(valA);
    Serial.print(",");
  }
  if (abs(valB - lastB) > 10) {
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
