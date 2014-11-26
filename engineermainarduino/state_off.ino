// OFF


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
  if (switches[5] != 1) {
    states = false;
  }
  if (states) {
    gameState = STATE_WARMUP;
    lastChangedSwitch = -1;
  }
}
