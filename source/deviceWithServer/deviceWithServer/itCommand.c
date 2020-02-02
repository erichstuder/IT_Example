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

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "itCommand.h"

#define LoggedSignalCapacity 10
struct {
	ItSignal_t* signal[LoggedSignalCapacity];
	unsigned char nextFreeIndex;
	unsigned char count;
} loggedSignals;

static const char* const LogCommandPrefix = "log ";

static ItSignal_t* signals;
static unsigned char signalCount;

static SendCommandResult_t sendResult;

static bool hasLogCommandFormat(const char* const command);
static ItError_t handleLogCommand(const char* const command);
static ItError_t addSignalToLog(ItSignal_t* signal);
static bool hasSetCommandFormat(const char* const command);
static ItError_t handleSetCommand(const char* const command);
static bool hasRequestCommandFormat(const char* const command);
static ItError_t handleRequestCommand(const char* const command);
static ItError_t readSignalValue(ItSignal_t* signal);

void itCommandInit(ItSignal_t* itSignals, unsigned char itSignalCount, SendCommandResult_t sendCommandResult) {
	signals = itSignals;
	signalCount = itSignalCount;
	sendResult = sendCommandResult;

	loggedSignals.nextFreeIndex = 0;
	loggedSignals.count = 0;
}

static ItError_t parseCommand_Implementation(const char* const command) {
	
	if (hasLogCommandFormat(command)) {
		return handleLogCommand(command);
	}
	else if (hasSetCommandFormat(command)) {
		return handleSetCommand(command);
	}
	else if (hasRequestCommandFormat(command))	{
		return handleRequestCommand(command);
	}
	else {
		return ItError_InvalidCommand;
	}
}
ItError_t (*parseCommand) (const char* const command) = parseCommand_Implementation;

static bool hasLogCommandFormat(const char* const command) {
	return strstr(command, LogCommandPrefix) == command;
}

static ItError_t handleLogCommand(const char* const command) {
	unsigned char n;
	for (n = 0; n < signalCount; n++) {
		ItSignal_t* signalPtr = &(signals[n]);
		const unsigned char LogCommandPrefixLength = strlen(LogCommandPrefix);
		if (strcmp(signalPtr->name, command + LogCommandPrefixLength) == 0) {
			return addSignalToLog(signalPtr);
		}
	}
	return ItError_InvalidCommand;
}

static ItError_t addSignalToLog(ItSignal_t* signal) {
	if (loggedSignals.nextFreeIndex >= LoggedSignalCapacity) {
		return ItError_MaximumOfLoggedSignalsReached;
	}
	loggedSignals.signal[loggedSignals.nextFreeIndex] = signal;
	loggedSignals.nextFreeIndex++;
	loggedSignals.count++;
	return ItError_NoError;
}

static bool hasSetCommandFormat(const char* const command) {
	unsigned char index = 0;
	unsigned char spaceCount = 0;
	while (command[index] != '\0') {
		if (command[index] == ' ') {
			spaceCount++;
		}
		index++;
	}
	return spaceCount == 1;
}

static ItError_t handleSetCommand(const char* const command) {
	//signal suchen => wenn nicht gefunden, dann error
	//danach muss ein Leerschlag kommen => wenn nicht, dann error
	//den rest in zahl umwandeln => wenn nicht möglich, dann error

	unsigned char signalIndex;
	bool signalFound = false;
	const char* signalName = "";
	ItSignal_t signal;
	for (signalIndex = 0; signalIndex < signalCount; signalIndex++) {
		signalName = signals[signalIndex].name;
		if (strstr(command, signalName) == command) {
			signalFound = true;
			break;
		}
	}
	if (!signalFound) {
		return ItError_InvalidCommand;
	}

	if (strlen(command) <= strlen(signalName)) {
		return ItError_InvalidCommand;
	}

	if (command[strlen(signalName)] != ' ') {
		return ItError_InvalidCommand;
	}

	const char* startPosition = strchr(command, ' ') + 1;
	if (*startPosition == '\0') {
		return ItError_InvalidCommand;
	}

	char* charAfterNumber;
	double value = strtod(startPosition, &charAfterNumber);
	if (*charAfterNumber != '\0') {
		return ItError_InvalidCommand;
	}

	switch (signals[signalIndex].valueType) {
	case ItValueType_Int8:
		((void (*) (signed char))signals[signalIndex].setter)((signed char)value);
		break;
	case ItValueType_Uint8:
		((void (*) (unsigned char))signals[signalIndex].setter)((unsigned char)value);
		break;
	case ItValueType_Ulong:
		((void (*) (unsigned long))signals[signalIndex].setter)((unsigned long)value);
		break;
	case ItValueType_Float:
		((void (*) (float))signals[signalIndex].setter)((float)value);
		break;
	default:
		break;
	}

	return ItError_NoError;
}

static bool hasRequestCommandFormat(const char* const command) {
	return !hasLogCommandFormat(command) && !hasSetCommandFormat(command);
}

static ItError_t handleRequestCommand(const char* const command) {
	unsigned char n;
	for (n = 0; n < signalCount; n++) {
		ItSignal_t signal = signals[n];
		if (strcmp(command, signal.name) == 0) {
			return readSignalValue(&signal);
		}
	}
	return ItError_InvalidCommand;
}

ItError_t logSignals_Implementation(void) {
	unsigned char n;
	for (n = 0; n < loggedSignals.count; n++) {
		ItError_t err = readSignalValue(loggedSignals.signal[n]);
		if (err != ItError_NoError) {
			return err;
		}
	}
	return ItError_NoError;
}
ItError_t (*logSignals) (void) = logSignals_Implementation;

static ItError_t readSignalValue(ItSignal_t* signal) {
	ItCommandResult_t result;
	result.name = signal->name;
	switch (signal->valueType) {
	case ItValueType_Int8:
		result.valueType = ItValueType_Int8;
		result.resultInt8 = ((signed char (*) (void)) signal->getter)();
		break;
	case ItValueType_Uint8:
		result.valueType = ItValueType_Uint8;
		result.resultUint8 = ((unsigned char (*) (void)) signal->getter)();
		break;
	case ItValueType_Ulong:
		result.valueType = ItValueType_Ulong;
		result.resultUlong = ((unsigned long (*) (void)) signal->getter)();
		break;
	case ItValueType_Float:
		result.valueType = ItValueType_Float;
		result.resultFloat = ((float (*) (void)) signal->getter)();
		break;
	default:
		return ItError_InvalidValueType;
		break;
	}
	return sendResult(&result);
}