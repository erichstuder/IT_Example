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
#include "controller.h"

class ControllerTest : public ::testing::Test {
protected:
	ControllerTest() {}

	virtual ~ControllerTest() {}

	virtual void SetUp() {
		controllerReset();
	}

	virtual void TearDown() {
		controllerReset();
	}
};

TEST_F(ControllerTest, controllerTickExists) {
	controllerTick();
}

TEST_F(ControllerTest, setDesiredValueExists) {
	setControllerDesiredValue(0.0f);
}

TEST_F(ControllerTest, setActualValueExists) {
	setControllerActualValue(0.0f);
}

TEST_F(ControllerTest, hasP) {
	setControllerDesiredValue(1);
	setControllerActualValue(0);
	setControllerKp(1);
	setControllerKi(0);
	ASSERT_EQ(getControllerSignal(), 0);
	controllerTick();
	ASSERT_EQ(getControllerSignal(), 1);
}

TEST_F(ControllerTest, hasI) {
	setControllerDesiredValue(1.0);
	setControllerActualValue(0.0);
	setControllerKp(0.0);
	setControllerKi(2.5);
	ASSERT_EQ(getControllerSignal(), 0);
	controllerTick();
	ASSERT_EQ(getControllerSignal(), 2.5);
	controllerTick();
	controllerTick();
	controllerTick();
	ASSERT_EQ(getControllerSignal(), 10);
}

TEST_F(ControllerTest, pAndISumUp) {
	const float DesiredValue = 3;
	const float Kp = 42;
	const float Ki = 0.23f;
	
	setControllerDesiredValue(DesiredValue);
	setControllerActualValue(0.0);
	setControllerKp(Kp);
	setControllerKi(Ki);
	controllerTick();
	ASSERT_EQ(getControllerSignal(), Kp*DesiredValue+Ki*DesiredValue);
}

TEST_F(ControllerTest, desiredValueStep) {
	setControllerDesiredValue(5);
	setControllerActualValue(0.0);
	setControllerKp(1);
	setControllerKi(1);
	controllerTick();
	controllerTick();
	ASSERT_EQ(getControllerSignal(), 15);
	
	setControllerDesiredValue(33);
	controllerTick();
	controllerTick();
	ASSERT_EQ(getControllerSignal(), 109);
}

TEST_F(ControllerTest, actualValueDiffering) {
	setControllerDesiredValue(0);
	setControllerActualValue(4);
	setControllerKp(1);
	setControllerKi(1);
	controllerTick();
	controllerTick();
	ASSERT_EQ(getControllerSignal(), -12);
}

TEST_F(ControllerTest, actualValueStep) {
	setControllerDesiredValue(0);
	setControllerActualValue(10);
	setControllerKp(1);
	setControllerKi(1);
	controllerTick();
	controllerTick();
	ASSERT_EQ(getControllerSignal(), -30);

	setControllerActualValue(20);
	controllerTick();
	controllerTick();
	ASSERT_EQ(getControllerSignal(), -80);
}
