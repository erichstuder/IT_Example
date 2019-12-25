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

#include "app.h"
#include "squareWave.h"
#include "controller.h"
#include "plant.h"

extern "C" {
#include "it.h"
}

float squareWaveTickTime;
float squareWaveFrequency;
float squareWaveLevel1;
float squareWaveLevel2;
bool squareWaveTickDone;
float controllerKp;
float controllerKi;
float controllerDesiredValue;
float controllerActualValue;
bool controllerTickDone;
float plantIn;
bool plantTickDone;
unsigned char byteToReadFromClient;
unsigned char nrOfBytesToReadFromClient;

ByteFromClientAvailable_t byteFromClientAvailable;
ReadByteFromClient_t readByteFromClient;
WriteByteToClient_t writeByteToClient;

ByteFromUartAvailable_t byteFromUartAvailable;
ReadByteFromUart_t readByteFromUart;
WriteByteToUart_t writeByteToUart;

bool itTickDone;

static void setSquareWaveTickTime_Spy(float time) {
	squareWaveTickTime = time;
}

static void setSquareWaveFrequency_Spy(float signalFrequency) {
	squareWaveFrequency = signalFrequency;
}

static void setSquareWaveLevels_Spy(float level1, float level2) {
	squareWaveLevel1 = level1;
	squareWaveLevel2 = level2;
}

static float getSquareWaveSignal_Fake(void) {
	return 5.0f;
}

static void squareWaveTick_Spy(void) {
	squareWaveTickDone = true;
}


static void setControllerKp_Spy(float value) {
	controllerKp = value;
}

static void setControllerKi_Spy(float value) {
	controllerKi = value;
}

static void setControllerDesiredValue_Spy(float value) {
	controllerDesiredValue = value;
}

static void setControllerActualValue_Spy(float value) {
	controllerActualValue = value;
}

static float getControllerSignal_Fake(void) {
	return 13.0f;
}

static void controllerTick_Spy(void) {
	controllerTickDone = true;
}


static float getPlantOut_Fake(void) {
	return 7.0f;
}

static void setPlantIn_Spy(float value) {
	plantIn = value;
}

static void plantTick_Spy(void) {
	plantTickDone = true;
}


static bool byteFromUartAvailable_Spy(void) {
	return true;
}

static AppError readByteFromUart_Spy(unsigned char* const data) {
	if (nrOfBytesToReadFromClient > 0) {
		*data = byteToReadFromClient;
		nrOfBytesToReadFromClient--;
#pragma warning(push)
#pragma warning(disable : 26812)
		return AppError::NoError;
#pragma warning(pop)
	}
	else {
		return AppError::NoDataAvailable;
	}
}

/*static AppError_t writeByteToUart_Spy(const unsigned char data) {
#pragma warning(push)
#pragma warning(disable : 26812)
	return AppError_NoError;
#pragma warning(pop)
}*/


static unsigned long millis_Fake(void) {
	return 33;
}

static void itInit_Spy(ItParameters_t* parameters, ItCallbacks_t* callbacks) {
	byteFromClientAvailable = callbacks->byteFromClientAvailable;
	readByteFromClient = callbacks->readByteFromClient;
	writeByteToClient = callbacks->writeByteToClient;
}

static void itTick_Spy(void) {
	itTickDone = true;
}


class AppTest : public ::testing::Test {
protected:
	void (*setSquareWaveTickTime_Original)(float time) = NULL;
	void (*setSquareWaveFrequency_Original)(float signalFrequency) = NULL;
	void (*setSquareWaveLevels_Original)(float level1, float level2) = NULL;
	float (*getSquareWaveSignal_Original)(void) = NULL;
	void (*squareWaveTick_Original)(void) = NULL;

	void (*setControllerKp_Original)(float value) = NULL;
	void (*setControllerKi_Original)(float value) = NULL;
	void (*setControllerDesiredValue_Original)(float value) = NULL;
	void (*setControllerActualValue_Original)(float value) = NULL;
	float (*getControllerSignal_Original)(void) = NULL;
	void (*controllerTick_Original)(void) = NULL;

	float (*getPlantOut_Original)(void) = NULL;
	void (*setPlantIn_Original)(float value) = NULL;
	void (*plantTick_Original)(void) = NULL;

