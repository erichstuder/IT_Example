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
extern "C" {
#include "it.h"
}


unsigned long timestamp;

ItError_t readByteFromClient_error;
char readByteFromClient_buffer[256];
unsigned char readByteFromClient_bytesLeft;

unsigned char writeByteToClient_buffer[256];
unsigned char writeByteToClient_bufferCount;

static bool byteFromClientAvailable(void) {
	mock_c()->actualCall("byteFromClientAvailable");
	return readByteFromClient_bytesLeft > 0;
}

static ItError_t readByteFromClient(char* const data) {
	mock_c()->actualCall("readByteFromClient")->withOutputParameter("data", data);
	if (readByteFromClient_bytesLeft > 0)
	{
		readByteFromClient_bytesLeft--;
		*data = readByteFromClient_buffer[readByteFromClient_bytesLeft];
#pragma warning(push)
#pragma warning(disable : 26812)
		return ItError_NoError;
#pragma warning(pop) 
	}
	else
	{
		return ItError_NoDataAvailable;
	}
}

static ItError_t writeByteToClient(const unsigned char data) {
	mock_c()->actualCall("writeByteToClient");
	writeByteToClient_buffer[writeByteToClient_bufferCount] = data;
	writeByteToClient_bufferCount++;
#pragma warning(push)
#pragma warning(disable : 26812)
	return ItError_NoError;
#pragma warning(pop)
}

/*static ItError_t cmdHandler_Spy(ItCommandResult_t* result) {
	if (readByteFromClient_buffer[1] == 'A') {
#pragma warning(push)
#pragma warning(disable : 26812)
		result->valueType = ItValueType_Uint8;
#pragma warning(pop) 
		result->resultInt8 = 0x45;
	}
	else if (readByteFromClient_buffer[1] == 'B') {
		result->valueType = ItValueType_Float;
		result->resultFloat = 1.26f;
	}
	else {
		result->valueType = ItValueType_Int8;
		result->resultInt8 = 0x11;
	}
	return ItError_NoError;
}*/

static unsigned long getTimeStamp(void) {
	return timestamp;
}

static signed char int8Function(void) {
	return 0x11;
}

static unsigned char uint8Function(void) {
	return 0x45;
}

static float floatFunction(void) {
	return 1.26f;
}

static ItCommandError_t parseCommand_Mock(const char* const command, ItCommandResult_t* result) {
	mock_c()->actualCall("parseCommand_Mock");
		//->withStringParameters("command", "")
		//->withOutputParameter("result", NULL);
	result = NULL;
#pragma warning(push)
#pragma warning(disable : 26812)
	return ItCommandError_UnknownCommand;
#pragma warning(pop)
}

const unsigned char ItInputBufferSize = 30;
char itInputBuffer[ItInputBufferSize];

TEST_GROUP(ItTest) {

	void setup() {
		UT_PTR_SET(parseCommand, parseCommand_Mock);

		ItCallbacks_t callbacks;
		callbacks.byteFromClientAvailable = byteFromClientAvailable;
		callbacks.readByteFromClient = readByteFromClient;
		callbacks.writeByteToClient = writeByteToClient;
		/*callbacks.getTimestamp = getTimeStamp;*/
		ItParameters_t itParameters;
		itParameters.itInputBuffer = itInputBuffer;
		itParameters.itInputBufferSize = ItInputBufferSize;
		/*itParameters.itSignals = itSignals;
		itParameters.itSignalCount = ItSignalCount;*/
		itInit(&itParameters, &callbacks);
	}

	void teardown() {
		mock().checkExpectations();
		mock().clear();
	}
};

/*void checkMocksMinimal(char* const readByteFromClient_buffer) {
	mock().expectNCalls(2, "byteFromClientAvailable");
	mock().expectOneCall("readByteFromClient")
		.withOutputParameterReturning("data", readByteFromClient_buffer, sizeof(readByteFromClient_buffer[0]));
}*/

/*class ItTest : public ::testing::Test {
protected:

	ItTest() {}

	virtual ~ItTest() {}

	virtual void SetUp() {
		static ItSignal_t itSignals[] = {
			{
				"TestCommand",
				ItValueType_Int8,
				(void (*)(void)) int8Function,
				NULL,
			},
			{
				"A",
				ItValueType_Uint8,
				(void (*)(void)) uint8Function,
				NULL,
			},
			{
				"B",
				ItValueType_Float,
				(void (*)(void)) floatFunction,
				NULL,
			},
		};
		static const unsigned char ItSignalCount = sizeof(itSignals) / sizeof(itSignals[0]);


		ItCallbacks_t callbacks;
		callbacks.byteFromClientAvailable = byteFromClientAvailable_Spy;
		callbacks.readByteFromClient = readByteFromClient_Spy;
		callbacks.writeByteToClient = writeByteToClient_Spy;
		callbacks.getTimestamp = getTimeStamp;
		ItParameters_t itParameters;
		itParameters.itInputBuffer = itInputBuffer;
		itParameters.itInputBufferSize = ItInputBufferSize;
		itParameters.itSignals = itSignals;
		itParameters.itSignalCount = ItSignalCount;

		itInit(&itParameters, &callbacks);

		readByteFromClient_done = false;
		readByteFromClient_bytesLeft = 0;
		readByteFromClient_error = ItError_NoError;

		writeByteToClient_bufferCount = 0;
	}

	virtual void TearDown() {
		for (unsigned char n = 0; n < ItInputBufferSize; n++) {
			itInputBuffer[n] = '\0';
		}
		
	}
};*/

