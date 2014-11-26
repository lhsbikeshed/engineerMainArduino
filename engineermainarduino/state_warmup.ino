// WARMUP

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

  // current switch is 5, all orange is done
  if (currentSwitch == 5) {
    currentSwitch = 0;
    gameState = STATE_POWERING;
  }

  if (expectSwitchChange(curLight, SWITCH_POS_MID)) {
    currentSwitch++;
    Serial.print("S");
    Serial.print(currentSwitch);
    Serial.print(",");
  }
}

