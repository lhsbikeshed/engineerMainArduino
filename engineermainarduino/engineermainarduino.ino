#include <FastLED.h>
#include <ClickEncoder.h>
#include <TimerOne.h>



#define PIN_AIRLOCK 16

#define PIN_DATA  12
#define PIN_CLOCK 3
#define PIN_LOAD  2


// led stuff  ------------------
// led clock on 13
// led data on 11

#define NUM_PANEL_LEDS  5
#define PIXELS_PER_RING 12
#define NUM_RINGS       4
#define NUMPIXELS       NUM_PANEL_LEDS + PIXELS_PER_RING * NUM_RINGS + 1




// encoder stuff
ClickEncoder *encoders[4];
float value[4] = {
  60, 60, 60, 60
};
float lastValue[4] = {
  -1, -1, -1, -1
};

long lastReadTime = 0;

// main pane switches

byte switches[6];
byte lastSwitches[6];
boolean switchesChanged = false;


// map bits to switch num and pos
// {bit, switch pos }
byte switchMap[] = {
  3, 2,
  1, 0,
  7, 6,
  5, 4,
  12, 13
};

long switchPos = 0;
int lastChangedSwitch = 0;

// --------------- game logic bits ----------
#define STATE_DEAD -1
#define STATE_OFF 0
#define STATE_WARMUP 1
#define STATE_POWERING 2
#define STATE_PREON 3
#define STATE_ON 4

// reactor puzzle game state
int gameState = STATE_OFF;

byte warmupOrder[5];      // order of switches for warmup (orange) phase
byte powerOnOrder[5];     // order of switches for powerup (green) phse
byte currentSwitch = 0;   // switch id we are waiting for correct value for

// blinking stuff
int lastBlinkTime = 0;
int nextBlinkTime = 100;
boolean blinker = false;
int counter = 0;

// analog bits
int lastA = -1;
int lastB = -1;

// airlock switch state
byte lastAirlock = 0;
long lastAirlockRead = 0;
boolean lockSent = false;

// power ring leds
byte powerLevels[4] = {
  6, 6, 6, 6
};
boolean ringLightState = false;

// led states
CRGB leds[NUMPIXELS];
CRGB *ledsPanel = &leds[0],
     *ledRings[NUM_RINGS] = {
       &leds[NUM_PANEL_LEDS],
       &leds[NUM_PANEL_LEDS + PIXELS_PER_RING * 1],
       &leds[NUM_PANEL_LEDS + PIXELS_PER_RING * 2],
       &leds[NUM_PANEL_LEDS + PIXELS_PER_RING * 3]
     },
     *ledReactor = &leds[NUM_PANEL_LEDS + PIXELS_PER_RING * NUM_RINGS];

// LED Colours
CRGB BrightRed = 0xFF0000;
CRGB BrightGreen = 0x00FF00;

// Ideally convert these to HSV
CRGB PanelYellow = CRGB(255, 92, 15); // ?64 h, ~239.955 s, 100 v
CRGB PanelGreen = 0x00C800; // ~96 h, 255 s, ~199.92 v

// These could do with being HSV too
CRGB RingRed = 0x220000;
CRGB RingYellow = 0x222200;
CRGB RingGreen = 0x002200;


// serial handling
char buffer[10]; // serial buffer
byte bufPtr = 0;

int damageTimer = 0;

void setup() {
  FastLED.addLeds<NEOPIXEL, 13>(leds, NUMPIXELS);

  // power man buttons
  pinMode(5, INPUT);
  digitalWrite(5, HIGH);
  pinMode(6, INPUT);
  digitalWrite(6, HIGH);
  pinMode(7, INPUT);
  digitalWrite(7, HIGH);
  pinMode(8, INPUT);
  digitalWrite(8, HIGH);

  pinMode(PIN_AIRLOCK, INPUT);
  digitalWrite(PIN_AIRLOCK, HIGH);

  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_LOAD, OUTPUT);
  pinMode(PIN_DATA, INPUT);

  digitalWrite(PIN_CLOCK, LOW);

  // I'm pretty sure you shouldn't do this.
  memset(leds, 0, NUMPIXELS * 3);

  // serial shit
  Serial.begin(9600);
  reset();

  encoders[0] = new ClickEncoder(4 , 5);
  encoders[0]->setAccelerationEnabled(false);
  encoders[1] = new ClickEncoder(6, 7);
  encoders[1]->setAccelerationEnabled(false);
  encoders[2] = new ClickEncoder(8, 9);
  encoders[2]->setAccelerationEnabled(false);
  encoders[3] = new ClickEncoder(10, 11);
  encoders[3]->setAccelerationEnabled(false);

  powerLevels[0] = 60;
  powerLevels[1] = 60;
  powerLevels[2] = 60;
  powerLevels[3] = 60;

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);
}

void timerIsr() {
  for (int i = 0; i < 4; i++) {
    encoders[i]->service();
  }

}

