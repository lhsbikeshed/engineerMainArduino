// WARMUP

void setStateWarmup() {
  lastChangedSwitch = -1;
  currentSwitch = 0;
}

void stateWarmup() {
  int curLight = warmupOrder[currentSwitch];
  // clear leds
  clearLEDs();
  // turn the on switches that're yellow
  for (int i = 0; i < NUM_PANEL_LEDS; ++i) {
    if (switches[i] == SWITCH_POS_MID) {
      ledsPanel[i] = PanelYellow;
    }
  }
  // set the reactor switch led to red
  *ledReactor = BrightRed;

  // blink the current light
  if (blinker) {
    ledsPanel[curLight] = PanelYellow;
  }

  if (! expectSwitchChange(curLight, SWITCH_POS_MID)){
    return;
  }

  currentSwitch++;
  Serial.print("S");
  Serial.print(currentSwitch);
  Serial.print(",");
  // current switch is 5, all orange is done
  if (currentSwitch == 5) {
    setState(STATE_POWERING);
  }
}