	void (*itInit_Original)(ItParameters_t* parameters, ItCallbacks_t* callbacks) = NULL;
	void (*itTick_Original)(void) = NULL;

	AppTest() {}

	virtual ~AppTest() {}

	virtual void SetUp() {
		squareWaveTickTime = 0.0f;
		squareWaveFrequency = 0.0f;
		squareWaveLevel1 = 0.0f;
		squareWaveLevel2 = 0.0f;
		squareWaveTickDone = false;
		controllerKp = 0.0f;
		controllerKi = 0.0f;
		controllerDesiredValue = 0.0f;
		controllerActualValue = 0.0f;
		controllerTickDone = false;
		plantIn = 0.0f;
		plantTickDone = false;
		itTickDone = false;

		setSquareWaveTickTime_Original = setSquareWaveTickTime;
		setSquareWaveTickTime = setSquareWaveTickTime_Spy;

		setSquareWaveFrequency_Original = setSquareWaveFrequency;
		setSquareWaveFrequency = setSquareWaveFrequency_Spy;

		setSquareWaveLevels_Original = setSquareWaveLevels;
		setSquareWaveLevels = setSquareWaveLevels_Spy;

		getSquareWaveSignal_Original = getSquareWaveSignal;
		getSquareWaveSignal = getSquareWaveSignal_Fake;

		squareWaveTick_Original = squareWaveTick;
		squareWaveTick = squareWaveTick_Spy;
		

		setControllerKp_Original = setControllerKp;
		setControllerKp = setControllerKp_Spy;

		setControllerKi_Original = setControllerKi;
		setControllerKi = setControllerKi_Spy;

		setControllerDesiredValue_Original = setControllerDesiredValue;
		setControllerDesiredValue = setControllerDesiredValue_Spy;

		setControllerActualValue_Original = setControllerActualValue;
		setControllerActualValue = setControllerActualValue_Spy;

		getControllerSignal_Original = getControllerSignal;
		getControllerSignal = getControllerSignal_Fake;

		controllerTick_Original = controllerTick;
		controllerTick = controllerTick_Spy;


		getPlantOut_Original = getPlantOut;
		getPlantOut = getPlantOut_Fake;

		setPlantIn_Original = setPlantIn;
		setPlantIn = setPlantIn_Spy;

		plantTick_Original = plantTick;
		plantTick = plantTick_Spy;


		itInit_Original = itInit;
		itInit = itInit_Spy;

		itTick_Original = itTick;
		itTick = itTick_Spy;

		byteFromUartAvailable = NULL;
		readByteFromUart = NULL;
		writeByteToUart = NULL;

		byteFromClientAvailable = NULL;
		readByteFromClient = NULL;
		writeByteToClient = NULL;
	}

	virtual void TearDown() {
		setSquareWaveTickTime = setSquareWaveTickTime_Original;
		setSquareWaveFrequency = setSquareWaveFrequency_Original;
		setSquareWaveLevels = setSquareWaveLevels_Original;
		getSquareWaveSignal = getSquareWaveSignal_Original;
		squareWaveTick = squareWaveTick_Original;

		setControllerKp = setControllerKp_Original;
		setControllerKi = setControllerKi_Original;
		setControllerDesiredValue = setControllerDesiredValue_Original;
		setControllerActualValue = setControllerActualValue_Original;
		getControllerSignal = getControllerSignal_Original;
		controllerTick = controllerTick_Original;

		getPlantOut = getPlantOut_Original;
		setPlantIn = setPlantIn_Original;
		plantTick = plantTick_Original;

		itInit = itInit_Original;
		itTick = itTick_Original;
	}
};

TEST_F(AppTest, appInitSettings) {
	ASSERT_EQ(squareWaveTickTime, 0.0f);
	ASSERT_EQ(squareWaveFrequency, 0.0f);
	ASSERT_EQ(squareWaveLevel1, 0.0f);
	ASSERT_EQ(squareWaveLevel2, 0.0f);
	ASSERT_EQ(controllerKp, 0.0f);
	ASSERT_EQ(controllerKi, 0.0f);
	AppCallbacks_t dummy = { NULL, NULL, NULL, NULL };
	appInit(dummy);
	ASSERT_EQ(squareWaveTickTime, 1);
	ASSERT_EQ(squareWaveFrequency, 0.2f);
	ASSERT_EQ(squareWaveLevel1, 2.0f);
	ASSERT_EQ(squareWaveLevel2, 10.0f);
	ASSERT_EQ(controllerKp, 1.0f);
	ASSERT_EQ(controllerKi, 1.0f);
}

