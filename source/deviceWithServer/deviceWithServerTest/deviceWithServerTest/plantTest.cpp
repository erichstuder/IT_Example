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
#include "plant.h"

static const int ExpectedDelayLength = 20;

static void setPlantDelayQueueToZero(void);
static void tickThroughTheDelayQueue(void);

TEST_GROUP(PlantTest) {
	void setup() {
		setPlantDelayQueueToZero();
		LONGS_EQUAL(0, getPlantOut());
	}

	void teardown() {
	}
};

TEST(PlantTest, inputToOutput) {
	const float InputValue = 42;
	const int NumberOfTestValues = ExpectedDelayLength + 10;

	setPlantIn(InputValue);

	for (int n = 0; n < ExpectedDelayLength-1; n++) {
		LONGS_EQUAL(0, getPlantOut());
		plantTick();
	}

	for (int n = 0; n < NumberOfTestValues-ExpectedDelayLength; n++) {
		LONGS_EQUAL(InputValue, getPlantOut());
		plantTick();
	}
}

TEST(PlantTest, inputIsChangeable) {
	float InputValue = 22222;
	setPlantIn(InputValue);
	tickThroughTheDelayQueue();
	LONGS_EQUAL(InputValue, getPlantOut());

	InputValue = -30.932f;
	setPlantIn(InputValue);
	tickThroughTheDelayQueue();
	LONGS_EQUAL(InputValue, getPlantOut());
}

static void setPlantDelayQueueToZero(void) {
	setPlantIn(0);
	tickThroughTheDelayQueue();
}

static void tickThroughTheDelayQueue(void) {
	for (int n = 0; n < ExpectedDelayLength; n++) {
		plantTick();
	}
}
