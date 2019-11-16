#include "pch.h"
#include "squareWave.h"

/*
TestList:
- Amplitude
*/

TEST(SquareWaveTest, squareWaveTickExists) {
	squareWaveTick();
}

TEST(SquareWaveTest, initialSignal) {
	for (int n = 0; n < 1e6; n++) {
		ASSERT_EQ(getSquareWaveSignal(), 0);
		squareWaveTick();
	}
}

TEST(SquareWaveTest, setTickTime) {
	setSquareWaveTickTime(0);
}

TEST(SquareWaveTest, setSignalFrequency) {
	setSquareWaveFrequency(0);
}

TEST(SquareWaveTest, frquency1Hz) {
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

TEST(SquareWaveTest, amplitude) {
	setSquareWaveAmplitude(56);
	setSquareWaveTickTime(0.001);
	setSquareWaveFrequency(1000);

	float maximum = FLT_MIN;
	float minimum = FLT_MAX;
	float signal;
	for (int n = 0; n < 1e3; n++) {
		squareWaveTick();
		signal = getSquareWaveSignal();
		maximum = std::max(maximum, signal);
		minimum = std::min(minimum, signal);
	}
	ASSERT_EQ(maximum, 56);
	ASSERT_EQ(minimum, 0);
}
