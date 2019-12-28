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

#define TelegramStart 0xAA
#define TelegramEnd 0xBB
#define ReplacementMarker 0xCC

static char* inputBuffer;
static unsigned char inputBufferSize;
static unsigned char inputBufferIndex;
static bool inputBufferFull;

static ItCommandResult_t commandResult;

static ByteFromClientAvailable_t byteFromClientAvailable;
static ReadByteFromClient_t readByteFromClient;
static WriteByteToClient_t writeByteToClient;
static GetTimestamp_t getTimestamp;
//static CmdHandler_t cmdHandler;

typedef enum {
	TelegramType_SingleRequestAnswer = 0x01,
	//TelegramType_Logging             = 0x02, //TODO: notwendig?
	//TelegramType_Error               = 0x03, //TODO: notwendig?
} TelegramType_t;

static void handleDataByte(unsigned char dataByte);
static void handleCommandBufferError(ItError_t err);
static ItError_t sendString(const char* const text);
static ItError_t sendResult(void);
static ItError_t sendContentByte(unsigned char data);
static ItError_t sendTelegramStart(void);
static ItError_t sendTelegramContent(void);
static ItError_t sendTelegramType(TelegramType_t type);
static ItError_t sendValueName(void);
static ItError_t sendValueType(ItValueType_t valueType);
static ItError_t sendValue(void);
static ItError_t sendTelegramEnd(void);
static ItError_t sendTimeStampOfValue(unsigned long timeStampOfValue);
static ItError_t cmdBufferAppend(const char dataByte);
static void clearCmdBuffer(void);

static void itInit_Implementation(ItParameters_t* parameters, ItCallbacks_t* callbacks) {
	inputBuffer = parameters->itInputBuffer;
	inputBufferSize = parameters->itInputBufferSize;
	inputBufferIndex = 0;
	inputBufferFull = false;

	itCommandInit(parameters->itSignals, parameters->itSignalCount);

	byteFromClientAvailable = callbacks->byteFromClientAvailable;
	readByteFromClient = callbacks->readByteFromClient;
	writeByteToClient = callbacks->writeByteToClient;
	getTimestamp = callbacks->getTimestamp;
	//cmdHandler = callbacks->itCmdHandler;
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
	ItCommandError_t commandErr;
	if (dataByte == '\r') {
		err = cmdBufferAppend('\0');
		handleCommandBufferError(err);

		commandErr = parseCommand(inputBuffer, &commandResult);
		if (commandErr != ItCommandError_NoError) {
			sendString("Error while parsing command.");
			clearCmdBuffer(); //TOOD: kann dieser Aufruf an nur einer Stelle gemacht werden?
			return;
		}

		err = sendResult();
		if (err != ItError_NoError) {
			sendString("Error while sending result.\n");
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
			localErr = sendString("Error: Input Buffer is full!\n");
		} while (localErr != ItError_NoError);
		//TOOD: wäre es nicht sinnvoll den Buffer zu leeren, wenn BufferFull error?
		//Evtl. den vollen Buffer zu debugging zwecken an den Client senden
		//Evtl. an den Client die Capacität vom Buffer senden
	}
}

static ItError_t sendString(const char* const text) { //TODO: irgendwie ist das unschön
	ItError_t err;
	unsigned char n;
	for (n = 0; n < strlen(text); n++) {
		err = writeByteToClient(text[n]);
		if (err != ItError_NoError) {
			return err;
		}
	}
	return ItError_NoError;
}

static ItError_t sendResult(void) {
	ItError_t err = sendTelegramStart();
	if (err != ItError_NoError) {
		return err;
	}

	err = sendTelegramContent();
	if (err != ItError_NoError) {
		return err;
	}

	return sendTelegramEnd();
}

static ItError_t sendTelegramStart(void) {
	return writeByteToClient(TelegramStart);
}

static ItError_t sendTelegramContent(void) {
	ItError_t err;

	//Telegram definition: telegrammStartId, telegramType(data, error, ...), valueName, valueDataTypeId, value, timeStampOfValue, telegramStopId
	err = sendTelegramType(TelegramType_SingleRequestAnswer);
	if (err != ItError_NoError) {
		return err;
	}

	/*ItCommandResult_t result;
	err = cmdHandler(&result);
	if (err != ItError_NoError) {
		return err;
	}*/

	err = sendValueName();
	if (err != ItError_NoError) {
		return err;
	}

	err = sendValueType(commandResult.valueType);
	if (err != ItError_NoError) {
		return err;
	}

	err = sendValue();
	if (err != ItError_NoError) {
		return err;
	}

	err = sendTimeStampOfValue(getTimestamp());
	if (err != ItError_NoError) {
		return err;
	}

	return ItError_NoError;
}

static ItError_t sendTelegramType(TelegramType_t type) {
	return sendContentByte((unsigned char)type);//TODO: ist casten die saubere L�sung?
}

static ItError_t sendContentByte(unsigned char data) {
	ItError_t err;
	switch (data) {
		case TelegramStart:
		case TelegramEnd:
		case ReplacementMarker:
			err = writeByteToClient(ReplacementMarker);
			if (err != ItError_NoError) {
				return err;
			}
			data--;
			break;
	}
	return writeByteToClient(data);
}

static ItError_t sendValueName(void) {
	ItError_t err;
	for (unsigned char n = 0; n < strlen(commandResult.name)+1; n++) {
		err = sendContentByte(commandResult.name[n]);
		if (err != ItError_NoError) {
			return err;
		}
	}
	return ItError_NoError;
}

