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
extern "C" {
#include "itCommand.h"
}

static signed char getInt8(void) {
    return -42;
}

static unsigned char getUint8(void) {
    return 201;
}

static unsigned long getUlong(void) {
    return 1000;
}

static float getFloat(void) {
    return 63.8f;
}

static ItSignal_t itSignals[] = {
    {
        "InvalidType",
        (ItValueType_t)99,
        (void (*)(void)) NULL,
        NULL,
    },
    {
        "Int8Value",
        ItValueType_Int8,
        (void (*)(void)) getInt8,
        NULL,
    },
    {
        "desiredValue",
        ItValueType_Uint8,
        (void (*)(void)) getUint8,
        NULL,
    },
    {
        "AAA",
        ItValueType_Ulong,
        (void (*)(void)) getUlong,
        NULL,
    },
    {
        "5674",
        ItValueType_Float,
        (void (*)(void)) getFloat,
        NULL,
    },
};

TEST_GROUP(ItCommandTest) {
    void setup() {
        itCommandInit(itSignals, sizeof(itSignals)/sizeof(itSignals[0]));
    }

    void teardown() {
    }
};

TEST(ItCommandTest, UnknownCommand) {
    ItError_t err = parseCommand("ochotzgue", NULL);
    LONGS_EQUAL(ItError_UnknownCommand, err);
}

TEST(ItCommandTest, InvalidCommand) {
    ItCommandResult_t result;
    ItError_t err = parseCommand("InvalidType", &result);
    LONGS_EQUAL(ItError_InvalidCommand, err);
}

TEST(ItCommandTest, Int8) {
    ItCommandResult_t result;
    ItError_t err = parseCommand("Int8Value", &result);
    LONGS_EQUAL(ItError_NoError, err);
    STRCMP_EQUAL("Int8Value", result.name);
    LONGS_EQUAL(ItValueType_Int8, result.valueType);
    LONGS_EQUAL(-42, result.resultInt8);
}

TEST(ItCommandTest, Uint8) {
    ItCommandResult_t result;
    ItError_t err = parseCommand("desiredValue", &result);
    LONGS_EQUAL(ItError_NoError, err);
    STRCMP_EQUAL("desiredValue", result.name);
    LONGS_EQUAL(ItValueType_Uint8, result.valueType);
    LONGS_EQUAL(201, result.resultUint8);
}

TEST(ItCommandTest, Ulong) {
    ItCommandResult_t result;
    ItError_t err = parseCommand("AAA", &result);
    LONGS_EQUAL(ItError_NoError, err);
    STRCMP_EQUAL("AAA", result.name);
    LONGS_EQUAL(ItValueType_Ulong, result.valueType);
    LONGS_EQUAL(1000, result.resultUlong);
}

TEST(ItCommandTest, Float) {
    ItCommandResult_t result;
    ItError_t err = parseCommand("5674", &result);
    LONGS_EQUAL(ItError_NoError, err);
    STRCMP_EQUAL("5674", result.name);
    LONGS_EQUAL(ItValueType_Float, result.valueType);
    LONGS_EQUAL(63.8f, result.resultFloat);
}
