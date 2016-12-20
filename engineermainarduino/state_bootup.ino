// bootup

// Reactor takes 20 seconds to start
const unsigned long bootupTime = 20000;

CHSV one = CHSV(96, 255, 255);
CHSV two = CHSV(96, 100, 255);

void setStateBootup() {
  startupStart = millis();
  clearLEDs();
}

void stateBootup() {
  unsigned long now = millis();
  const unsigned long timeElapsed = now - startupStart;
  if (timeElapsed > bootupTime) {
    setState(STATE_ON);
    return;
  }
  const unsigned long timeLeft = bootupTime - timeElapsed;
  const double percent = static_cast<double>(timeElapsed) / static_cast<double>(bootupTime);

  // clear leds
  // Don't clear the LEDs for now since the bad code after this relies on stuff sticking
  //clearLEDs();

  if (now % 75 < 10) {
    // set the LEDs to be the colours they should be
    for (int i = 0; i < NUM_PANEL_LEDS; ++i) {
      ledsPanel[i] = CHSV(96, 255, random(50 + floor(150.0 * percent), 200));
    }
  }

  // Spinners!
  // We don't have to worry about clearing the LEDs because the magic code that runs before this does that
  int i, j;
  const int test = PIXELS_PER_RING / 2;
  int slowNow = now / 100;
  int otherNow = slowNow + test;
  int x = slowNow % PIXELS_PER_RING;
  int y = otherNow % PIXELS_PER_RING;
  for (i = 0; i < NUM_RINGS; i++) {
    //int wat1 = i % 2 == 0 ? x : PIXELS_PER_RING - x - 1;
    //int wat2 = i % 2 == 0 ? y : PIXELS_PER_RING - y - 1;
    ledRings[i][x] = RingYellow;
    ledRings[i][y] = RingYellow;
  }
}
