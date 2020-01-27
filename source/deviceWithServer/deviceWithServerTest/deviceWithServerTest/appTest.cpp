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
#include "CppUTestExt/MockSupport.h"
#include "CppUTestExt/MockSupport_c.h"
#include "app.h"
#include "squareWave.h"
#include "controller.h"
#include "plant.h"

extern "C" {
#include "it.h"
}

static void setSquareWaveTickTime_Mock(float time) {
	mock_c()->actualCall("setSquareWaveTickTime_Mock")
		->withDoubleParameters("time", time);
}

static void setSquareWaveFrequency_Mock(float signalFrequency) {
	mock_c()->actualCall("setSquareWaveFrequency_Mock")
		->withDoubleParameters("signalFrequency", signalFrequency);
}

static void setSquareWaveLevels_Mock(float level1, float level2) {
	mock_c()->actualCall("setSquareWaveLevels_Mock")
		->withDoubleParameters("level1", level1)
		->withDoubleParameters("level2", level2);
}

static float getSquareWaveSignal_Mock(void) {
	mock_c()->actualCall("getSquareWaveSignal_Mock");
	return (float)mock_c()->returnValue().value.doubleValue;
}

static void squareWaveTick_Mock(void) {
	mock_c()->actualCall("squareWaveTick_Mock");
}


static void setControllerKp_Mock(float value) {
	mock_c()->actualCall("setControllerKp_Mock")
		->withDoubleParameters("value", value);
}

static void setControllerKi_Mock(float value) {
	mock_c()->actualCall("setControllerKi_Mock")
		->withDoubleParameters("value", value);
}

static void setControllerDesiredValue_Mock(float value) {
	mock_c()->actualCall("setControllerDesiredValue_Mock")
		->withDoubleParameters("value", value);
}

static void setControllerActualValue_Mock(float value) {
	mock_c()->actualCall("setControllerActualValue_Mock")
		->withDoubleParameters("value", value);
}

static float getControllerSignal_Mock(void) {
	mock_c()->actualCall("getControllerSignal_Mock");
	return (float)mock_c()->returnValue().value.doubleValue;
}

static void controllerTick_Mock(void) {
	mock_c()->actualCall("controllerTick_Mock");
}


static float getPlantOut_Mock(void) {
	mock_c()->actualCall("getPlantOut_Mock");
	return (float)mock_c()->returnValue().value.doubleValue;
}

static void setPlantIn_Mock(float value) {
	mock_c()->actualCall("setPlantIn_Mock")
		->withDoubleParameters("value", value);
}

static void plantTick_Mock(void) {
	mock_c()->actualCall("plantTick_Mock");
}


static void itInit_Mock(ItParameters_t* parameters, ItCallbacks_t* callbacks) {
	mock_c()->actualCall("itInit_Mock");
	
	CHECK(parameters != NULL);
	if (parameters != NULL) {
		CHECK(parameters->itInputBuffer != NULL);
		CHECK(parameters->itInputBufferSize != 0);
		CHECK(parameters->itSignals != NULL);
		CHECK(parameters->itSignalCount != 0);

		if (parameters->itSignals != NULL) {
			for (int n = 0; n < parameters->itSignalCount; n++) {
				CHECK(parameters->itSignals[n].name != NULL);
				CHECK(parameters->itSignals[n].getter != NULL);
			}
		}
	}

	CHECK(callbacks != NULL);
	//TODO: check if callback functions are the same as expected
}

static void itTick_Mock(void) {
	mock_c()->actualCall("itTick_Mock");
}

TEST_GROUP(AppTest) {
	void setup() {
	}

	void teardown() {
		mock().checkExpectations();
		mock().clear();
	}
};

TEST(AppTest, appInitSettings) {
	UT_PTR_SET(setSquareWaveTickTime, setSquareWaveTickTime_Mock);
	mock().expectOneCall("setSquareWaveTickTime_Mock").withParameter("time", 1.0);

	UT_PTR_SET(setSquareWaveFrequency, setSquareWaveFrequency_Mock);
	mock().expectOneCall("setSquareWaveFrequency_Mock").withParameter("signalFrequency", 0.2);

	UT_PTR_SET(setSquareWaveLevels, setSquareWaveLevels_Mock);
	mock().expectOneCall("setSquareWaveLevels_Mock")
		.withParameter("level1", 2.0)
		.withParameter("level2", 10.0);

	UT_PTR_SET(setControllerKp, setControllerKp_Mock);
	mock().expectOneCall("setControllerKp_Mock").withParameter("value", 1.0);

	UT_PTR_SET(setControllerKi, setControllerKi_Mock);
	mock().expectOneCall("setControllerKi_Mock").withParameter("value", 1.0f);

	AppCallbacks_t dummy = { NULL, NULL, NULL, NULL };
	appInit(dummy);
}

TEST(AppTest, appTickWiring) {
	UT_PTR_SET(getSquareWaveSignal, getSquareWaveSignal_Mock);
	UT_PTR_SET(setControllerDesiredValue, setControllerDesiredValue_Mock);
	const float SquareWaveSignal = 5.0f;
	mock().expectOneCall("getSquareWaveSignal_Mock").andReturnValue(SquareWaveSignal);
	mock().expectOneCall("setControllerDesiredValue_Mock").withParameter("value", SquareWaveSignal);

	UT_PTR_SET(getPlantOut, getPlantOut_Mock);
	UT_PTR_SET(setControllerActualValue, setControllerActualValue_Mock);
	const float PlantOut = 7.0f;
	mock().expectOneCall("getPlantOut_Mock").andReturnValue(PlantOut);
	mock().expectOneCall("setControllerActualValue_Mock").withParameter("value", PlantOut);

	UT_PTR_SET(getControllerSignal, getControllerSignal_Mock);
	UT_PTR_SET(setPlantIn, setPlantIn_Mock);
	const float ControllerSignal = 13.0f;
	mock().expectOneCall("getControllerSignal_Mock").andReturnValue(ControllerSignal);
	mock().expectOneCall("setPlantIn_Mock").withParameter("value", ControllerSignal);

	//itTick is mocked here to prevent Null-Pointer Exception
	UT_PTR_SET(itTick, itTick_Mock);
	mock().expectOneCall("itTick_Mock");

	appTick();
}

TEST(AppTest, appTickFunctions) {
	UT_PTR_SET(squareWaveTick, squareWaveTick_Mock);
	mock().expectOneCall("squareWaveTick_Mock");
	UT_PTR_SET(controllerTick, controllerTick_Mock);
	mock().expectOneCall("controllerTick_Mock");
	UT_PTR_SET(plantTick, plantTick_Mock);
	mock().expectOneCall("plantTick_Mock");
	UT_PTR_SET(itTick, itTick_Mock);
	mock().expectOneCall("itTick_Mock");

	appTick();
}

TEST(AppTest, itInitCalled) {
	UT_PTR_SET(itInit, itInit_Mock);
	mock().expectOneCall("itInit_Mock");
	AppCallbacks_t dummy = { NULL, NULL, NULL, NULL };
	appInit(dummy);
}
