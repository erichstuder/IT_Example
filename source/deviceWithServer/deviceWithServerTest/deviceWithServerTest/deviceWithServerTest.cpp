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

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTestExt/MockSupport_c.h"
#include "Arduino.h"
#include "app.h"

HardwareSerial Serial;
unsigned char TCCR1A;
unsigned char TCCR1B;
short OCR1A;
unsigned char TIMSK1;

extern void setup_CppUTest(void);
extern void loop(void);

unsigned long millis(void) { return 1042; }
void pinMode(uint8_t, uint8_t) {}

void digitalWrite(uint8_t pin, uint8_t value) {
	mock_c()->actualCall("digitalWrite")
		->withIntParameters("pin", pin)
		->withIntParameters("value", value);
}

void appInit_Mock(AppCallbacks_t callbacks) {
	mock_c()->actualCall("appInit_Mock");

	Serial.writeCalled = false;
	LONGS_EQUAL(callbacks.writeByteToUart(0xA8), AppError::NoError);
	CHECK(Serial.writeCalled);

	Serial.readCalled = false;
	char myByte = 0;
	LONGS_EQUAL(callbacks.readByteFromUart(&myByte), AppError::NoError);
	LONGS_EQUAL(myByte, 42);
	CHECK(Serial.readCalled);

	LONGS_EQUAL(callbacks.getCurrentMillis(), 1042);
}

void appTick_Mock(void) {
	mock_c()->actualCall("appTick_Mock");
}

TEST_GROUP(DeviceWithServerTest) {
	void setup() { }

	void teardown() {
		mock().checkExpectations();
		mock().clear();
	}
};

TEST(DeviceWithServerTest, setup) {
	UT_PTR_SET(appInit, appInit_Mock);
	mock().expectOneCall("appInit_Mock");

	setup_CppUTest();
	LONGS_EQUAL(TCCR1A, 0);
	LONGS_EQUAL(TCCR1B, 0x0D);
	LONGS_EQUAL(OCR1A, 15625);
	LONGS_EQUAL(TIMSK1, 0x02);
	LONGS_EQUAL(Serial.baudRate, 9600);
}

TEST(DeviceWithServerTest, loopNoEvent) {
	mock().expectOneCall("digitalWrite")
		.withParameter("pin", 13)
		.withParameter("value", 0);
	loop();
}

TEST(DeviceWithServerTest, loopEvent) {
	mock().expectOneCall("digitalWrite")
		.withParameter("pin", 13)
		.withParameter("value", 1);
	UT_PTR_SET(appTick, appTick_Mock);
	mock().expectOneCall("appTick_Mock");
	timerISR();
	loop();
}

TEST(DeviceWithServerTest, loopEventIsReset) {
	mock().expectOneCall("digitalWrite")
		.withParameter("pin", 13)
		.withParameter("value", 1);
	mock().expectOneCall("digitalWrite")
		.withParameter("pin", 13)
		.withParameter("value", 0);

	UT_PTR_SET(appTick, appTick_Mock);
	mock().expectOneCall("appTick_Mock");

	timerISR();
	loop();
	loop();
}
