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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef IT_H
#define IT_H

typedef enum {
	ItError_NoError,
	ItError_BufferFull,
	ItError_ClientUnavailable,
	ItError_ClientWriteError,
	ItError_NoDataAvailable,
	ItError_InvalidCommand,
	ItError_InvalidValueType,
	ItError_Unknown,
}ItError_t;

typedef enum {
	ValueType_Int8 = 0x01,
	ValueType_Uint8 = 0x02,
	ValueType_Float = 0x03,
} ValueType_t;

typedef struct {
	ValueType_t valueType;
	union {
		signed char valueInt8;
		unsigned char valueUint8;
		float valueFloat;
	};
	unsigned long timestamp;
}ItCommandResult_t;

typedef bool (*ByteFromClientAvailable_t) (void);
typedef ItError_t (*ReadByteFromClient_t) (unsigned char* const data);
typedef ItError_t (*WriteByteToClient_t) (const unsigned char data);
typedef ItError_t (*CmdHandler_t) (ItCommandResult_t* result);

typedef struct {
	ByteFromClientAvailable_t byteFromClientAvailable;
	ReadByteFromClient_t readByteFromClient;
	WriteByteToClient_t writeByteToClient;
	CmdHandler_t itCmdHandler;
}ItCallbacks_t;

#ifdef ITLIBRARY_EXPORTS
//nothing to do
#else
extern void (*itInit)(unsigned char* itCmdBuffer, unsigned char itCmdBufferSize, ItCallbacks_t callbacks);
extern void (*itTick)(void);
#endif

#endif
