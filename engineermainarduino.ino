#include <FastSPI_LED.h>

#define PIN_AIRLOCK 16

#define PIN_CLOCK 3
#define PIN_LOAD  4
#define PIN_DATA  2

//led stuff  ------------------
//led clock on 13
//led data on 11
#define NUM_LEDS 6

struct CRGB {
  unsigned char r; 
  unsigned char g; 
  unsigned char b; 
};
struct CRGB *leds;


byte switches[6];
byte lastSwitches[6];
boolean switchesChanged = false;

byte powerButtons = 0;
byte lastPowerButtons = 0;
long lastDbTime[4];

//map bits to switch num and pos
// {bit, switch pos }
byte switchMap[] = {  
  3,2,
  1,0,
  7,6,
  5,4,
  12,13 };

long switchPos = 0;
int lastChangedSwitch = 0;

//--------------- game logic bits ----------
#define STATE_DEAD -1
#define STATE_OFF 0
#define STATE_WARMUP 1
#define STATE_POWERING 2
#define STATE_PREON 3
#define STATE_ON 4

int gameState = STATE_OFF;

byte warmupOrder[5];
byte powerOnOrder[5];
byte currentSwitch = 0;

int lastBlinkTime = 0;
int nextBlinkTime = 100;
boolean blinker = false;
int counter = 0;

//analog bits
int lastA = -1;
int lastB = -1;
byte lastAirlock = 0;
long lastAirlockRead = 0;
boolean lockSent = false;

void setup(){
  
  //power man buttons
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

  //led setup
  FastSPI_LED.setLeds(NUM_LEDS);
  //FastSPI_LED.setChipset(CFastSPI_LED::SPI_TM1809);
  FastSPI_LED.setChipset(CFastSPI_LED::SPI_LPD6803);
  //  FastSPI_LED.setPin(PIN);
  FastSPI_LED.init();
  FastSPI_LED.start();

  leds = (struct CRGB*)FastSPI_LED.getRGBData(); 
  memset(leds, 0, NUM_LEDS * 3);  

  //serial shit
  Serial.begin(9600);
  reset();

}

