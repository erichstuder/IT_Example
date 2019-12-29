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

typedef struct {
	ItSignal_t signal[10];
	unsigned char nextIndex;
} LoggedSignales_t;

static ItSignal_t* signals;
static unsigned char signalCount;

void itCommandInit(ItSignal_t* itSignals, unsigned char itSignalCount) {
	signals = itSignals;
	signalCount = itSignalCount;
}

static ItError_t parseCommand_Implementation(const char* const command, ItCommandResult_t* result) {
	unsigned char n;
	unsigned char commandFound = 0;
	for (n = 0; n < signalCount; n++) {
		ItSignal_t signal = signals[n];
		if (strcmp(signal.name, command) == 0) {
			commandFound = 1;
			result->name = command;
			switch (signal.valueType) {
			case ItValueType_Int8:
				result->valueType = ItValueType_Int8;
				result->resultInt8 = ((signed char (*) (void)) signal.getter)();
				break;
			case ItValueType_Uint8:
				result->valueType = ItValueType_Uint8;
				result->resultUint8 = ((unsigned char (*) (void)) signal.getter)();
				break;
			case ItValueType_Ulong:
				result->valueType = ItValueType_Ulong;
				result->resultUlong = ((unsigned long (*) (void)) signal.getter)();
				break;
			case ItValueType_Float:
				result->valueType = ItValueType_Float;
				result->resultFloat = ((float (*) (void)) signal.getter)();
				break;
			default:
				return ItError_InvalidCommand;
				break;
			}
			
		}
	}
	if (commandFound == 0) {
		return ItError_UnknownCommand;
	}
	return ItError_NoError;


	/*const char* LogString = "log ";
	if (strstr(command, LogString) == 0) {
		int n;
		for (n = 0; n < commandCount; n++) {
			if (strcmp(commands->name, cmdBuffer + strlen(LogString))) {
				//logCommands + ;
			}
		}
		//check if cmd exists in cmds
		//add link to cmd to loging cmds
	}
	else {
	}*/
}
ItError_t (*parseCommand) (const char* const command, ItCommandResult_t* result) = parseCommand_Implementation;
