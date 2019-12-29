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

unsigned char writeByteToClient_buffer[256];
unsigned char writeByteToClient_bufferCount;

static bool byteFromClientAvailable(void) {
	mock_c()->actualCall("byteFromClientAvailable");
	return mock_c()->returnValue().value.boolValue;
}

static ItError_t readByteFromClient(char* const data) {
	mock_c()->actualCall("readByteFromClient")->withOutputParameter("data", data);
#pragma warning(push)
#pragma warning(disable : 26812)
	return (ItError_t)mock_c()->returnValue().value.intValue;
#pragma warning(pop) 
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

static unsigned long getTimeStamp(void) {
	mock_c()->actualCall("getTimeStamp");
	return mock_c()->returnValue().value.intValue;
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

static ItError_t parseCommand_Mock(const char* const command, ItCommandResult_t* result) {
	mock_c()->actualCall("parseCommand_Mock")
		->withPointerParameters("command", (void *)command);
		//->withStringParameters("command", command);
		//->withOutputParameter("result", NULL);
	*result = *((ItCommandResult_t*)mock_c()->getData("result").value.pointerValue);
	return (ItError_t)mock_c()->returnValue().value.intValue;
}

const unsigned char ItInputBufferSize = 30;
char itInputBuffer[ItInputBufferSize];

TEST_GROUP(ItTest) {

	void setup() {
		for (int n = 0; n < ItInputBufferSize; n++) {
			itInputBuffer[n] = '\0';
		}

		writeByteToClient_bufferCount = 0;

		UT_PTR_SET(parseCommand, parseCommand_Mock);

		ItCallbacks_t callbacks;
		callbacks.byteFromClientAvailable = byteFromClientAvailable;
		callbacks.readByteFromClient = readByteFromClient;
		callbacks.writeByteToClient = writeByteToClient;
		callbacks.getTimestamp = getTimeStamp;
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

TEST(ItTest, byteFromClientAvailable_called) {
	mock().expectOneCall("byteFromClientAvailable").andReturnValue(false);
	itTick();
}

TEST(ItTest, appendToBuffer) {
	char readByteFromClient_buffer[] = { 'x', 'A' };

	mock().strictOrder();

	mock().expectOneCall("byteFromClientAvailable").andReturnValue(true);
	mock().expectOneCall("readByteFromClient")
		.withOutputParameterReturning("data", &readByteFromClient_buffer[0], sizeof(readByteFromClient_buffer[0]))
		.andReturnValue((int)ItError_NoError);

	mock().expectOneCall("byteFromClientAvailable").andReturnValue(true);
	mock().expectOneCall("readByteFromClient")
		.withOutputParameterReturning("data", &readByteFromClient_buffer[1], sizeof(readByteFromClient_buffer[0]))
		.andReturnValue((int)ItError_NoError);

	mock().expectOneCall("byteFromClientAvailable").andReturnValue(false);

	itTick();

	LONGS_EQUAL('x', itInputBuffer[0]);
	LONGS_EQUAL('A', itInputBuffer[1]);
}

TEST(ItTest, readByteButClientUnavailable) {
	char readByteFromClient_buffer[] = { 'b', 'Z' };

	mock().strictOrder();

	mock().expectOneCall("byteFromClientAvailable").andReturnValue(true);
	mock().expectOneCall("readByteFromClient")
		.withOutputParameterReturning("data", &readByteFromClient_buffer[0], sizeof(readByteFromClient_buffer[0]))
		.andReturnValue((int)ItError_NoError);

	mock().expectOneCall("byteFromClientAvailable").andReturnValue(true);
	mock().expectOneCall("readByteFromClient")
		.withOutputParameterReturning("data", &readByteFromClient_buffer[1], sizeof(readByteFromClient_buffer[0]))
		.andReturnValue((int)ItError_ClientUnavailable);

	itTick();

	LONGS_EQUAL('b', itInputBuffer[0]);
	LONGS_EQUAL('\0', itInputBuffer[1]);
}

TEST(ItTest, tooMuchInput) {
	//no string-terminator
	mock().strictOrder();
	char readByteFromClient_buffer[] = { '1','2','3','4','5','6','7','8','9','_','1','2','3','4','5','6','7','8','9','_','1','2','3','4','5','6','7','8','9','_','\r' };
	for (unsigned char n = 0; n < 31; n++) {
		mock().expectOneCall("byteFromClientAvailable").andReturnValue(true);
		mock().expectOneCall("readByteFromClient")
			.withOutputParameterReturning("data", &readByteFromClient_buffer[n], sizeof(readByteFromClient_buffer[0]));
	}

	mock().expectNCalls(33, "writeByteToClient").andReturnValue(ItError_NoError);
	ItCommandResult_t dummy;
	mock().setData("result", &dummy);
	mock().expectOneCall("parseCommand_Mock").withPointerParameter("command", itInputBuffer);
	mock().expectNCalls(32, "writeByteToClient").andReturnValue(ItError_NoError);

	mock().expectOneCall("byteFromClientAvailable").andReturnValue(false);

	itTick();

	const unsigned char ExpectedTelegram[] = {0xAA, 0x02, 'E','r','r','o','r',':',' ','I','n','p','u','t',' ','B','u','f','f','e','r',' ','i','s',' ','f','u','l','l','!','\n','\0', 0xBB};
	for (unsigned char n = 0; n < sizeof(ExpectedTelegram); n++) {
		LONGS_EQUAL(ExpectedTelegram[n], writeByteToClient_buffer[n]);
	}
}

TEST(ItTest, parseCommand) {
	//no string-terminator
	mock().strictOrder();
	char readByteFromClient_buffer[] = "myCommand\r";
	for (unsigned char n = 0; n < strlen(readByteFromClient_buffer); n++) {
		mock().expectOneCall("byteFromClientAvailable").andReturnValue(true);
		mock().expectOneCall("readByteFromClient")
			.withOutputParameterReturning("data", &readByteFromClient_buffer[n], sizeof(readByteFromClient_buffer[0]));
	}

	ItCommandResult_t result;
	result.name = "myName";
	result.valueType = ItValueType_Float;
	result.resultFloat = 32.8f;
	mock().setData("result", &result);
	mock().expectOneCall("parseCommand_Mock")
		.withPointerParameter("command", itInputBuffer)
		.andReturnValue((int)ItError_NoError);
	mock().expectOneCall("getTimeStamp")
		.andReturnValue(5263);
	mock().expectNCalls(19, "writeByteToClient").andReturnValue(ItError_NoError);

	mock().expectOneCall("byteFromClientAvailable").andReturnValue(false);

	itTick();

	const unsigned char ExpectedTelegram[] = { 0xAA, 0x01, 'm','y','N','a','m','e','\0', 4, 0x33, 0x33, 0x03, 0x42, 0x8F, 0x14, 0x00, 0x00, 0xBB };
	for (unsigned char n = 0; n < sizeof(ExpectedTelegram); n++) {
		LONGS_EQUAL(ExpectedTelegram[n], writeByteToClient_buffer[n]);
	}

	for (unsigned char n = 0; n < sizeof(itInputBuffer); n++) {
		LONGS_EQUAL('\0', itInputBuffer[n]);
	}
}

