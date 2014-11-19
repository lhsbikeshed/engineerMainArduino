// OFF


void stateOff() {
  ringLightState = false;

  // ------clear reactor leds
  for (int i = 0; i < NUM_PANEL_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  *ledReactor = CRGB::Black;
  // --------------------------

  // ----------- blink the reactor leds red
  if (blinker) {
    for (int i = 0; i < NUM_PANEL_LEDS; i++) {
      if (switches[i] != 1) {
        ledsPanel[i] = BrightRed;
      }
    }
    *ledReactor = BrightRed;
  }
  else {
    for (int i = 0; i < NUM_PANEL_LEDS; i++) {
      ledsPanel[i] = CRGB::Black;
    }
    *ledReactor = CRGB::Black;
  }

  boolean states = true;
  for (int i = 0; i < 5; i++) {
    if (switches[i] != 1) {
      states = false;
    }
  }
  if (switches[5] != 1) {
    states = false;
  }
  if (states) {
    gameState = STATE_WARMUP;
    lastChangedSwitch = -1;
  }
}
