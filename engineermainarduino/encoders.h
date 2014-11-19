#pragma once
// encoder stuff
ClickEncoder *encoders[4];
float value[4] = {
	60,
	60,
	60,
	60
};
float lastValue[4] = {
	-1,
	-1,
	-1,
	-1
};

// power ring leds
byte powerLevels[4] = {
	6,
	6,
	6,
	6
};
