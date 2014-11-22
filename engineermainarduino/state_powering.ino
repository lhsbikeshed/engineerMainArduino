// powering

void statePowering() {
  int curLight = powerOnOrder[currentSwitch];
  // set the reactor switch led to red
  *ledReactor = BrightRed;

  // ----- blink current light green
  if (blinker) {
    ledsPanel[curLight] = PanelGreen;
  }
  else {
    ledsPanel[curLight] = CRGB::Black;
  }

  // made it to switch 5 without cocking up, switch to pre-on state
  if (currentSwitch == 5) {
    currentSwitch = 0;
    gameState = STATE_PREON;
    setBlinkSpeed(75);
  }
  // did the switches change? if so then set the next one in sequence to green and transmit the current switch state
  if (switchesChanged) {
    if (lastChangedSwitch == curLight) {
      if (switches[curLight] == 2) {
        ledsPanel[curLight] = PanelGreen;
        currentSwitch ++;
        Serial.print("S");
        Serial.print(currentSwitch + 5);
        Serial.print(",");
      }
      else {
        reset();
      }
    }
    else {
      reset();
    }
  }
}
