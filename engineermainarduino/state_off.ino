// OFF

void setStateOff() {
  resetLEDs();
  resetPowerLevels();
  setBlinkSpeed(100);
}

void stateOff() {
  clearLEDs();

  // ----------- blink the reactor leds red
  if (blinker) {
    for (int i = 0; i < NUM_PANEL_LEDS; i++) {
      if (switches[i] != SWITCH_POS_DOWN) {
        ledsPanel[i] = BrightRed;
      }
    }
    *ledReactor = BrightRed;
  }

  boolean states = true;
  // Top switches
  for (int i = 0; i < 5; i++) {
    if (switches[i] != SWITCH_POS_DOWN) {
      return;
    }
  }
  // Reactor switch
  if (switches[REACTOR_SWITCH] != REACTOR_SWITCH_OFF) {
    return;
  }

  // All tests passed, let's start the reactor
  setState(STATE_WARMUP);
}
