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

#define REACTOR_SWITCH 5

// main pane switches
byte switches[6];
boolean switchesChanged = false;
int lastChangedSwitch = 0;

byte warmupOrder[5];      // order of switches for warmup (orange) phase
byte powerOnOrder[5];     // order of switches for powerup (green) phse
byte currentSwitch = 0;   // switch id we are waiting for correct value for
