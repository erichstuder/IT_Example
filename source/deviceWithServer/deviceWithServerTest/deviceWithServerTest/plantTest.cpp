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

TEST_GROUP(PlantTest) {
	void setup() {
	}

	void teardown() {
	}
};

TEST(PlantTest, inputToOutput) {
	const float InputValue = 42;
	setPlantIn(InputValue);
	plantTick();
	LONGS_EQUAL(getPlantOut(), InputValue);
}

TEST(PlantTest, inputIsChangeable) {
	float InputValue = 22222;
	setPlantIn(InputValue);
	plantTick();
	LONGS_EQUAL(getPlantOut(), InputValue);

	InputValue = -30.932f;
	setPlantIn(InputValue);
	plantTick();
	LONGS_EQUAL(getPlantOut(), InputValue);
}
