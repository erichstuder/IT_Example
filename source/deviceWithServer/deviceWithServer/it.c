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

#include "it.h"
#include "itCommand.h"

static char* inputBuffer;
static unsigned char inputBufferSize;
static unsigned char inputBufferIndex;
static bool inputBufferFull;

static ByteFromClientAvailable_t byteFromClientAvailable;
static ReadByteFromClient_t readByteFromClient;
static GetTimestamp_t getTimestamp;

static void handleDataByte(unsigned char dataByte);
static void handleCommandBufferError(ItError_t err);
static ItError_t sendResult(ItCommandResult_t* result);
static ItError_t cmdBufferAppend(const char dataByte);
static void clearCmdBuffer(void);

static void itInit_Implementation(ItParameters_t* parameters, ItCallbacks_t* callbacks) {
	inputBuffer = parameters->itInputBuffer;
	inputBufferSize = parameters->itInputBufferSize;
	inputBufferIndex = 0;
	inputBufferFull = false;

	itCommandInit(parameters->itSignals, parameters->itSignalCount, sendResult);

	byteFromClientAvailable = callbacks->byteFromClientAvailable;
	readByteFromClient = callbacks->readByteFromClient;
	getTimestamp = callbacks->getTimestamp;

	itTelegramInit(callbacks->writeByteToClient);
}
#ifdef ITLIBRARY_EXPORTS
__declspec(dllexport) void itInit(ItParameters_t* parameters, ItCallbacks_t* callbacks) {
	itInit_Implementation(parameters, callbacks);
}
#else
void (*itInit)(ItParameters_t* parameters, ItCallbacks_t* callbacks) = itInit_Implementation;
#endif

static void itTick_Implementation(void){
	char dataByte;
	ItError_t err;

	while (byteFromClientAvailable()) {
		err = readByteFromClient(&dataByte);
		if (err != ItError_NoError) {
			// Most probably just disconnected. We can't do anything about it.
			// In the future there may be a logging file or an error callback to restart the communication or error-signaling on the LED or ...
			return;
		}
		handleDataByte(dataByte);
	}
}
#ifdef ITLIBRARY_EXPORTS
__declspec(dllexport) void itTick(void) {
	itTick_Implementation();
}
#else
void (*itTick)(void) = itTick_Implementation;
#endif

static void handleDataByte(unsigned char dataByte) {
	ItError_t err;
	ItError_t commandErr;
	if (dataByte == '\r') {
		err = cmdBufferAppend('\0');
		handleCommandBufferError(err);

		commandErr = parseCommand(inputBuffer);
		if (commandErr != ItError_NoError) {
			itSendStringTelegram("Error while parsing command.");
			clearCmdBuffer(); //TOOD: kann dieser Aufruf an nur einer Stelle gemacht werden?
			return;
		}

		clearCmdBuffer(); 
	}
	else {
		err = cmdBufferAppend(dataByte);
		handleCommandBufferError(err);
	}
}

static void handleCommandBufferError(ItError_t err) {
	if (err == ItError_BufferFull) {
		ItError_t localErr;
		do {
			localErr = itSendStringTelegram("Error: Input Buffer is full!\n");
		} while (localErr != ItError_NoError);
		//TOOD: wäre es nicht sinnvoll den Buffer zu leeren, wenn BufferFull error?
		//Evtl. den vollen Buffer zu debugging zwecken an den Client senden
		//Evtl. an den Client die Capacität vom Buffer senden
	}
}

static ItError_t sendResult(ItCommandResult_t* result) {
	switch (result->valueType) {
	case ItValueType_Int8:
		return itSendValueTelegram_Int8(result->name, result->resultInt8, getTimestamp());
		break;
	case ItValueType_Uint8:
		return itSendValueTelegram_Uint8(result->name, result->resultInt8, getTimestamp());
		break;
	case ItValueType_Ulong:
		return itSendValueTelegram_Ulong(result->name, result->resultUlong, getTimestamp());
		break;
	case ItValueType_Float:
		return itSendValueTelegram_Float(result->name, result->resultFloat, getTimestamp());
		break;
	default:
		return ItError_InvalidValueType;
	}
}

static void clearCmdBuffer(void) {
	for (unsigned char n = 0; n < inputBufferSize; n++) {
		inputBuffer[n] = '\0';
	}
	inputBufferIndex = 0;
	inputBufferFull = false;
}

static ItError_t cmdBufferAppend(const char dataByte) {
	if (inputBufferFull == true) {
		return ItError_BufferFull;
	}
	inputBuffer[inputBufferIndex] = dataByte;
	if (inputBufferIndex < inputBufferSize - 1) {
		inputBufferIndex++;
	}
	else {
		inputBufferFull = true;
	}
	return ItError_NoError;
}