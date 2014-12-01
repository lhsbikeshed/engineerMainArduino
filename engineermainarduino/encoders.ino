#include "encoders.h"

void setupEncoders() {
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
  Timer1.attachInterrupt(timerEncoderServicing);
}

void timerEncoderServicing() {
  for (int i = 0; i < 4; i++) {
    encoders[i]->service();
  }
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
