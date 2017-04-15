#include "encoders.h"

void resetPowerLevels() {
  powerLevels[0] = 6;
  powerLevels[1] = 6;
  powerLevels[2] = 6;
  powerLevels[3] = 6;
  value[0] = 60;
  value[1] = 60;
  value[2] = 60;
  value[3] = 60;
}

void setupEncoders() {
  encoders[0] = new ClickEncoder(21, 20);
  encoders[0]->setAccelerationEnabled(false);
  encoders[1] = new ClickEncoder(19, 18);
  encoders[1]->setAccelerationEnabled(false);
  encoders[2] = new ClickEncoder(17, 16);
  encoders[2]->setAccelerationEnabled(false);
  encoders[3] = new ClickEncoder(15, 14);
  encoders[3]->setAccelerationEnabled(false);

  resetPowerLevels();

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
    } else if (v < 0) {
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
  } else if (value[index] >= 120 && sign == 1) {
    return;
  }

  // work out
  if (value[index] + val < 0) {
    remainderToAssign -= abs(value[index] + val);
    value[index] = 0;
  } else if (value[index] + val > 120) {
    remainderToAssign -= value[index] + val - 120;
    Serial.println(remainderToAssign);
    value[index] = 120;
  } else {
    value[index] += val;
  }

  // add remainder to everything else
  int nextIndex = (index + 1) % 4;
  while (remainderToAssign > 0) {
    if (value[nextIndex] - sign >= 0 && value[nextIndex] - sign < 120) {
      value[nextIndex] += -sign;
      remainderToAssign--;
    }

    nextIndex++;
    nextIndex %= 4;

    if (nextIndex == index) {
      nextIndex++;
      nextIndex %= 4;
    }
  }
}
