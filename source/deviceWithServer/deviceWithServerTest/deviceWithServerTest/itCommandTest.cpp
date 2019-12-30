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
#include "itCommand.h"
}

static signed char getInt8(void) {
    mock_c()->actualCall("getInt8");
    return (signed char)mock_c()->returnValue().value.intValue;
}

static unsigned char getUint8(void) {
    mock_c()->actualCall("getUint8");
    return (unsigned char)mock_c()->returnValue().value.intValue;;
}

static unsigned long getUlong(void) {
    mock_c()->actualCall("getUlong");
    return (unsigned long)mock_c()->returnValue().value.intValue;
}

static float getFloat(void) {
    mock_c()->actualCall("getFloat");
    return (float)mock_c()->returnValue().value.doubleValue;
}

static ItError_t sendCommandResult(ItCommandResult_t* result) {
    switch (result->valueType) {
    case ItValueType_Int8:
        mock_c()->actualCall("sendCommandResult")
            ->withStringParameters("result->name", result->name)
            ->withIntParameters("result->resultInt8", result->resultInt8)
            ->withIntParameters("result->valueType", result->valueType);
        break;
    case ItValueType_Uint8:
        mock_c()->actualCall("sendCommandResult")
            ->withStringParameters("result->name", result->name)
            ->withIntParameters("result->resultUint8", result->resultUint8)
            ->withIntParameters("result->valueType", result->valueType);
        break;
    case ItValueType_Ulong:
        mock_c()->actualCall("sendCommandResult")
            ->withStringParameters("result->name", result->name)
            ->withIntParameters("result->resultUlong", result->resultUlong)
            ->withIntParameters("result->valueType", result->valueType);
        break;
    case ItValueType_Float:
        mock_c()->actualCall("sendCommandResult")
            ->withStringParameters("result->name", result->name)
            ->withDoubleParameters("result->resultFloat", result->resultFloat)
            ->withIntParameters("result->valueType", result->valueType);
        break;
    }
#pragma warning(push)
#pragma warning(disable : 26812)
    return (ItError_t)mock_c()->returnValue().value.intValue;
#pragma warning(pop)
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
        itCommandInit(itSignals, sizeof(itSignals)/sizeof(itSignals[0]), sendCommandResult);
    }

    void teardown() {
        mock().checkExpectations();
        mock().clear();
    }
};

TEST(ItCommandTest, UnknownCommand) {
    ItError_t err = parseCommand("ochotzgue");
    LONGS_EQUAL(ItError_UnknownCommand, err);
}

TEST(ItCommandTest, InvalidCommand) {
    ItError_t err = parseCommand("InvalidType");
    LONGS_EQUAL(ItError_InvalidCommand, err);
}

TEST(ItCommandTest, Int8) {
    mock().strictOrder();
    mock().expectOneCall("getInt8").andReturnValue(-42);
    mock().expectOneCall("sendCommandResult")
        .withParameter("result->name", "Int8Value")
        .withParameter("result->resultInt8", -42)
        .withParameter("result->valueType", ItValueType_Int8)
        .andReturnValue(ItError_NoError);
    ItError_t err = parseCommand("Int8Value");
    LONGS_EQUAL(ItError_NoError, err);
}

TEST(ItCommandTest, Uint8) {
    mock().strictOrder();
    mock().expectOneCall("getUint8").andReturnValue(201);
    mock().expectOneCall("sendCommandResult")
        .withParameter("result->name", "desiredValue")
        .withParameter("result->resultUint8", 201)
        .withParameter("result->valueType", ItValueType_Uint8)
        .andReturnValue(ItError_NoError);
    ItError_t err = parseCommand("desiredValue");
    LONGS_EQUAL(ItError_NoError, err);
}

TEST(ItCommandTest, Ulong) {
    mock().strictOrder();
    mock().expectOneCall("getUlong").andReturnValue(1000);
    mock().expectOneCall("sendCommandResult")
        .withParameter("result->name", "AAA")
        .withParameter("result->resultUlong", 1000)
        .withParameter("result->valueType", ItValueType_Ulong)
        .andReturnValue(ItError_NoError);
    ItError_t err = parseCommand("AAA");
    LONGS_EQUAL(ItError_NoError, err);
}

TEST(ItCommandTest, Float) {
    mock().strictOrder();
    mock().expectOneCall("getFloat").andReturnValue(63.8f);
    mock().expectOneCall("sendCommandResult")
        .withParameter("result->name", "5674")
        .withParameter("result->resultFloat", 63.8f)
        .withParameter("result->valueType", ItValueType_Float)
        .andReturnValue(ItError_NoError);
    ItError_t err = parseCommand("5674");
    LONGS_EQUAL(ItError_NoError, err);
}

/*TEST(ItCommandTest, logValidCommand) {
    mock().expectOneCall("getFloat").andReturnValue(-3.1856f);
    ItCommandResult_t result;
    ItError_t err = parseCommand("log 5674", &result);
    LONGS_EQUAL(ItError_NoError, err);
    STRCMP_EQUAL("5674", result.name);
    LONGS_EQUAL(ItValueType_Float, result.valueType);
    LONGS_EQUAL(63.8f, result.resultFloat);
}*/