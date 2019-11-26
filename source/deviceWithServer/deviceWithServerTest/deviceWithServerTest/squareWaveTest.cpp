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
#include "squareWave.h"

/*
- value at start
*/

class SquareWaveTest : public ::testing::Test {
protected:
	SquareWaveTest() {}

	virtual ~SquareWaveTest() {}

	virtual void SetUp() {
		squareWaveReset();
	}

	virtual void TearDown() {
		squareWaveReset();
	}
};

TEST_F(SquareWaveTest, squareWaveTickExists) {
	squareWaveTick();
}

TEST_F(SquareWaveTest, initialSignal) {
	for (int n = 0; n < 1e6; n++) {
		ASSERT_EQ(getSquareWaveSignal(), 0);
		squareWaveTick();
	}
}

TEST_F(SquareWaveTest, setTickTime) {
	setSquareWaveTickTime(0);
}

TEST_F(SquareWaveTest, setSignalFrequency) {
	setSquareWaveFrequency(0);
}

TEST_F(SquareWaveTest, frquency1Hz) {
	setSquareWaveTickTime(0.025);
	setSquareWaveFrequency(1);

	for (int i = 0; i < 1e3; i++) {
		for (int n = 0; n < 20; n++) {
			squareWaveTick();
			ASSERT_EQ(getSquareWaveSignal(), 0);
		}
		for (int n = 0; n < 20; n++) {
			squareWaveTick();
			ASSERT_EQ(getSquareWaveSignal(), 1);
		}
	}
}

TEST_F(SquareWaveTest, amplitude) {
	setSquareWaveLevels(-4, 67);
	setSquareWaveTickTime(0.001);
	setSquareWaveFrequency(250);

	float maximum = FLT_MIN;
	float minimum = FLT_MAX;
	float signal;
	for (int n = 0; n < 1e3; n++) {
		squareWaveTick();
		signal = getSquareWaveSignal();
		maximum = std::max(maximum, signal);
		minimum = std::min(minimum, signal);
	}
	ASSERT_EQ(maximum, 67);
	ASSERT_EQ(minimum, -4);
}

TEST_F(SquareWaveTest, valueAtStart) {
	const float Level1 = -56;
	setSquareWaveLevels(Level1, 2);

	ASSERT_EQ(getSquareWaveSignal(), Level1);
}
