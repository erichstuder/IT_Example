//test double
#pragma once

#include "CppUTestExt/MockSupport.h"

#define LED_BUILTIN 13

#define HIGH 0x1
#define LOW  0x0

#define OUTPUT 0x1

#define _BV(bit) (1 << (bit))

typedef unsigned char uint8_t;

class HardwareSerial {
public:
	int available(void) {
		return 0;
	}

	int read(void) {
		return 0;
	}

	size_t write(uint8_t *buf, unsigned short len) {
		mock().actualCall("Serial.write");
		return 0; 
	}

	operator bool(void) { 
		return true;
	}
};
extern HardwareSerial Serial;

extern void pinMode(uint8_t pin, uint8_t value);
extern void digitalWrite(uint8_t, uint8_t);

//see: iom32u4.h
#define TIMER1_COMPA_vect

#define ISR(x) void timerISR(void)

extern unsigned char TCCR1A;

extern unsigned char TCCR1B;
#define CS10 0
#define CS12 2
#define WGM12 3

extern short OCR1A;

extern unsigned char TIMSK1;
#define OCIE1A 1
