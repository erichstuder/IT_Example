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

extern "C" {
#include "it.h"
}

const unsigned char ItCmdBufferSize = 30;
unsigned char itCmdBuffer[ItCmdBufferSize];

bool readByteFromClient_done;
ItError_t readByteFromClient_error;
unsigned char readByteFromClient_buffer[256];
unsigned char readByteFromClient_bytesLeft;
const unsigned char TestCommand[] = { 'T', 'e', 's', 't', 'C', 'o', 'm', 'm', 'a', 'n', 'd', '\r' }; //no string-terminator

unsigned char writeByteToClient_buffer[256];
unsigned char writeByteToClient_bufferCount;

static ItError_t writeByteToClient_Spy(const unsigned char data) {
	writeByteToClient_buffer[writeByteToClient_bufferCount] = data;
	writeByteToClient_bufferCount++;
	return ItError_NoError;
}

static ItError_t readByteFromClient_Spy(unsigned char* const data) {
	readByteFromClient_done = true;

	if (readByteFromClient_bytesLeft)
	{
		readByteFromClient_bytesLeft--;
		*data = readByteFromClient_buffer[readByteFromClient_bytesLeft];
		return ItError_NoError;
	}
	else
	{
		return ItError_NoDataAvailable;
	}
}

static ItError_t cmdHandler_Spy(ItCommandResult_t* result) {
	if (readByteFromClient_buffer[1] == 'A') {
		result->valueType = ValueType_Uint8;
		result->valueInt8 = 0x45;
		result->timestamp = 0x12345678;
	}
	else if (readByteFromClient_buffer[1] == 'B') {
		result->valueType = ValueType_Float;
		result->valueFloat = 1.26f;
		result->timestamp = 0xAA99CCDD;
	}
	else {
		result->valueType = ValueType_Int8;
		result->valueInt8 = 0x11;
		result->timestamp = 0xAABBCCDD;//TOOD: der Datentyp von Windows ist grösser als der vom Arduino!!
	}
	return ItError_NoError;
}


class ItTest : public ::testing::Test {
protected:

	ItTest() {}

	virtual ~ItTest() {}

	virtual void SetUp() {
		itInit(itCmdBuffer, ItCmdBufferSize, cmdHandler_Spy, writeByteToClient_Spy, readByteFromClient_Spy);

		readByteFromClient_done = false;
		readByteFromClient_bytesLeft = 0;
		readByteFromClient_error = ItError_NoError;

		writeByteToClient_bufferCount = 0;
	}

	virtual void TearDown() {
		for (unsigned char n = 0; n < ItCmdBufferSize; n++) {
			itCmdBuffer[n] = '\0';
		}
		
	}
};


TEST_F(ItTest, readByteFromClient_called) {
	ASSERT_EQ(readByteFromClient_done, false);
	itTick();
	ASSERT_EQ(readByteFromClient_done, true);
}

TEST_F(ItTest, appendToBuffer) {

	readByteFromClient_buffer[0] = 'E';
	readByteFromClient_bytesLeft = 1;

	ASSERT_EQ(itCmdBuffer[0], '\0');
	itTick();
	ASSERT_EQ(itCmdBuffer[0], 'E');
}

TEST_F(ItTest, handleCmdInt8) {
	for (unsigned char n = 0; n < sizeof(TestCommand); n++) {
		readByteFromClient_buffer[n] = TestCommand[sizeof(TestCommand)-1-n]; //fill the other way round
	}
	readByteFromClient_bytesLeft = sizeof(TestCommand);

	itTick();

	const unsigned char ExpectedTelegram[] = { 0xAA, 0x01, 'T', 'e', 's', 't', 'C', 'o', 'm', 'm', 'a', 'n', 'd', 0x00, 0x01, 0x11, 0xDD, 0xCC, 0xCB, 0xCC, 0xBA, 0xCC, 0xA9, 0xBB };
	for (unsigned char n = 0; n < sizeof(ExpectedTelegram); n++) {
		ASSERT_EQ(writeByteToClient_buffer[n], ExpectedTelegram[n]);
	}
}

TEST_F(ItTest, handleCmdUint8) {
	readByteFromClient_buffer[0] = '\r';
	readByteFromClient_buffer[1] = 'A';
	readByteFromClient_bytesLeft = 2;

	itTick();

	const unsigned char ExpectedTelegram[] = { 0xAA, 0x01, 'A', 0x00, 0x02, 0x45, 0x78, 0x56, 0x34, 0x12, 0xBB };
	for (unsigned char n = 0; n < sizeof(ExpectedTelegram); n++) {
		ASSERT_EQ(writeByteToClient_buffer[n], ExpectedTelegram[n]);
	}
}

TEST_F(ItTest, handleCmdFloat) {
	readByteFromClient_buffer[0] = '\r';
	readByteFromClient_buffer[1] = 'B';
	readByteFromClient_bytesLeft = 2;

	itTick();

	const unsigned char ExpectedTelegram[] = { 0xAA, 0x01, 'B', 0x00, 0x03, 0xAE, 0x47, 0XA1, 0x3F, 0xDD, 0xCC, 0xCB, 0x99, 0xCC, 0xA9, 0xBB };
	for (unsigned char n = 0; n < sizeof(ExpectedTelegram); n++) {
		ASSERT_EQ(writeByteToClient_buffer[n], ExpectedTelegram[n]);
	}
}