TEST_F(AppTest, appTickWiring) {
	ASSERT_EQ(controllerDesiredValue, 0.0f);
	ASSERT_EQ(controllerActualValue, 0.0f);
	ASSERT_EQ(plantIn, 0.0f);
	AppCallbacks_t callbacks = { NULL, NULL, NULL, NULL };
	appInit(callbacks);
	appTick();
	ASSERT_EQ(controllerDesiredValue, 5.0f);
	ASSERT_EQ(controllerActualValue, 7.0f);
	ASSERT_EQ(plantIn, 13.0f);
}

TEST_F(AppTest, appTickFunctions) {
	ASSERT_FALSE(squareWaveTickDone);
	ASSERT_FALSE(controllerTickDone);
	ASSERT_FALSE(plantTickDone);
	ASSERT_FALSE(itTickDone);
	AppCallbacks_t callbacks = { NULL, NULL, NULL, NULL };
	appInit(callbacks);
	appTick();
	ASSERT_TRUE(squareWaveTickDone);
	ASSERT_TRUE(controllerTickDone);
	ASSERT_TRUE(plantTickDone);
	ASSERT_TRUE(itTickDone);
}

TEST_F(AppTest, itInitCalled) {
	ASSERT_TRUE(byteFromUartAvailable == NULL);
	ASSERT_TRUE(readByteFromClient == NULL);
	ASSERT_TRUE(writeByteToClient == NULL);
	AppCallbacks_t callbacks;
	callbacks.byteFromUartAvailable = byteFromUartAvailable_Spy;
	callbacks.readByteFromUart = NULL;
	callbacks.writeByteToUart = NULL;
	callbacks.getCurrentMillis = NULL;
	appInit(callbacks);
	ASSERT_TRUE(byteFromClientAvailable == byteFromUartAvailable_Spy);
	ASSERT_TRUE(readByteFromClient != NULL);
	ASSERT_TRUE(writeByteToClient != NULL);
}

/*TEST(AppTest_withIt, handleCmd) {
	AppCallbacks_t callbacks;
	callbacks.byteFromUartAvailable = byteFromUartAvailable_Spy;
	callbacks.readByteFromUart = readByteFromUart_Spy;
	callbacks.writeByteToUart = NULL;
	callbacks.getCurrentMillis = millis_Fake;
	appInit(callbacks);

	char cmd[] = "desiredValue";
#pragma warning(push)
#pragma warning(disable : 26812)
	ItError_t err;
#pragma warning(pop) 
	for (int n = 0; n < strlen(cmd); n++) {
		byteToReadFromClient = cmd[n];
		nrOfBytesToReadFromClient = 1;
		appTick();
	}

	ItCommandResult_t result;
	err = itCmdHandler(&result);
	ASSERT_EQ(err, ItError_NoError);
	ASSERT_EQ(result.valueType, ValueType_Float);
	ASSERT_EQ(result.valueFloat, 10);
	ASSERT_EQ(result.timestamp, 33);
}*/

/*TEST(AppTest_withIt, handleInvalidCmd) {
	AppCallbacks_t callbacks;
	callbacks.byteFromUartAvailable = byteFromUartAvailable_Spy;
	callbacks.readByteFromUart = readByteFromUart_Spy;
	callbacks.writeByteToUart = NULL;
	callbacks.getCurrentMillis = NULL;
	appInit(callbacks);

	char cmd[] = "desiredValue42";
	ItError_t err;
	for (int n = 0; n < strlen(cmd); n++) {
		byteToReadFromClient = cmd[n];
		nrOfBytesToReadFromClient = 1;
		appTick();
	}

	ItCommandResult_t result;
	err = itCmdHandler(&result);
	ASSERT_EQ(err, ItError_InvalidCommand);
	ASSERT_EQ(result.timestamp, 0);
}*/
