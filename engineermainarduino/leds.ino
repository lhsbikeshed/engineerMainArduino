#include "leds.h"

#define LED_PIN 13

// blinking stuff
int lastBlinkTime = 0;
int nextBlinkTime = 100;
boolean blinker = false;
int counter = 0;

void setupLEDs() {
	FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUMPIXELS);
}

void resetLEDs() {
	// Set all LEDs to black
	memset(leds, 0, NUMPIXELS * 3);
	setBlinkSpeed(100);
}

void setBlinkSpeed(int blinkSpeed) {
	nextBlinkTime = blinkSpeed;
}

void LEDBlinkThink() {
	// thingy counter
	counter ++;
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

	int ledId;
	for (i = 0; i < NUM_RINGS; i++) {
		for (ledId = 0; ledId < powerLevels[i]; ledId++) {
			if (ledId < 4) {
				ledRings[i][ledId] = RingRed;
			}
			else if (ledId < 8) {
				ledRings[i][ledId] = RingYellow;
			}
			else {
				ledRings[i][ledId] = RingGreen;
			}
		}
	}
}