static ItError_t sendValueType(ItValueType_t valueType) {
	return sendContentByte(valueType);
}

static ItError_t sendValue(void) {
	ItError_t err;
	unsigned char* value;
	switch (commandResult.valueType) {
	case ItValueType_Int8:
		err = sendContentByte(commandResult.resultInt8);
		if (err != ItError_NoError) {
			return err;
		}
		break;
	case ItValueType_Uint8:
		err = sendContentByte(commandResult.resultUint8);
		if (err != ItError_NoError) {
			return err;
		}
		break;
	case ItValueType_Float:
		value = (unsigned char*)&(commandResult.resultFloat);
		for (unsigned char n = 0; n < sizeof(float); n++) {
			err = sendContentByte(*value);
			value++;
			if (err != ItError_NoError) {
				return err;
			}
		}
		break;
	default:
		return ItError_InvalidValueType;
	}
	return ItError_NoError;
}

static ItError_t sendTimeStampOfValue(unsigned long timeStampOfValue) {
	union {
		unsigned long value;
		unsigned char valueByteArray[sizeof(unsigned long)];
	} data;
	data.value = timeStampOfValue;
	ItError_t err;
	for (unsigned char n = 0; n < sizeof(data.valueByteArray); n++) {
		err = sendContentByte(data.valueByteArray[n]);
		if (err != ItError_NoError) {
			return err;
		}
	}
	return ItError_NoError;
}

static ItError_t sendTelegramEnd(void) {
	return writeByteToClient(TelegramEnd);
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







/*static void itSendToClient(char* signalName, double signalData, double timeStampOfSignalData){
	OutBuffer outBuffer;
	ItError err;

	err = createTelegram(&outBuffer, signalName, signalData, timeStampOfSignalData);
	if(err == ItError::BufferFull){
		sendError("BufferFull", err);
	}else if(err != ItError::NoError){
		sendError("Unexpected Error", err);
	}
	
 	err = writeBytesToClient(outBuffer.data, outBuffer.writeIndex);
	if(err == ItError::ClientUnavailable){
		//nothing done at the moment. Possible solution would be to have a buffer to store old telegrams
	}else if(err == ItError::ClientWriteError){
		sendError("ClientWriteError", err);
		for(;;);//endless loop to stop the program in case we can't send the error to the client
	}else if(err != ItError::NoError){
		sendError("Unexpected Error", err);
		for(;;);//endless loop to stop the program in case we can't send the error to the client
	}
}*/

/*static void sendError(const char* errMessage, ItError errId){
  	writeBytesToClientCallback(errMessage, strlen(errMessage));
	writeBytesToClientCallback((const byte*)(&errId), sizeof(errId));
}*/

//TODO:
//- CRC
//- zero terminator at end of signal name string
//static ItError createTelegram(OutBuffer* outBuffer, char* signalName, double signalData, double timeStampOfSignalData){
	//Telegram definition: telegrammStartId, telegramType(data, error, ...), valueName, valueDataTypeId, value, timeDataTypeId, timeStampOfValue, telegramStopId
	
	/*TODO: comment in again
	ItError_t err;
	
	initBuffer(outBuffer);

	err = appendCharArrayToBuffer(outBuffer, signalName, strlen(signalName)+1);
	if(err != NoError){
		return err;
	}

	err = appendDoubleToBuffer(outBuffer, signalData);
	if(err != NoError){
		return err;
	}
	
	err = appendDoubleToBuffer(outBuffer, timeStampOfSignalData);
	if(err != NoError){
		return err;
	}*/

//	return ItError::NoError;
//}

/*static void initBuffer(OutBuffer* outBuffer){
	outBuffer->data[0] = TelegramStart;
	outBuffer->writeIndex = 2;
}*/

/*static ItError appendDoubleToBuffer(OutBuffer* outBuffer, double doubleToAppend){
	ItError err;
	
	union{
		double doubleValue;
		char byteArray[sizeof(doubleValue)];
	}doubleToByteArray;
	doubleToByteArray.doubleValue = doubleToAppend;
	
	err = appendCharArrayToBuffer(outBuffer, doubleToByteArray.byteArray, sizeof(doubleToByteArray.byteArray));
	if(err != ItError::NoError){
		return err;
	}
	
	return ItError::NoError;
}*/

/*static ItError appendCharArrayToBuffer(OutBuffer* outBuffer, const char* array, unsigned int arrayLength){
	ItError err;
	
	for(unsigned int idx = 0; idx < arrayLength; idx++){
		err = appendByteToBuffer(outBuffer, array[idx]);
		if(err != ItError::NoError){
			return err;
		}
	}
	return ItError::NoError;
}*/

/*static ItError appendByteToBuffer(OutBuffer* outBuffer, unsigned char byteToAppend){
	ItError err;
	
	if(outBuffer->writeIndex >= sizeof(outBuffer->data)){
		return ItError::BufferFull;
	}
	
	if((byteToAppend == TelegramStart) || (byteToAppend == ReplacementMarker)){
		outBuffer->data[outBuffer->writeIndex] = ReplacementMarker;
		outBuffer->writeIndex++;
		
		err = appendByteToBuffer(outBuffer, byteToAppend-1);
		if(err != ItError::NoError){
			return err;
		}
	}else{
		outBuffer->data[outBuffer->writeIndex] = byteToAppend;
		outBuffer->writeIndex++;
	}
	
	outBuffer->data[1]=outBuffer->writeIndex;//set telegram length
	return ItError::NoError;
}*/
