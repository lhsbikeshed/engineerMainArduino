// OFF

void setStateOff() {
  resetLEDs();
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
      states = false;
    }
  }
  // Reactor switch
    states = false;
  }
  if (states) {
    setState(STATE_WARMUP);
  if (switches[REACTOR_SWITCH] != REACTOR_SWITCH_OFF) {
  }
}
