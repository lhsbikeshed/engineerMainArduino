// PRE-ON
void setStatePreOn() {
  setBlinkSpeed(75);
}

/* pre-on state, all lights blinking green, wait for reactor switch to be turned */
void statePreOn() {
  clearLEDs();
  // lets check that someone hasnt cocked the switches up
  boolean powerState = true;
  for (int i = 0; i < NUM_PANEL_LEDS; i++) {
    if (blinker) {
      ledsPanel[i] = PanelGreen;
    }
    if (switches[i] != SWITCH_POS_UP) {
      powerState = false;
    }
  }
  // reset if they screwed up
  if (powerState == false) {
    reset();
    return;
  }

  // blink the reactor light
  if (blinker) {
    *ledReactor = BrightGreen;
  }

  // if the reactor twisty switch has been twisted then turn the ship on
  if (switches[5] == 0) {
    Serial.print("R1,");
    setState(STATE_ON);
  }
}
