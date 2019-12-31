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

#include "itCommand.h"

#define LoggedSignalCapacity 10
struct {
	ItSignal_t* signal[LoggedSignalCapacity];
	unsigned char nextFreeIndex;
	unsigned char count;
} loggedSignals;

static ItSignal_t* signals;
static unsigned char signalCount;

static SendCommandResult_t sendResult;

static ItError_t addSignalToLog(ItSignal_t* signal);
static ItError_t readSignalValue(ItSignal_t* signal);

void itCommandInit(ItSignal_t* itSignals, unsigned char itSignalCount, SendCommandResult_t sendCommandResult) {
	signals = itSignals;
	signalCount = itSignalCount;
	sendResult = sendCommandResult;

	loggedSignals.nextFreeIndex = 0;
	loggedSignals.count = 0;
}

static ItError_t parseCommand_Implementation(const char* const command) {
	const char* const LogId = "log ";
	if (strstr(command, LogId) == command) {
		for (int n = 0; n < signalCount; n++) {
			if (strcmp(signals[n].name, command + strlen(LogId)) == 0) {
				return addSignalToLog(&(signals[n]));
			}
		}
		return ItError_InvalidCommand;
	}
	else {
		unsigned char n;
		for (n = 0; n < signalCount; n++) {
			ItSignal_t signal = signals[n];
			if (strcmp(signal.name, command) == 0) {
				return readSignalValue(&signal);
			}
		}
		return ItError_InvalidCommand;
	}
}
ItError_t (*parseCommand) (const char* const command) = parseCommand_Implementation;

static ItError_t addSignalToLog(ItSignal_t* signal) {
	if (loggedSignals.nextFreeIndex >= LoggedSignalCapacity) {
		return ItError_MaximumOfLoggedSignalsReached;
	}
	loggedSignals.signal[loggedSignals.nextFreeIndex] = signal;
	loggedSignals.nextFreeIndex++;
	loggedSignals.count++;
	return ItError_NoError;
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