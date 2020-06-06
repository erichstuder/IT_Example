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

static void setInt8(signed char value) {
    mock_c()->actualCall("setInt8")->withIntParameters("value", value);
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

static void setFloat(float value) {
    mock_c()->actualCall("setFloat")->withDoubleParameters("value", (double)value);
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
        (void (*)(void)) setInt8,
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
        (void (*)(void)) setFloat,
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

TEST(ItCommandTest, UnknownGetCommand) {
    ItError_t err = parseCommand("ochotzgue");
    LONGS_EQUAL(ItError_InvalidCommand, err);
}

TEST(ItCommandTest, ValidGetCommandWithSpace) {
    ItError_t err = parseCommand("desiredValue ");
    LONGS_EQUAL(ItError_InvalidCommand, err);
}

TEST(ItCommandTest, InvalidGetCommand) {
    ItError_t err = parseCommand("InvalidType");
    LONGS_EQUAL(ItError_InvalidValueType, err);
}

TEST(ItCommandTest, GetInt8) {
    const char Int8Value = -42;
    mock().expectOneCall("getInt8")
        .andReturnValue(Int8Value);
    mock().expectOneCall("sendCommandResult")
        .withParameter("result->name", "Int8Value")
        .withParameter("result->resultInt8", Int8Value)
        .withParameter("result->valueType", ItValueType_Int8)
        .andReturnValue(ItError_NoError);
    ItError_t err = parseCommand("Int8Value");
    LONGS_EQUAL(ItError_NoError, err);
}

TEST(ItCommandTest, GetUint8) {
    const unsigned char Uint8Value = 201;
    mock().expectOneCall("getUint8").andReturnValue(Uint8Value);
    mock().expectOneCall("sendCommandResult")
        .withParameter("result->name", "desiredValue")
        .withParameter("result->resultUint8", Uint8Value)
        .withParameter("result->valueType", ItValueType_Uint8)
        .andReturnValue(ItError_NoError);
    ItError_t err = parseCommand("desiredValue");
    LONGS_EQUAL(ItError_NoError, err);
}

TEST(ItCommandTest, GetUlong) {
    const unsigned long UlongValue = 1000;
    mock().expectOneCall("getUlong").andReturnValue(UlongValue);
    mock().expectOneCall("sendCommandResult")
        .withParameter("result->name", "AAA")
        .withParameter("result->resultUlong", UlongValue)
        .withParameter("result->valueType", ItValueType_Ulong)
        .andReturnValue(ItError_NoError);
    ItError_t err = parseCommand("AAA");
    LONGS_EQUAL(ItError_NoError, err);
}

TEST(ItCommandTest, GetFloat) {
    const float FloatValue = 63.8f;
    mock().expectOneCall("getFloat").andReturnValue(FloatValue);
    mock().expectOneCall("sendCommandResult")
        .withParameter("result->name", "5674")
        .withParameter("result->resultFloat", FloatValue)
        .withParameter("result->valueType", ItValueType_Float)
        .andReturnValue(ItError_NoError);
    ItError_t err = parseCommand("5674");
    LONGS_EQUAL(ItError_NoError, err);
}

TEST(ItCommandTest, logValidCommand) {
    mock().expectNoCall("getFloat");
    mock().expectNoCall("sendCommandResult");
    ItError_t err = parseCommand("log 5674");
    LONGS_EQUAL(ItError_NoError, err);
}

TEST(ItCommandTest, logInvalidCommand) {
    mock().expectNoCall("getFloat");
    mock().expectNoCall("sendCommandResult");
    ItError_t err = parseCommand("log 5674_");
    LONGS_EQUAL(ItError_InvalidCommand, err);
}

TEST(ItCommandTest, logSignals) {
    const unsigned long UlongValue = 1000000;
    mock().expectOneCall("getUlong").andReturnValue(UlongValue);
    mock().expectOneCall("sendCommandResult")
        .withParameter("result->name", "AAA")
        .withParameter("result->resultUlong", UlongValue)
        .withParameter("result->valueType", ItValueType_Ulong)
        .andReturnValue(ItError_NoError);

    const char Int8Value = -99;
    mock().expectOneCall("getInt8").andReturnValue(Int8Value);
    mock().expectOneCall("sendCommandResult")
        .withParameter("result->name", "Int8Value")
        .withParameter("result->resultInt8", Int8Value)
        .withParameter("result->valueType", ItValueType_Int8)
        .andReturnValue(ItError_NoError);

    ItError_t err = parseCommand("log AAA");
    LONGS_EQUAL(ItError_NoError, err);
    err = parseCommand("log Int8Value");
    LONGS_EQUAL(ItError_NoError, err);

    err = logSignals();
}

TEST(ItCommandTest, tooManySignalsToLog) {
    ItError_t err;
    for (unsigned char n = 0; n < 10; n++) {
        err = parseCommand("log 5674");
        LONGS_EQUAL(ItError_NoError, err);
    }
    err = parseCommand("log 5674");
    LONGS_EQUAL(ItError_MaximumOfLoggedSignalsReached, err);
}

TEST(ItCommandTest, setFloat) {
    const float Value = 24.5;
    mock().expectOneCall("setFloat")
        .withParameter("value", Value);
    mock().expectOneCall("getFloat").andReturnValue(Value);
    mock().expectOneCall("sendCommandResult")
        .withParameter("result->name", "5674")
        .withParameter("result->resultFloat", Value)
        .withParameter("result->valueType", ItValueType_Float)
        .andReturnValue(ItError_NoError);

    ItError_t err = parseCommand("5674 24.5");
    LONGS_EQUAL(ItError_NoError, err);
}

TEST(ItCommandTest, SetSignalUnknown) {
    ItError_t err = parseCommand("unknownSignalName 24.5");
    LONGS_EQUAL(ItError_InvalidCommand, err);
}

TEST(ItCommandTest, SetSignalNoSpaceAfterSignalName) {
    ItError_t err = parseCommand("Int8Value_255");
    LONGS_EQUAL(ItError_InvalidCommand, err);
}

TEST(ItCommandTest, SetSignalNoValue) {
    ItError_t err = parseCommand("Int8Value ");
    LONGS_EQUAL(ItError_InvalidCommand, err);
}

TEST(ItCommandTest, SetSignalBadValue) {
    ItError_t err = parseCommand("Int8Value ae");
    LONGS_EQUAL(ItError_InvalidCommand, err);
}

TEST(ItCommandTest, reset) {
    ItError_t err = parseCommand("log desiredValue");
    LONGS_EQUAL(ItError_NoError, err);

    const unsigned char Uint8Value = 77;
    mock().expectOneCall("getUint8").andReturnValue(Uint8Value);
    mock().expectOneCall("sendCommandResult")
        .withParameter("result->name", "desiredValue")
        .withParameter("result->resultUint8", Uint8Value)
        .withParameter("result->valueType", ItValueType_Uint8)
        .andReturnValue(ItError_NoError);
    logSignals();

    err = parseCommand("reset");
    LONGS_EQUAL(ItError_NoError, err);

    logSignals();
}

TEST(ItCommandTest, unlog) {
    ItError_t err = parseCommand("log desiredValue");
    LONGS_EQUAL(ItError_NoError, err);
    err = parseCommand("log AAA");
    LONGS_EQUAL(ItError_NoError, err);

    const unsigned char Uint8Value = 77;
    mock().expectOneCall("getUint8").andReturnValue(Uint8Value);
    mock().expectOneCall("sendCommandResult")
        .withParameter("result->name", "desiredValue")
        .withParameter("result->resultUint8", Uint8Value)
        .withParameter("result->valueType", ItValueType_Uint8)
        .andReturnValue(ItError_NoError);
    const unsigned long UlongValue = 1000;
    mock().expectOneCall("getUlong").andReturnValue(UlongValue);
    mock().expectOneCall("sendCommandResult")
        .withParameter("result->name", "AAA")
        .withParameter("result->resultUlong", UlongValue)
        .withParameter("result->valueType", ItValueType_Ulong)
        .andReturnValue(ItError_NoError);
    logSignals();

    err = parseCommand("unlog AAA");
    LONGS_EQUAL(ItError_NoError, err);

    mock().expectOneCall("getUint8").andReturnValue(Uint8Value);
    mock().expectOneCall("sendCommandResult")
        .withParameter("result->name", "desiredValue")
        .withParameter("result->resultUint8", Uint8Value)
        .withParameter("result->valueType", ItValueType_Uint8)
        .andReturnValue(ItError_NoError);
    logSignals();
}
