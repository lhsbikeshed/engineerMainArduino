#define PIN_AIRLOCK 16
#define PIN_ENCODER_A 4
#define PIN_ENCODER_B 5

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
    Serial.print("B");
    Serial.print(valA);
    Serial.print(",");
  }
  if (valB != lastB) {
    lastB = valB;
    Serial.print("A");
    Serial.print(valB);
    Serial.print(",");
  }
}

void readAirlock() {
  if (lastAirlockRead + 10 < millis()) {
    lastAirlockRead = millis();
    byte b = digitalRead(PIN_AIRLOCK);
    if ((b == lastAirlock)) {
      if (b == 0) {
        if (!lockSent) {
          Serial.print("L,");
          lockSent = true;
        }
      }
      else {
        lockSent = false;
      }
    }
    lastAirlock = b;
  }
}
