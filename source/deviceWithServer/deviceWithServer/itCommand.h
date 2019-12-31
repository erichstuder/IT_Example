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

#ifndef IT_COMMAND_H
#define IT_COMMAND_H

#include "itError.h"

typedef enum {
	ItValueType_Int8,
	ItValueType_Uint8,
	ItValueType_Ulong,
	ItValueType_Float,
} ItValueType_t;

typedef const struct {
	const char* const name;
	const ItValueType_t valueType;
	void (*getter) (void); //TODO: testen ob getter und setter nicht verändert werden können.
	void (*setter) (void);
} ItSignal_t;

typedef struct {
	const char* name;
	ItValueType_t valueType;
	union {
		signed char resultInt8;
		unsigned char resultUint8;
		unsigned long resultUlong;
		float resultFloat;
	};
} ItCommandResult_t;

typedef ItError_t(*SendCommandResult_t) (ItCommandResult_t* result);

void itCommandInit(ItSignal_t* itSignals, unsigned char itSignalCount, SendCommandResult_t sendCommandResult);
extern ItError_t (*parseCommand) (const char* const command);
extern ItError_t (*logSignals) (void);

#endif
