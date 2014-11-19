#pragma once

#define PIN_DATA  12
#define PIN_CLOCK 3
#define PIN_LOAD  2

// main pane switches
byte switches[6];
byte lastSwitches[6];
boolean switchesChanged = false;


// map bits to switch num and pos
// {bit, switch pos }
byte switchMap[] = {
  3, 2,
  1, 0,
  7, 6,
  5, 4,
  12, 13
};

long switchPos = 0;
int lastChangedSwitch = 0;

byte warmupOrder[5];      // order of switches for warmup (orange) phase
byte powerOnOrder[5];     // order of switches for powerup (green) phse
byte currentSwitch = 0;   // switch id we are waiting for correct value for
