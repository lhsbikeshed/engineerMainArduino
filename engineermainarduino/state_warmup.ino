// WARMUP

void stateWarmup() {
  int curLight = warmupOrder[currentSwitch];
  // clear leds
  // set the reactor switch led to red
  *ledReactor = BrightRed;

  if (blinker) {
    // blink the current light
    ledsPanel[curLight] = PanelYellow;
  }
  else {
    // blink current light
    ledsPanel[curLight] = CRGB::Black;
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

