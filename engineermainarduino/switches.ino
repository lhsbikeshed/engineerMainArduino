#include "switches.h"

void setupSwitches() {
	pinMode(PIN_CLOCK, OUTPUT);
	pinMode(PIN_LOAD, OUTPUT);
	pinMode(PIN_DATA, INPUT);

	digitalWrite(PIN_CLOCK, LOW);
}

void resetSwitches() {
	// Randomise the switch order
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

	// Reset the bootup sequence
	currentSwitch = 0;
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
