// powering
void setStatePowering() {
  lastChangedSwitch = -1;
  currentSwitch = 0;
}

void statePowering() {
  // clear leds
  clearLEDs();
  // set the LEDs to be the colours they should be
  for (int i = 0; i < NUM_PANEL_LEDS; ++i) {
    if (switches[i] == SWITCH_POS_MID) {
      ledsPanel[i] = PanelYellow;
    }
    else if (switches[i] == SWITCH_POS_UP) {
      ledsPanel[i] = PanelGreen;
    }
  }
  // set the reactor switch led to red
  *ledReactor = BrightRed;

  int curLight = powerOnOrder[currentSwitch];
  // ----- blink current light green
  if (blinker) {
    ledsPanel[curLight] = PanelGreen;
  }

  // did the switches change? if so then set the next one in sequence to green and transmit the current switch state
  if (! expectSwitchChange(curLight, SWITCH_POS_UP)) {
    return;
  }

  currentSwitch++;
  Serial.print("S");
  Serial.print(currentSwitch + 5);
  Serial.print(",");
  // made it to switch 5 without cocking up, switch to pre-on state
  if (currentSwitch == 5) {
    setState(STATE_PREON);
  }
}
