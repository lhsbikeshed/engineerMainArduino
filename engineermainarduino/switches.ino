#include "switches.h"

#define NUM_SWITCH_INPUTS NUM_TOP_SWITCHES * 2
// We count backwards
#define PIN_FIRST_SWITCH 45
#define PIN_REACTOR 47

byte switchOrder[] = {
  1, 3, 0, 2, 4,
};

byte lastSwitches[NUM_REACTOR_SWITCHES];

void setupSwitches() {
  pinMode(PIN_REACTOR, INPUT);
  digitalWrite(PIN_REACTOR, HIGH);
  for (int i = 0; i < NUM_SWITCH_INPUTS; i++) {
    int pin = PIN_FIRST_SWITCH - i;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }
}

void resetSwitches() {
  // Randomise the switch order
  warmupOrder[0] = 0;
  warmupOrder[1] = 2;
  warmupOrder[2] = 4;
  warmupOrder[3] = 1;
  warmupOrder[4] = 3;

  powerOnOrder[0] = 1;
  powerOnOrder[1] = 4;
  powerOnOrder[2] = 2;
  powerOnOrder[3] = 0;
  powerOnOrder[4] = 3;

  for (int i = 4; i > 0; i--) {
    int rand = random(i + 1);
    if (rand != i) {
      int t = warmupOrder[i];
      warmupOrder[i] = warmupOrder[rand];
      warmupOrder[rand] = t;
    }
  }

  for (int i = 4; i > 0; i--) {
    int rand = random(i + 1);
    if (rand != i) {
      int t = powerOnOrder[i];
      powerOnOrder[i] = powerOnOrder[rand];
      powerOnOrder[rand] = t;
    }
  }

  // Reset the bootup sequence
  currentSwitch = 0;
  memset(lastSwitches, 255, NUM_REACTOR_SWITCHES);
  Serial.print("S0,");
}

void readSwitches() {
  switchesChanged = false;

  memset(switches, SWITCH_POS_MID, NUM_REACTOR_SWITCHES);

  for (int i = 0; i < NUM_SWITCH_INPUTS; i++) {
    int data = digitalRead(PIN_FIRST_SWITCH - i);

    bool isUp = i % 2 == 0;
    int switchLoc = switchOrder[i / 2];

    if (data != 0) {
      continue;
    }

    if (isUp) {
      switches[switchLoc] = SWITCH_POS_UP;
    } else {
      switches[switchLoc] = SWITCH_POS_DOWN;
    }
  }

  switches[REACTOR_SWITCH] = digitalRead(PIN_REACTOR);

  for (int i = 0; i < NUM_REACTOR_SWITCHES; i++) {
    if (lastSwitches[i] != switches[i]) {
      lastSwitches[i] = switches[i];
      lastChangedSwitch = i;
      switchesChanged = true;
    }
  }
}

bool expectSwitchChange(int which, SwitchPos pos) {
  if (!switchesChanged) {
    return false;
  }
  if (lastChangedSwitch == which) {
    if (switches[which] == pos) {
      return true;
    }
  }
  reset();
  return false;
}
