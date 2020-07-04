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
#include "Arduino.h"
#include "app.h"

HardwareSerial Serial;
unsigned char TCCR1A;
unsigned char TCCR1B;
short OCR1A;
unsigned char TIMSK1;

extern void setup_ForCppUTest(void);
extern void loop(void);
extern void timerISR(void);

static void createEvent(void) {
	timerISR();
}

void pinMode(uint8_t pin, uint8_t value) {
	mock().actualCall("pinMode")
		.withIntParameter("pin", pin)
		.withIntParameter("value", value);
}

void digitalWrite(uint8_t pin, uint8_t value) {
	mock().actualCall("digitalWrite")
		.withIntParameter("pin", pin)
		.withIntParameter("value", value);
}

static void appInit_Mock(AppCallbacks_t callbacks) {
	mock().actualCall("appInit_Mock")
		.withBoolParameter("byteFromUartAvailable_notNull", callbacks.byteFromUartAvailable != NULL)
		.withBoolParameter("readByteFromUart_notNull", callbacks.readByteFromUart != NULL)
		.withBoolParameter("writeByteToUart_notNull", callbacks.writeByteToUart != NULL)
		.withBoolParameter("getTickMicros_notNull", callbacks.getCurrentMicros != NULL);
}

static void appTick_Mock(void) {
	mock().actualCall("appTick_Mock");
}

TEST_GROUP(DeviceWithServerTest) {
	void setup() {
		setup_ForCppUTest();
	}

	void teardown() {
		mock().checkExpectations();
		mock().clear();
	}
};

TEST(DeviceWithServerTest, setup) {
	UT_PTR_SET(appInit, appInit_Mock);
	mock().expectOneCall("appInit_Mock")
		.withBoolParameter("byteFromUartAvailable_notNull", true)
		.withBoolParameter("readByteFromUart_notNull", true)
		.withBoolParameter("writeByteToUart_notNull", true)
		.withBoolParameter("getTickMicros_notNull", true);

	setup_ForCppUTest();

	LONGS_EQUAL(0, TCCR1A);
	LONGS_EQUAL(0x09, TCCR1B);
	LONGS_EQUAL(16000, OCR1A);
	LONGS_EQUAL(0x02, TIMSK1);
}

TEST(DeviceWithServerTest, loopWithNoEvent) {
	UT_PTR_SET(appTick, appTick_Mock);

	mock().expectOneCall("pinMode")
		.withParameter("pin", 13)
		.withParameter("value", 1);
	mock().expectOneCall("digitalWrite")
		.withParameter("pin", 13)
		.withParameter("value", 1);
	mock().expectNoCall("appTick_Mock");
	mock().expectNoCall("Serial.write");

	loop();
}

TEST(DeviceWithServerTest, loopWithEvent) {
	UT_PTR_SET(appTick, appTick_Mock);

	mock().expectOneCall("pinMode")
		.withParameter("pin", 13)
		.withParameter("value", 1);
	mock().expectOneCall("digitalWrite")
		.withParameter("pin", 13)
		.withParameter("value", 0);
	mock().expectOneCall("appTick_Mock");
	mock().expectOneCall("Serial.write");

	createEvent();

	loop();
}

TEST(DeviceWithServerTest, loopEventIsReset) {
	UT_PTR_SET(appTick, appTick_Mock);
	mock().ignoreOtherCalls();

	createEvent();

	mock().expectOneCall("appTick_Mock");
	loop();

	mock().expectNoCall("appTick_Mock");
	loop();

	createEvent();

	mock().expectOneCall("appTick_Mock");
	loop();
}
