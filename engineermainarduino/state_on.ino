// on

// On state, set everything to green
void stateOn() {
  boolean powerState = true;
  for (int i = 0; i < NUM_PANEL_LEDS; i++) {
    ledsPanel[i] = BrightGreen;
    // if(switches[i] != 2){
    // powerState = false;
    // }
  }
  // power light
  *ledReactor = BrightGreen;

  // if someone turns reactor switch off then
  if (switches[5] != 0) {
    powerState = false;
  }
  if (powerState == false) {
    reset();
  }
}
