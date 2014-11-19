// DEAD

void stateDead() {
  for (int i = 0; i < NUMPIXELS; i++) {
    leds[i] = CRGB::Black;
  }
  ringLightState = false;
}
