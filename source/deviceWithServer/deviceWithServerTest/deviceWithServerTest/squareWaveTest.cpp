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
