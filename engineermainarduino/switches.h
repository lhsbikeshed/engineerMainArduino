#pragma once

enum SwitchPos {
  SWITCH_POS_UP = 2,
  SWITCH_POS_MID = 0,
  SWITCH_POS_DOWN = 1,
};

enum ReactorSwitchPos {
  REACTOR_SWITCH_ON = 0,
  REACTOR_SWITCH_OFF = 1,
};

#define NUM_TOP_SWITCHES 5
#define NUM_REACTOR_SWITCHES NUM_TOP_SWITCHES + 1
#define REACTOR_SWITCH 5

// main pane switches
byte LEDsies[NUM_TOP_SWITCHES];
byte switches[NUM_REACTOR_SWITCHES];
boolean switchesChanged = false;
int lastChangedSwitch = 0;

byte warmupOrder[NUM_TOP_SWITCHES];  // order of switches for warmup (orange) phase
byte powerOnOrder[NUM_TOP_SWITCHES]; // order of switches for powerup (green) phse
byte currentSwitch = 0;              // switch id we are waiting for correct value for