void reset(){
  for (int i = 0; i < 5; i++){

    leds[i].r = 0;
    leds[i].g = 0;
    leds[i].b = 0;  
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
 
  for(int i = 4; i > 0; i--){
    int rand = random(i+1);
    if(rand != i){
      int t = warmupOrder[i];
      warmupOrder[i] = warmupOrder[rand];
      warmupOrder[rand] = t;
    }
  }
  for(int i = 4; i > 0; i--){
    int rand = random(i+1);
    if(rand != i){
      int t = powerOnOrder[i];
      powerOnOrder[i] = powerOnOrder[rand];
      powerOnOrder[rand] = t;
    }
  }

  leds[6].r = 255;
  leds[6].g = 0;
  leds[6].b = 0;
  gameState = STATE_OFF;
  currentSwitch = 0;
  nextBlinkTime = 100;

  Serial.print("S0,");


}  

void readSwitches(){
  switchPos = 0;
  switchesChanged = false;
  digitalWrite(PIN_LOAD, LOW);
  digitalWrite(PIN_LOAD, HIGH);


  delay(5);
  for(int i = 0 ; i < 16; i++){
    digitalWrite(PIN_CLOCK, LOW);
    byte a = digitalRead(PIN_DATA);
    switchPos |= ( (long)a << i);
    digitalWrite(PIN_CLOCK,HIGH);
  }

  for(int i = 0; i < 5; i++){
    byte p = (switchPos & 1l << switchMap[i * 2] ) == 0 ? 0 : 1; 

    byte p2 = (switchPos & 1l << switchMap[i * 2 + 1] ) == 0 ? 0 : 1; 
    p2 <<= 1;
    p = p2 | p ;


    switches[i] = p;
    if(lastSwitches[i] != switches[i]){
      lastSwitches[i] = p;
      lastChangedSwitch = i;
//      Serial.(lastChangedSwitch);
      switchesChanged = true;
    }      

    /*
    Serial.print ( i ); 
     Serial.print ( " : ");   
     Serial.println(switches[i]);       
     */
  }
  switches[5] = (switchPos & 1l << 15) == 0 ? 1 : 0; //inverted for the power switch



  for(int i = 0; i < 4; i++){
    
    byte a = 0;
    
    a = digitalRead(5+i);
    
    
    if( a == LOW){
      if(! (lastPowerButtons & (1 << i) > 0) && lastDbTime[i] + 50 < millis()){
        Serial.print("p");
        Serial.print(i);
        Serial.print(",");
      }     
      lastPowerButtons |= (1 << i);
      lastDbTime[i] = millis();

      
    } else {
      lastPowerButtons &= ~(1 << i);
    }
  }
//  Serial.println(lastPowerButtons, BIN);

}

void readAnalog(){
    //do analog
  int valA = 11 - map(analogRead(4), 0, 1024, 10,0);
  int valB = 11 - map(analogRead(5), 0, 1024, 10,0);

  if(valA != lastA){
    lastA = valA;
    Serial.print("B");
    Serial.print(valA);
    Serial.print(",");
  }
  if(valB != lastB){
    lastB = valB;
    Serial.print("A");
    Serial.print(valB);
    Serial.print(",");
  }

}

void loop(){
  
  while(Serial.available() > 0){
    char c = Serial.read();
    if( c == 'r'){
      reset();
    } else if (c == 'k'){
      gameState = STATE_DEAD;
      for (int i = 0; i < 6; i++){

        leds[i].r = 0;
        leds[i].g = 0;
        leds[i].b = 0;  
      }
    }
  }
  
  if(gameState != STATE_DEAD){
    readSwitches();  
    readAnalog();
  
    if(lastAirlockRead + 10 < millis()){
      lastAirlockRead = millis();
      byte b = digitalRead(PIN_AIRLOCK);
      if((b == lastAirlock)){
        if(b == 0){
          if(!lockSent){
            Serial.print("L,");
            lockSent = true;
          }
        } else {
          lockSent = false;
        }
      }
      lastAirlock = b;
    }
        
  }
  //thingy counter
  counter ++;
  // blinker
  if(lastBlinkTime + nextBlinkTime < counter){
    lastBlinkTime = counter;
    blinker = !blinker;
  }

  if(gameState == STATE_DEAD){
    for (int i = 0; i < 7; i++){
      leds[i].r = 0;
      leds[i].r = 0;
      leds[i].r = 0;
    }
  } else if(gameState == STATE_OFF){
    for (int i = 0; i < 6; i++){
      leds[i].r = 0;
      leds[i].r = 0;
      leds[i].r = 0;
    }
    if(blinker){
      //      Serial.println(switches[0]);
      for (int i = 0; i < 6; i++){
        if(switches[i] != 1){
          leds[i].r = 255;
          leds[i].g = 0;
          leds[i].b = 0;
        }
      }
    } 
    else {
      for (int i = 0; i < 6; i++){

        leds[i].r = 0;
        leds[i].g = 0;
        leds[i].b = 0;

      }
    }

    boolean states = true;
    for (int i = 0; i < 5; i++){
      if(switches[i] != 1){
        states = false;
      }
    }
    if(switches[5] != 1){
      states = false;
    }
    if(states){
      gameState = STATE_WARMUP;
      
      lastChangedSwitch = -1;
    }
  } 
  else if(gameState == STATE_WARMUP){
    int curLight = warmupOrder[currentSwitch];
    //clear leds

    leds[5].r = 255;
    if(blinker){

      //blink the current light
      leds[curLight].r = 255;
      leds[curLight].g = 92;
      leds[curLight].b = 15;

    } 
    else {
      //blink current light
      leds[curLight].r = 0;
      leds[curLight].g = 0;
      leds[curLight].b = 0;
    }
    /*
    if( switches[ curLight] == 0){
      leds[curLight].r = 255;
      leds[curLight].g = 92;
      leds[curLight].b = 15;
      currentSwitch ++;
    }*/
    if(currentSwitch == 5){
      currentSwitch = 0;
      gameState = STATE_POWERING;

//      Serial.println("STATE TO POWERING");
    }
    if(switchesChanged){
      if(lastChangedSwitch == curLight){
        leds[curLight].r = 255;
        leds[curLight].g = 92;
        leds[curLight].b = 15;
        currentSwitch ++;
        Serial.print("S");
        Serial.print(currentSwitch);
        Serial.print(",");
      } else {
         reset();
      }
    }
  } 
  else if(gameState == STATE_POWERING){
    int curLight = powerOnOrder[currentSwitch];
    //clear leds

    leds[5].r = 255;
    if(blinker){
      //blink the current light
      leds[curLight].r = 0;
      leds[curLight].g = 200;
      leds[curLight].b = 0;

    } 
    else {
      //blink current light
      leds[curLight].r = 0;
      leds[curLight].g = 0;
      leds[curLight].b = 0;
    }
    
    if(currentSwitch == 5){
      currentSwitch = 0;
      gameState = STATE_PREON;
//      Serial.println("STATE TO PRE-ON");
      nextBlinkTime = 75;
    }
    if(switchesChanged){
      if(lastChangedSwitch == curLight){
        if(switches[curLight] == 2){
          leds[curLight].r = 0;
          leds[curLight].g = 200;
          leds[curLight].b = 0;
          currentSwitch ++;
          Serial.print("S");
          Serial.print(currentSwitch + 5);
          Serial.print(",");


        } else {
          reset();
          
        }
      } else {
         reset();
      }
    }
  } 
  else if (gameState == STATE_PREON){
    boolean powerState = true;
    for (int i = 0; i < 5; i++){
      leds[i].r = 0;
      leds[i].r = 0;
      leds[i].r = 0;
      if(switches[i] != 2){
        powerState = false;
      }
    }
    if(powerState == false){
      reset();
    }

    if(blinker){ 
      leds[5].g = 255;
    } 
    else {
      leds[5].g = 0;
    }
    if(switches[5] == 0){
      gameState = STATE_ON;
      Serial.print("R1,");
    }
  } 
  else if(gameState == STATE_ON){
    boolean powerState = true;
    for (int i = 0; i < 5; i++){
      leds[i].r = 0;
      leds[i].g = 255;
      leds[i].b = 0;
      //if(switches[i] != 2){
       // powerState = false;
      //}
    }
    //power light
    leds[5].g = 255;

    if(switches[5] != 0){
      powerState = false;
    }
    if(powerState == false){
      reset();
    }
  }


  //leds
  FastSPI_LED.show();
}