void reset() {
  for (int i = 0; i < NUM_PANEL_LEDS; i++) {
    ledsPanel[i] = CRGB::Black;
  }

  warmupOrder[0] = 0;
  warmupOrder[1] = 2;
  warmupOrder[2] = 4;
  warmupOrder[3] = 1;
  warmupOrder[4] = 3;

  powerOnOrder[0] = 1;
  powerOnOrder[1] = 4;
  powerOnOrder[2] = 2;
  powerOnOrder[3] = 0;
  powerOnOrder[4] = 3;

  for (int i = 4; i > 0; i--) {
    int rand = random(i + 1);
    if (rand != i) {
      int t = warmupOrder[i];
      warmupOrder[i] = warmupOrder[rand];
      warmupOrder[rand] = t;
    }
  }

  for (int i = 4; i > 0; i--) {
    int rand = random(i + 1);
    if (rand != i) {
      int t = powerOnOrder[i];
      powerOnOrder[i] = powerOnOrder[rand];
      powerOnOrder[rand] = t;
    }
  }

  *ledReactor = BrightRed;

  gameState = STATE_OFF;
  currentSwitch = 0;
  nextBlinkTime = 100;

  Serial.print("S0,");
}

void readSwitches() {
  switchPos = 0;
  switchesChanged = false;
  digitalWrite(PIN_LOAD, LOW);
  digitalWrite(PIN_LOAD, HIGH);

  delay(5);

  for (int i = 0 ; i < 16; i++) {
    digitalWrite(PIN_CLOCK, LOW);
    byte a = digitalRead(PIN_DATA);
    switchPos |= ( (long)a << i);
    digitalWrite(PIN_CLOCK, HIGH);
  }

  for (int i = 0; i < 5; i++) {
    byte p = (switchPos & 1l << switchMap[i * 2] ) == 0 ? 0 : 1;
    byte p2 = (switchPos & 1l << switchMap[i * 2 + 1] ) == 0 ? 0 : 1;
    p2 <<= 1;
    p = p2 | p ;

    switches[i] = p;
    if (lastSwitches[i] != switches[i]) {
      lastSwitches[i] = p;
      lastChangedSwitch = i;
      //      Serial.(lastChangedSwitch);
      switchesChanged = true;
    }
  }
  switches[5] = (switchPos & 1l << 15) == 0 ? 1 : 0; // inverted for the power switch
}

void readEncoders() {
  int powerlevel = 0;
  boolean sendValues = false;
  for (int i = 0; i < 4; i++) {
    int v = encoders[i]->getValue();
    if (v > 0) {
      setValue(i, 3);
    }
    else if ( v < 0) {
      setValue(i, -3);
    }

    if (lastValue[i] != value[i]) {
      lastValue[i] = value[i];
      powerLevels[3 - i] = value[i] / 10;
      sendValues = true;
    }
  }

  if (sendValues) {
    Serial.print("p");
    Serial.print((char)(powerLevels[0] + 65));
    Serial.print((char)(powerLevels[1] + 65));
    Serial.print((char)(powerLevels[2] + 65));
    Serial.print((char)(powerLevels[3] + 65));

    Serial.print(',');
  }
}

void setValue(int index, int val) {
  int remainderToAssign = 3;
  int sign = val < 0 ? -1 : 1;

  // if were trying to move a dial out of bounds then return straight away
  if (value[index] <= 0 && sign == -1) {
    return;
  }
  else if (value[index] >= 120 && sign == 1) {
    return;
  }

  // work out
  if (value[index] + val < 0) {
    remainderToAssign -= abs(value[index] + val);
    value[index] = 0;
  }
  else if (value[index] + val > 120) {
    remainderToAssign -= value[index] + val - 120;
    Serial.println(remainderToAssign);
    value[index] = 120;
  }
  else {
    value[index] += val;
  }

  // add remainder to everything else
  int nextIndex = (index + 1 ) % 4;
  while (remainderToAssign > 0) {
    if (value[nextIndex] - sign  >= 0 && value[nextIndex] - sign  < 120) {
      value[nextIndex] += -sign;
      remainderToAssign--;
    }

    nextIndex ++;
    nextIndex %= 4;

    if (nextIndex == index) {
      nextIndex++;
      nextIndex %= 4;
    }
  }
}

void readAnalog() {
  // do analog
  int valA = 11 - map(analogRead(4), 0, 1024, 10, 0);
  int valB = 11 - map(analogRead(5), 0, 1024, 10, 0);

  if (valA != lastA) {
    lastA = valA;
    Serial.print("B");
    Serial.print(valA);
    Serial.print(",");
  }
  if (valB != lastB) {
    lastB = valB;
    Serial.print("A");
    Serial.print(valB);
    Serial.print(",");
  }
}

