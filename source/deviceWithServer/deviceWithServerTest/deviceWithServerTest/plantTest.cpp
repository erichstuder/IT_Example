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

float stepValueThroughDelayWithExpectedValue(float expectedValueWhileStepping);
const float ExpectedDelayLength = 10.0f;

TEST_GROUP(PlantTest) {
	void setup() {
		clearDelayBuffer();
	}

	void teardown() {
		clearDelayBuffer();
	}

	void clearDelayBuffer() {
		setPlantIn(0);
		for (int n = 0; n < ExpectedDelayLength; n++) {
			plantTick();
		}
	}
};

TEST(PlantTest, tickFunctionExists) {
	plantTick();
}

TEST(PlantTest, plantInExists) {
	setPlantIn((float)20);
}

TEST(PlantTest, plantOutInitialValue) {
	LONGS_EQUAL(getPlantOut(), 0);
}

TEST(PlantTest, delayLength) {
	const float InputValue = 1;
	setPlantIn(InputValue);
	CHECK(stepValueThroughDelayWithExpectedValue(0));
	plantTick();
	LONGS_EQUAL(getPlantOut(), InputValue);
}

TEST(PlantTest, inputIsPersistent) {
	const float InputValue = 42;
	setPlantIn(InputValue);
	CHECK(stepValueThroughDelayWithExpectedValue(0));
	for (int n = 0; n < 1e6; n++) {
		plantTick();
		LONGS_EQUAL(getPlantOut(), InputValue);
	}
}

TEST(PlantTest, inputIsChangeable) {
	const float InputValue1 = -12;
	const float InputValue2 = 3564;

	setPlantIn(InputValue1);
	CHECK(stepValueThroughDelayWithExpectedValue(0));
	setPlantIn(InputValue2);
	CHECK(stepValueThroughDelayWithExpectedValue(InputValue1));
	for (int n = 0; n < 1e6; n++) {
		plantTick();
		LONGS_EQUAL(getPlantOut(), InputValue2);
	}
}

float stepValueThroughDelayWithExpectedValue(float expectedValueWhileStepping) {
	for (int n = 0; n < ExpectedDelayLength; n++) {
		plantTick();
		float plantOut = getPlantOut();
		if (plantOut != expectedValueWhileStepping) {
			return false;
		}
	}
	return true;
}
