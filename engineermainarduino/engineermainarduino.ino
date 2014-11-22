#include <FastLED.h>
#include <ClickEncoder.h>
#include <TimerOne.h>

#include "leds.h"
#include "encoders.h"
#include "switches.h"


// --------------- game logic bits ----------
#define STATE_DEAD -1
#define STATE_OFF 0
#define STATE_WARMUP 1
#define STATE_POWERING 2
#define STATE_PREON 3
#define STATE_ON 4

// reactor puzzle game state
int gameState = STATE_OFF;

// serial handling
char buffer[10]; // serial buffer
byte bufPtr = 0;

int damageTimer = 0;

void setup() {
  setupLEDs();
  setupAirlock();
  setupSwitches();
  setupEncoders();

  // serial shit
  Serial.begin(9600);
  reset();
}

void reset() {
  resetLEDs();
  resetSwitches();

  *ledReactor = BrightRed;

  gameState = STATE_OFF;
}

void processBuffer() {
  char c = buffer[0];
  if ( c == 'r') {        // reset the game states
    reset();
  }
  else if (c == 'k') {   // ship was killed, turn off all the bling
    gameState = STATE_DEAD;
    for (int i = 0; i < NUM_PANEL_LEDS; i++) {
      ledsPanel[i] = CRGB::Black;
    }
  }
  else if (c == 'D') {   // set dial value, format is D<dial num + 65><value + 65>
    // +65 is so that its readable in serial monitor A=0, B=1 etc
  }
  else if (c == 'd') { // damage
    damageTimer = 60;
  }
}

void readSerial() {
  // -------------------- serial reads --------------
  while ( Serial.available() > 0) {  // If data is available,
    char c = Serial.read();
    if (c == ',') {
      processBuffer();
      bufPtr = 0;
    }
    else {
      buffer[bufPtr] = c;
      if (bufPtr + 1 < 5) {
        bufPtr++;
      }
      else {
        bufPtr = 0;
      }
    }
  }
}

void loop() {
  // memset(leds, 0, NUMPIXELS * 3);
  readEncoders();
  updatePowerRings();

  readSerial();

  if (gameState != STATE_DEAD) {
    readSwitches();
    readAnalog();
    readAirlock();

    if (damageTimer > 0) {
      damageTimer --;
    }
  }

  LEDBlinkThink();

  if (gameState == STATE_DEAD) {
    stateDead();
  }
  else if (gameState == STATE_OFF) {      // -----ship is off-------
    stateOff();
  }
  else if (gameState == STATE_WARMUP) {
    stateWarmup();
  }
  else if (gameState == STATE_POWERING) {
    statePowering();
  }
  else if (gameState == STATE_PREON) {
    statePreOn();
  }
  else if (gameState == STATE_ON) {
    stateOn();
  }

  // flicker everything if the damage timer is > 0
  if (damageTimer > 0) {
    for (int i = 0; i < NUMPIXELS; i++) {
      if (random(255) > 128) {
        leds[i] = CRGB::Black;
      }
    }
  }

  // leds
  showLEDs();
}
