#include "pch.h"
#include "app.h"
#include "squareWave.h"
#include "controller.h"
#include "plant.h"

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
CmdHandler_t cmdHandlerClb;
CmdBufferAppend_t cmdBufferAppendClb;
bool itTickDone;

void setSquareWaveTickTime_Spy(float time) {
	squareWaveTickTime = time;
}

void setSquareWaveFrequency_Spy(float signalFrequency) {
	squareWaveFrequency = signalFrequency;
}

void setSquareWaveLevels_Spy(float level1, float level2) {
	squareWaveLevel1 = level1;
	squareWaveLevel2 = level2;
}

float getSquareWaveSignal_Fake(void) {
	return 5.0f;
}

void squareWaveTick_Spy(void) {
	squareWaveTickDone = true;
}


void setControllerKp_Spy(float value) {
	controllerKp = value;
}

void setControllerKi_Spy(float value) {
	controllerKi = value;
}

void setControllerDesiredValue_Spy(float value) {
	controllerDesiredValue = value;
}

void setControllerActualValue_Spy(float value) {
	controllerActualValue = value;
}

float getControllerSignal_Fake(void) {
	return 13.0f;
}

void controllerTick_Spy(void) {
	controllerTickDone = true;
}


float getPlantOut_Fake(void) {
	return 7.0f;
}

void setPlantIn_Spy(float value) {
	plantIn = value;
}

void plantTick_Spy(void) {
	plantTickDone = true;
}


ItError writeBytesToClient_Spy(const char* const byteArray, const unsigned char byteCount) {
	return ItError::NoError;
}

ItError readByteFromClient_Spy(char* const data) {
	return ItError::NoError;
}


void itInit_Spy(WriteBytesToClient_t writeBytesToClientCallback, ReadByteFromClient_t readByteFromClientCallback, CmdHandler_t cmdHandlerCallback, CmdBufferAppend_t cmdBufferAppendCallback) {
	cmdHandlerClb = cmdHandlerCallback;
	cmdBufferAppendClb = cmdBufferAppendCallback;
}

void itTick_Spy(void) {
	itTickDone = true;
}


class AppTest : public ::testing::Test {
protected:
	//TODO: gibt es dafür nicht irgendwelche MAKROS?
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

	void (*itInit_Original)(WriteBytesToClient_t writeBytesToClientCallback, ReadByteFromClient_t readByteFromClientCallback, CmdHandler_t cmdHandlerCallback, CmdBufferAppend_t cmdBufferAppendCallback) = NULL;
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
	appInit(NULL, NULL);
	ASSERT_EQ(squareWaveTickTime, 1e-3f);
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
	appInit(writeBytesToClient_Spy, readByteFromClient_Spy);
	appTick();
	ASSERT_EQ(controllerDesiredValue, 5.0f);
	ASSERT_EQ(controllerActualValue, 7.0f);
	ASSERT_EQ(plantIn, 13.0f);
}

TEST_F(AppTest, appTickFunctions) {
	ASSERT_FALSE(squareWaveTickDone);
	ASSERT_FALSE(controllerTickDone);
	ASSERT_FALSE(plantTickDone);
	appInit(writeBytesToClient_Spy, readByteFromClient_Spy);
	appTick();
	ASSERT_TRUE(squareWaveTickDone);
	ASSERT_TRUE(controllerTickDone);
	ASSERT_TRUE(plantTickDone);
}

TEST_F(AppTest, itTickIsCalled) {
	ASSERT_FALSE(itTickDone);
	appTick();
	ASSERT_TRUE(itTickDone);
}

TEST_F(AppTest, cmdBufferSize) {
	ASSERT_EQ(IT_CMD_BUFFER_SIZE, 20);
}

TEST_F(AppTest, appendToBufferFull) {
	appInit(NULL, NULL);

	const unsigned char NrOfBytesToSend = 40;
	ItError err[NrOfBytesToSend];
	for (int n = 0; n < NrOfBytesToSend; n++) {
		err[n] = cmdBufferAppendClb(42);
	}
	ASSERT_EQ(err[0], ItError::NoError);
	ASSERT_EQ(err[18], ItError::NoError);
	ASSERT_EQ(err[19], ItError::BufferFull);
	ASSERT_EQ(err[39], ItError::BufferFull);
}

TEST_F(AppTest, handleCmd) {
	appInit(NULL, NULL);

	char cmd[] = "desiredValue";
	ItError err;
	for (int n = 0; n < strlen(cmd); n++) {
		ASSERT_EQ(cmdBufferAppendClb(cmd[n]), ItError::NoError);
	}

	double value;
	err = cmdHandlerClb(&value);
	ASSERT_EQ(err, ItError::NoError);
	ASSERT_EQ(value, 5.0f);
}

TEST_F(AppTest, handleInvalidCmd) {
	appInit(NULL, NULL);

	char cmd[] = "desiredValue42"; //take a valid cmd and add something
	ItError err;
	for (int n = 0; n < strlen(cmd); n++) {
		ASSERT_EQ(cmdBufferAppendClb(cmd[n]), ItError::NoError);
	}

	double value;
	err = cmdHandlerClb(&value);
	ASSERT_EQ(err, ItError::InvalidCommand);
	ASSERT_EQ(value, 0.0f);
}
