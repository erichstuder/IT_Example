/* IT - Internal Tracer
 * Copyright (C) 2019 Erich Studer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <://www.gnu.org/licenses/>.
 */

#include "pch.h"
#include "Arduino.h"
#include "app.h"

HardwareSerial Serial;
unsigned char TCCR1A;
unsigned char TCCR1B;
short OCR1A;
unsigned char TIMSK1;

unsigned long millis(void) { return 1042; }
void pinMode(uint8_t, uint8_t) {}

uint8_t writtenLedValue = LOW;
void digitalWrite(uint8_t pin, uint8_t value) {
	ASSERT_EQ(pin, 13);
	writtenLedValue = value;
}

extern void setup(void);
extern void loop(void);

void appInit_Spy(AppCallbacks_t callbacks) {
	Serial.writeCalled = false;
	ASSERT_EQ(callbacks.writeByteToUart(0xA8), AppError::NoError);
	ASSERT_TRUE(Serial.writeCalled);

	Serial.readCalled = false;
	unsigned char myByte = 0;
	ASSERT_EQ(callbacks.readByteFromUart(&myByte), AppError::NoError);
	ASSERT_EQ(myByte, 42);
	ASSERT_TRUE(Serial.readCalled);

	ASSERT_EQ(callbacks.getCurrentMillis(), 1042);
}

bool appTickCalled;
void appTick_Spy(void) {
	appTickCalled = true;
}

class DeviceWithServerTest : public ::testing::Test {
protected:
	void (*appTick_Original)(void) = NULL;
	void (*appInit_Original)(AppCallbacks_t callbacks) = NULL;

	DeviceWithServerTest() {}

	virtual ~DeviceWithServerTest() {}

	virtual void SetUp() {
		appInit_Original = appInit;
		appInit = appInit_Spy;

		appTick_Original = appTick;
		appTick = appTick_Spy;

		appTickCalled = false;
	}

	virtual void TearDown() {
		appInit = appInit_Original;
		appTick = appTick_Original;
	}
};

TEST_F(DeviceWithServerTest, setup) {
	setup();
	//timerSetup
	ASSERT_EQ(TCCR1A, 0);
	ASSERT_EQ(TCCR1B, 0x0D);
	ASSERT_EQ(OCR1A, 15625);
	ASSERT_EQ(TIMSK1, 0x02);
	//Serial
	ASSERT_EQ(Serial.baudRate, 9600);
}

TEST_F(DeviceWithServerTest, loopNoEvent) {
	writtenLedValue = HIGH;
	loop();
	ASSERT_FALSE(appTickCalled);
	ASSERT_EQ(writtenLedValue, LOW);
}

TEST_F(DeviceWithServerTest, loopEvent) {
	writtenLedValue = LOW;
	timerISR();
	loop();
	ASSERT_TRUE(appTickCalled);
	ASSERT_EQ(writtenLedValue, HIGH);
}

TEST_F(DeviceWithServerTest, loopEventIsReset) {
	timerISR();
	loop();
	writtenLedValue = HIGH;
	appTickCalled = false;
	loop();
	ASSERT_FALSE(appTickCalled);
	ASSERT_EQ(writtenLedValue, LOW);
}
