// PRE-ON

/* pre-on state, all lights blinking green, wait for reactor switch to be turned */
void statePreOn() {
  // lets check that someone hasnt cocked the switches up
  boolean powerState = true;
  for (int i = 0; i < NUM_PANEL_LEDS; i++) {
    ledsPanel[i] = CRGB::Black;
    if (switches[i] != 2) {
      powerState = false;
    }
  }
  // reset if they screwed up
  if (powerState == false) {
    reset();
  }

  // blink the reactor light
  if (blinker) {
    *ledReactor = BrightGreen;
  }
  else {
    *ledReactor = CRGB::Black;
  }

  // if the reactor twisty switch has been twisted then turn the ship on
  if (switches[5] == 0) {
    gameState = STATE_ON;
    Serial.print("R1,");
    ringLightState = true;
  }
}
