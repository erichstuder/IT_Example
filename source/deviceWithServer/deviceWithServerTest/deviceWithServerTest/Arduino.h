//test double
#pragma once

#define LED_BUILTIN 13

#define HIGH 0x1
#define LOW  0x0

#define OUTPUT 0x1

#define _BV(bit) (1 << (bit))

typedef unsigned char uint8_t;
//typedef uint8_t byte;

class HardwareSerial {
public:
	unsigned long baudRate;
	bool readCalled;
	bool writeCalled;
	void begin(unsigned long baud) { baudRate = baud; }
	int available(void) { return 1; }
	int read(void) { 
		readCalled = true;
		return 42;
	}
	size_t write(uint8_t byte) {
		writeCalled = true;
		return 1; 
	}
	operator bool() { return true; }
};
extern HardwareSerial Serial;

extern unsigned long millis(void);
extern void pinMode(uint8_t pin, uint8_t value);
extern void digitalWrite(uint8_t, uint8_t);

//see: iom32u4.h
#define TIMER1_COMPA_vect

#define ISR(x) void timerISR(void)
extern void timerISR(void); //a bit dirty but it works

extern unsigned char TCCR1A;

extern unsigned char TCCR1B;
#define CS10 0
#define CS12 2
#define WGM12 3

extern short OCR1A;

extern unsigned char TIMSK1;
#define OCIE1A 1
