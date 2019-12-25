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

typedef enum {
	ItCommandError_NoError,
	ItCommandError_InvalidCommand,
	ItCommandError_UnknownCommand,
} ItCommandError_t;

typedef enum {
	ItValueType_Int8 = 0x01,
	ItValueType_Uint8 = 0x02,
	ItValueType_Ulong = 0x03,
	ItValueType_Float = 0x04,
} ItValueType_t;

typedef const struct {
	const char* const name;
	const ItValueType_t valueType;
	void (*getter) (void); //TODO: testen ob getter und setter nicht ver�ndert werden k�nnen.
	void (*setter) (void);
} ItSignal_t;

typedef struct {
	const char* name;
	ItValueType_t valueType;
	union {
		signed char resultInt8;
		unsigned char resultUint8;
		float resultFloat;
	};
} ItCommandResult_t;

void itCommandInit(ItSignal_t* itSignals, unsigned char itSignalCount);
ItCommandError_t parseCommand(const char* const command, ItCommandResult_t* result);

#endif