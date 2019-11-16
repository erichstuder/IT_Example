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