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

#include "it.h"

static ItError writeBytesToClient_Spy(const char* const byteArray, const unsigned char byteCount) {
	return ItError::NoError;
}

static ItError readByteFromClient_Spy(char* const data) {
	return ItError::NoError;
}

static ItError cmdHandler_Spy(double* result) {
	return ItError::NoError;
}

static ItError cmdBufferAppend_Spy(const char letter) {
	return ItError::NoError;
}


class ItTest : public ::testing::Test {
protected:

	ItTest() {}

	virtual ~ItTest() {}

	virtual void SetUp() {
		itInit(writeBytesToClient_Spy, readByteFromClient_Spy, cmdHandler_Spy, cmdBufferAppend_Spy);
	}

	virtual void TearDown() {}
};


TEST_F(ItTest, itInit) {
	itInit(NULL, NULL, NULL, NULL);
	//Whether the callbacks are assigned correctly is tested indirectly by other tests below.
}

TEST_F(ItTest, tick) {
	itTick();
	//TODO: add more to test
}
