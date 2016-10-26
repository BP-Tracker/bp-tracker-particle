#include "application.h"
/*
	Compile: particle compile electron failsafe.cpp
	Upload: particle serial --flash electron_firmware_xxxx.bin or,
		particle serial --flash electron_failsafe.bin
	Usage:
		particle serial monitor
*/

// Global variables
int ON_BOARD_LED = D7;

void setup(){
	pinMode(ON_BOARD_LED, OUTPUT);
	Serial.begin(9600);
}

void loop() {

	Serial.println("LED ON");
	digitalWrite(ON_BOARD_LED, HIGH);
	delay(5000);

	Serial.println("LED OFF");
	digitalWrite(ON_BOARD_LED, LOW);
	delay(5000);
}
