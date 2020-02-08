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

extern void setup_ForCppUTest(void);
extern void loop(void);

unsigned long millis(void) {
	mock_c()->actualCall("millis");
	return mock_c()->returnValue().value.intValue;
}

void pinMode(uint8_t pin, uint8_t value) {
	mock_c()->actualCall("pinMode")
		->withIntParameters("pin", pin)
		->withIntParameters("value", value);
}

void digitalWrite(uint8_t pin, uint8_t value) {
	mock_c()->actualCall("digitalWrite")
		->withIntParameters("pin", pin)
		->withIntParameters("value", value);
}

void appInit_Mock(AppCallbacks_t callbacks) {
	mock_c()->actualCall("appInit_Mock")
		->withIntParameters("callbacks.getCurrentMillis", callbacks.getCurrentMillis());

	Serial.writeCalled = false;
	LONGS_EQUAL(ItError_NoError, callbacks.writeByteToUart(0xA8));
	CHECK(Serial.writeCalled);

	Serial.readCalled = false;
	char myByte = 0;
	LONGS_EQUAL(ItError_NoError, callbacks.readByteFromUart(&myByte));
	LONGS_EQUAL(42, myByte);
	CHECK(Serial.readCalled);

	/*LONGS_EQUAL(mock_c().actual, callbacks.getCurrentMillis());*/
}

void appTick_Mock(void) {
	mock_c()->actualCall("appTick_Mock");
}

TEST_GROUP(DeviceWithServerTest) {
	void setup() { 
		mock().strictOrder();
	}

	void teardown() {
		mock().checkExpectations();
		mock().clear();
	}
};

TEST(DeviceWithServerTest, setup) {
	UT_PTR_SET(appInit, appInit_Mock);
	mock().expectOneCall("appInit_Mock")
		.withParameter("callbacks.getCurrentMillis", 0);

	setup_ForCppUTest();
	LONGS_EQUAL(0, TCCR1A);
	LONGS_EQUAL(0x0D, TCCR1B);
	LONGS_EQUAL(15625, OCR1A);
	LONGS_EQUAL(0x02, TIMSK1);
}

TEST(DeviceWithServerTest, loopNoEvent) {
	setup_ForCppUTest();
	mock().expectOneCall("pinMode")
		.withParameter("pin", 13)
		.withParameter("value", 1);
	mock().expectOneCall("digitalWrite")
		.withParameter("pin", 13)
		.withParameter("value", 1);
	loop();
}

TEST(DeviceWithServerTest, loopEvent) {
	mock().expectOneCall("pinMode")
		.withParameter("pin", 13)
		.withParameter("value", 1);
	mock().expectOneCall("digitalWrite")
		.withParameter("pin", 13)
		.withParameter("value", 0);
	mock().expectOneCall("millis");
	UT_PTR_SET(appTick, appTick_Mock);
	mock().expectOneCall("appTick_Mock");
	timerISR();
	loop();
}

TEST(DeviceWithServerTest, loopEventIsReset) {
	timerISR();

	mock().expectOneCall("pinMode")
		.withParameter("pin", 13)
		.withParameter("value", 1);
	mock().expectOneCall("digitalWrite")
		.withParameter("pin", 13)
		.withParameter("value", 0);
	mock().expectOneCall("millis");
	UT_PTR_SET(appTick, appTick_Mock);
	mock().expectOneCall("appTick_Mock");
	loop();

	mock().expectOneCall("pinMode")
		.withParameter("pin", 13)
		.withParameter("value", 1);
	mock().expectOneCall("digitalWrite")
		.withParameter("pin", 13)
		.withParameter("value", 1);
	loop();
}

TEST(DeviceWithServerTest, locallyStoredMillis) {
	timerISR();

	unsigned long CurrentMillis = 9876;
	mock().expectOneCall("pinMode")
		.withParameter("pin", 13)
		.withParameter("value", 1);
	mock().expectOneCall("digitalWrite")
		.withParameter("pin", 13)
		.withParameter("value", 0);
	mock().expectOneCall("millis")
		.andReturnValue(CurrentMillis);
	UT_PTR_SET(appTick, appTick_Mock);
	mock().expectOneCall("appTick_Mock");
	loop();
	
	UT_PTR_SET(appInit, appInit_Mock);
	mock().expectOneCall("appInit_Mock")
		.withParameter("callbacks.getCurrentMillis", CurrentMillis);
	setup_ForCppUTest();
}
