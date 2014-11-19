#pragma once

#define NUM_PANEL_LEDS  5
#define PIXELS_PER_RING 12
#define NUM_RINGS       4
#define NUMPIXELS       NUM_PANEL_LEDS + PIXELS_PER_RING * NUM_RINGS + 1

#define LED_PIN 13

// blinking stuff
int lastBlinkTime = 0;
int nextBlinkTime = 100;
boolean blinker = false;
int counter = 0;

// led states
CRGB leds[NUMPIXELS];
CRGB *ledsPanel = &leds[0],
*ledRings[NUM_RINGS] = {
	&leds[NUM_PANEL_LEDS],
	&leds[NUM_PANEL_LEDS + PIXELS_PER_RING * 1],
	&leds[NUM_PANEL_LEDS + PIXELS_PER_RING * 2],
	&leds[NUM_PANEL_LEDS + PIXELS_PER_RING * 3]
	},
	*ledReactor = &leds[NUM_PANEL_LEDS + PIXELS_PER_RING * NUM_RINGS];

// LED Ring
boolean ringLightState = false;

// LED Colours
CRGB BrightRed = 0xFF0000;
CRGB BrightGreen = 0x00FF00;

// Ideally convert these to HSV
CRGB PanelYellow = CRGB(255, 92, 15); // ?64 h, ~239.955 s, 100 v
CRGB PanelGreen = 0x00C800; // ~96 h, 255 s, ~199.92 v

// These could do with being HSV too
CRGB RingRed = 0x220000;
CRGB RingYellow = 0x222200;
CRGB RingGreen = 0x002200;