TEST(ItTest, byteFromClientAvailable_called) {
	readByteFromClient_bytesLeft = 0;
	mock().expectOneCall("byteFromClientAvailable").andReturnValue(false);
	itTick();
}

TEST(ItTest, readByteFromClient_called) {
	readByteFromClient_buffer[0] = 'x';
	readByteFromClient_bytesLeft = 1;
	mock().expectNCalls(2, "byteFromClientAvailable").andReturnValue(false);
	mock().expectOneCall("readByteFromClient")
		.withOutputParameterReturning("data", readByteFromClient_buffer, sizeof(readByteFromClient_buffer[0]))
		.andReturnValue(ItError_NoError);
	itTick();
}

TEST(ItTest, appendToBuffer) {
	readByteFromClient_buffer[0] = 'E';
	readByteFromClient_bytesLeft = 1;
	mock().expectNCalls(2, "byteFromClientAvailable");
	mock().expectOneCall("readByteFromClient")
		.withOutputParameterReturning("data", readByteFromClient_buffer, sizeof(readByteFromClient_buffer[0]));
	LONGS_EQUAL(itInputBuffer[0], '\0');
	itTick();
	LONGS_EQUAL(itInputBuffer[0], 'E');
}

TEST(ItTest, tooMuchInput) {
	//no string-terminator
	const char TestCommand[] = { '1','2','3','4','5','6','7','8','9','_','1','2','3','4','5','6','7','8','9','_','1','2','3','4','5','6','7','8','9','_','\r' };
	for (unsigned char n = 0; n < sizeof(TestCommand); n++) {
		readByteFromClient_buffer[n] = TestCommand[sizeof(TestCommand) - 1 - n]; //fill the other way round
	}
	readByteFromClient_bytesLeft = sizeof(TestCommand) / sizeof(TestCommand[0]);

	mock().expectNCalls(32, "byteFromClientAvailable");
	mock().expectNCalls(31, "readByteFromClient")
		.withOutputParameterReturning("data", readByteFromClient_buffer, sizeof(readByteFromClient_buffer[0]));
	mock().expectOneCall("parseCommand_Mock");
	mock().expectNCalls(57, "writeByteToClient").andReturnValue(ItError_NoError);

	itTick();

	const unsigned char ExpectedTelegram[] = {'E','r','r','o','r',':',' ','I','n','p','u','t',' ','B','u','f','f','e','r',' ','i','s',' ','f','u','l','l','!','\n'};
	for (unsigned char n = 0; n < sizeof(ExpectedTelegram); n++) {
		LONGS_EQUAL(writeByteToClient_buffer[n], ExpectedTelegram[n]);
	}
}

//TODO: add tests for correct telegram generation. => or do telegram generation in separated modul
/*TEST(ItTest, handleCmdInt8) {
	const unsigned char TestCommand[] = { 'T', 'e', 's', 't', 'C', 'o', 'm', 'm', 'a', 'n', 'd', '\r' }; //no string-terminator
	for (unsigned char n = 0; n < sizeof(TestCommand); n++) {
		readByteFromClient_buffer[n] = TestCommand[sizeof(TestCommand)-1-n]; //fill the other way round
	}
	readByteFromClient_bytesLeft = sizeof(TestCommand) / sizeof(TestCommand[0]);

	timestamp = 0xAABBCCDD;

	itTick();
	const unsigned char ExpectedTelegram[] = { 0xAA, 0x01, 'T', 'e', 's', 't', 'C', 'o', 'm', 'm', 'a', 'n', 'd', 0x00, 0x01, 0x11, 0xDD, 0xCC, 0xCB, 0xCC, 0xBA, 0xCC, 0xA9, 0xBB };
	for (unsigned char n = 0; n < sizeof(ExpectedTelegram); n++) {
		LONGS_EQUAL(writeByteToClient_buffer[n], ExpectedTelegram[n]);
	}
}

TEST(ItTest, handleCmdUint8) {
	readByteFromClient_buffer[0] = '\r';
	readByteFromClient_buffer[1] = 'A';
	readByteFromClient_bytesLeft = 2;

	timestamp = 0x12345678;

	itTick();
	const unsigned char ExpectedTelegram[] = { 0xAA, 0x01, 'A', 0x00, 0x02, 0x45, 0x78, 0x56, 0x34, 0x12, 0xBB };
	for (unsigned char n = 0; n < sizeof(ExpectedTelegram); n++) {
		LONGS_EQUAL(writeByteToClient_buffer[n], ExpectedTelegram[n]);
	}
}

TEST(ItTest, handleCmdFloat) {
	readByteFromClient_buffer[0] = '\r';
	readByteFromClient_buffer[1] = 'B';
	readByteFromClient_bytesLeft = 2;

	timestamp = 0xAA99CCDD;

	itTick();
	const unsigned char ExpectedTelegram[] = { 0xAA, 0x01, 'B', 0x00, 0x04, 0xAE, 0x47, 0XA1, 0x3F, 0xDD, 0xCC, 0xCB, 0x99, 0xCC, 0xA9, 0xBB };
	for (unsigned char n = 0; n < sizeof(ExpectedTelegram); n++) {
		LONGS_EQUAL(writeByteToClient_buffer[n], ExpectedTelegram[n]);
	}
}*/
