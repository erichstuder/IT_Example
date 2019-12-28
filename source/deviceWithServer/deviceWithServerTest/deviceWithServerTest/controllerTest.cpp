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
#include "controller.h"

TEST_GROUP(ControllerTest) {
	void setup() {
		controllerReset();
	}

	void teardown() {
		controllerReset();
	}
};

TEST(ControllerTest, controllerTickExists) {
	controllerTick();
}

TEST(ControllerTest, setDesiredValueExists) {
	setControllerDesiredValue(0.0f);
}

TEST(ControllerTest, setActualValueExists) {
	setControllerActualValue(0.0f);
}

TEST(ControllerTest, hasP) {
	setControllerDesiredValue(1);
	setControllerActualValue(0);
	setControllerKp(1);
	setControllerKi(0);
	LONGS_EQUAL(getControllerSignal(), 0);
	controllerTick();
	LONGS_EQUAL(getControllerSignal(), 1);
}

TEST(ControllerTest, hasI) {
	setControllerDesiredValue(1.0);
	setControllerActualValue(0.0);
	setControllerKp(0.0);
	setControllerKi(2.5);
	LONGS_EQUAL(getControllerSignal(), 0);
	controllerTick();
	LONGS_EQUAL(getControllerSignal(), 2.5);
	controllerTick();
	controllerTick();
	controllerTick();
	LONGS_EQUAL(getControllerSignal(), 10);
}

TEST(ControllerTest, pAndISumUp) {
	const float DesiredValue = 3;
	const float Kp = 42;
	const float Ki = 0.23f;
	
	setControllerDesiredValue(DesiredValue);
	setControllerActualValue(0.0);
	setControllerKp(Kp);
	setControllerKi(Ki);
	controllerTick();
	LONGS_EQUAL(getControllerSignal(), Kp*DesiredValue+Ki*DesiredValue);
}

TEST(ControllerTest, desiredValueStep) {
	setControllerDesiredValue(5);
	setControllerActualValue(0.0);
	setControllerKp(1);
	setControllerKi(1);
	controllerTick();
	controllerTick();
	LONGS_EQUAL(getControllerSignal(), 15);
	
	setControllerDesiredValue(33);
	controllerTick();
	controllerTick();
	LONGS_EQUAL(getControllerSignal(), 109);
}

TEST(ControllerTest, actualValueDiffering) {
	setControllerDesiredValue(0);
	setControllerActualValue(4);
	setControllerKp(1);
	setControllerKi(1);
	controllerTick();
	controllerTick();
	LONGS_EQUAL(getControllerSignal(), -12);
}

TEST(ControllerTest, actualValueStep) {
	setControllerDesiredValue(0);
	setControllerActualValue(10);
	setControllerKp(1);
	setControllerKi(1);
	controllerTick();
	controllerTick();
	LONGS_EQUAL(getControllerSignal(), -30);

	setControllerActualValue(20);
	controllerTick();
	controllerTick();
	LONGS_EQUAL(getControllerSignal(), -80);
}
