#include "leds.h"

#define LED_PIN 48

// blinking stuff
int lastBlinkTime = 0;
int nextBlinkTime = 100;
boolean blinker = false;
int counter = 0;

void setupLEDs() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUMPIXELS);
  resetLEDs();
}

void clearLEDs() {
  // Set all LEDs to black
  memset(leds, 0, sizeof(leds));
}

void resetLEDs() {
  clearLEDs();
  setBlinkSpeed(100);
  ringLightState = false;
}

void setBlinkSpeed(int blinkSpeed) {
  nextBlinkTime = blinkSpeed;
}

void LEDBlinkThink() {
  // thingy counter
  counter++;
  // blinker
  if (lastBlinkTime + nextBlinkTime < counter) {
    lastBlinkTime = counter;
    blinker = !blinker;
  }
}

void showLEDs() {
  FastSPI_LED.show();
}

void updatePowerRings() {
  int i, j;
  // Reset all LEDs to black
  // TODO: Potential subtle bug: Previous iteration of this also set reactor led to 0.
  for (i = 0; i < NUM_RINGS; i++) {
    for (j = 0; j < PIXELS_PER_RING; j++) {
      ledRings[i][j] = CRGB::Black;
    }
  }

  if (!ringLightState) {
    return;
  }

  int ledId, powerLevel, colour;
  float pct;
  CRGB ringColour;
  for (i = 0; i < NUM_RINGS; i++) {
    powerLevel = powerLevels[i];
    if (powerLevel < 4 && !blinker) {
      continue;
    }
    if (powerLevel == 0) {
      ringColour = RingRed;
      // Ensure the entire ring lights
      powerLevel = PIXELS_PER_RING;
    } else {
      pct = (float)powerLevel / (float)PIXELS_PER_RING;
      colour = floor(pct * 96);
      ringColour = CHSV(colour, 255, 100);
    }
    for (ledId = 0; ledId < powerLevel; ledId++) {
      ledRings[i][ledId] = ringColour;
    }
  }
}