/*do the ring leds*/
void updatePowerRings() {
  int i, j;
  // Reset all LEDs to black
  // TODO: Potential subtle bug: Previous iteration of this also set reactor led to 0.
  for (i = 0; i < NUM_RINGS; i++) {
    for (j = 0; j < PIXELS_PER_RING; j++) {
      ledRings[i][j] = CRGB::Black;
    }
  }

  if (!ringLightState)
    return;

  int ledId;
  for (i = 0; i < NUM_RINGS; i++) {
    for (ledId = 0; ledId < powerLevels[i]; ledId++) {
      if (ledId < 4) {
        ledRings[i][ledId] = RingRed;
      }
      else if (ledId < 8) {
        ledRings[i][ledId] = RingYellow;
      }
      else {
        ledRings[i][ledId] = RingGreen;
      }
    }
  }
}

void processBuffer() {
  char c = buffer[0];
  if ( c == 'r') {        // reset the game states
    reset();
  }
  else if (c == 'k') {   // ship was killed, turn off all the bling
    gameState = STATE_DEAD;
    for (int i = 0; i < NUM_PANEL_LEDS; i++) {
      ledsPanel[i] = CRGB::Black;
    }
  }
  else if (c == 'D') {   // set dial value, format is D<dial num + 65><value + 65>
    // +65 is so that its readable in serial monitor A=0, B=1 etc
  }
  else if (c == 'd') { // damage
    damageTimer = 60;
  }
}

void loop() {
  // memset(leds, 0, NUMPIXELS * 3);
  readEncoders();
  updatePowerRings();

  // -------------------- serial reads --------------
  while ( Serial.available() > 0) {  // If data is available,
    char c = Serial.read();
    if (c == ',') {
      processBuffer();
      bufPtr = 0;
    }
    else {
      buffer[bufPtr] = c;
      if (bufPtr + 1 < 5) {
        bufPtr++;
      }
      else {
        bufPtr = 0;
      }
    }
  }

  if (gameState != STATE_DEAD) {
    readSwitches();
    readAnalog();

    if (lastAirlockRead + 10 < millis()) {
      lastAirlockRead = millis();
      byte b = digitalRead(PIN_AIRLOCK);
      if ((b == lastAirlock)) {
        if (b == 0) {
          if (!lockSent) {
            Serial.print("L,");
            lockSent = true;
          }
        }
        else {
          lockSent = false;
        }
      }
      lastAirlock = b;
    }

    if (damageTimer > 0) {
      damageTimer --;
    }

  }

  // thingy counter
  counter ++;
  // blinker
  if (lastBlinkTime + nextBlinkTime < counter) {
    lastBlinkTime = counter;
    blinker = !blinker;
  }

  if (gameState == STATE_DEAD) {
    stateDead();
  }
  else if (gameState == STATE_OFF) {      // -----ship is off-------
    stateOff();
  }
  else if (gameState == STATE_WARMUP) {
    stateWarmup();
  }
  else if (gameState == STATE_POWERING) {
    statePowering();
  }
  else if (gameState == STATE_PREON) {
    statePreOn();
  }
  else if (gameState == STATE_ON) {
    stateOn();
  }

  // flicker everything if the damage timer is > 0
  if (damageTimer > 0) {
    for (int i = 0; i < NUMPIXELS; i++) {
      if (random(255) > 128) {
        leds[i] = CRGB::Black;
      }
    }
  }

  // leds
  FastSPI_LED.show();
}



void stateDead() {
  for (int i = 0; i < NUMPIXELS; i++) {
    leds[i] = CRGB::Black;
  }
  ringLightState = false;
}

void stateOff() {
  ringLightState = false;

  // ------clear reactor leds
  for (int i = 0; i < NUM_PANEL_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  *ledReactor = CRGB::Black;
  // --------------------------

  // ----------- blink the reactor leds red
  if (blinker) {
    for (int i = 0; i < NUM_PANEL_LEDS; i++) {
      if (switches[i] != 1) {
        ledsPanel[i] = BrightRed;
      }
    }
    *ledReactor = BrightRed;
  }
  else {
    for (int i = 0; i < NUM_PANEL_LEDS; i++) {
      ledsPanel[i] = CRGB::Black;
    }
    *ledReactor = CRGB::Black;
  }

  boolean states = true;
  for (int i = 0; i < 5; i++) {
    if (switches[i] != 1) {
      states = false;
    }
  }
  if (switches[5] != 1) {
    states = false;
  }
  if (states) {
    gameState = STATE_WARMUP;
    lastChangedSwitch = -1;
  }
}

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

  if (switchesChanged) {
    if (lastChangedSwitch == curLight) {
      ledsPanel[curLight] = PanelYellow;
      currentSwitch ++;
      Serial.print("S");
      Serial.print(currentSwitch);
      Serial.print(",");
    }
    else {
      reset();
    }
  }
}

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
    nextBlinkTime = 75;
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
