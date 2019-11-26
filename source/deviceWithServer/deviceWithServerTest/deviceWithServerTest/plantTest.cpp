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
#include "plant.h"

float stepValueThroughDelayWithExpectedValue(float expectedValueWhileStepping);
const float ExpectedDelayLength = 10.0f;

class PlantTest : public ::testing::Test {
protected:
	PlantTest() {}

	virtual ~PlantTest() {}

	virtual void SetUp() {
		clearDelayBuffer();
	}

	virtual void TearDown() {
		clearDelayBuffer();
	}

	void clearDelayBuffer() {
		setPlantIn(0);
		for (int n = 0; n < ExpectedDelayLength; n++) {
			plantTick();
		}
	}
};

TEST_F(PlantTest, tickFunctionExists) {
	plantTick();
}

TEST_F(PlantTest, plantInExists) {
	setPlantIn((float)20);
}

TEST_F(PlantTest, plantOutInitialValue) {
	ASSERT_EQ(getPlantOut(), 0);
}

TEST_F(PlantTest, delayLength) {
	const float InputValue = 1;
	setPlantIn(InputValue);
	ASSERT_TRUE(stepValueThroughDelayWithExpectedValue(0));
	plantTick();
	ASSERT_EQ(getPlantOut(), InputValue);
}

TEST_F(PlantTest, inputIsPersistent) {
	const float InputValue = 42;
	setPlantIn(InputValue);
	ASSERT_TRUE(stepValueThroughDelayWithExpectedValue(0));
	for (int n = 0; n < 1e6; n++) {
		plantTick();
		ASSERT_EQ(getPlantOut(), InputValue);
	}
}

TEST_F(PlantTest, inputIsChangeable) {
	const float InputValue1 = -12;
	const float InputValue2 = 3564;

	setPlantIn(InputValue1);
	ASSERT_TRUE(stepValueThroughDelayWithExpectedValue(0));
	setPlantIn(InputValue2);
	ASSERT_TRUE(stepValueThroughDelayWithExpectedValue(InputValue1));
	for (int n = 0; n < 1e6; n++) {
		plantTick();
		ASSERT_EQ(getPlantOut(), InputValue2);
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