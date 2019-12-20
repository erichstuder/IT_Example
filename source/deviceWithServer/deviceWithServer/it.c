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
//#include "arduino.h" //debug

#define TelegramStart 0xAA
#define TelegramEnd 0xBB
#define ReplacementMarker 0xCC

//#ifndef IT_CMD_BUFFER_SIZE
//	#error IT_CMD_BUFFER_SIZE must be defined (including string terminator)
//#endif
//extern const unsigned char IT_CMD_BUFFER_SIZE;

static unsigned char* cmdBuffer;
static unsigned char cmdBufferSize;
static unsigned char cmdBufferIndex;
static bool cmdBufferFull;

static WriteByteToClient_t writeByteToClient;
static ReadByteFromClient_t readByteFromClient;
static CmdHandler_t cmdHandler;
//static CmdBufferAppend_t cmdBufferAppend;

/*static struct OutBuffer{
	char data[255];
	unsigned int writeIndex;
};*/

typedef enum {
	TelegramType_SingleRequestAnswer = 0x01,
	TelegramType_Logging             = 0x02,
	TelegramType_Error               = 0x03,
} TelegramType_t;

static ItError_t sendAnswer(void);
static ItError_t sendContentByte(unsigned char data);
static ItError_t sendTelegramStart(void);
static ItError_t sendTelegramContent(void);
static ItError_t sendTelegramType(TelegramType_t type);
static ItError_t sendValueName(void);
static ItError_t sendValueType(ValueType_t valueType);
static ItError_t sendValue(const ItCommandResult_t* const result);
static ItError_t sendTelegramEnd(void);
static ItError_t sendTimeStampOfValue(unsigned long timeStampOfValue);
static ItError_t cmdBufferAppend(const char dataByte);
static void clearCmdBuffer(void);

static void itInit_Implementation(unsigned char* itCmdBuffer, unsigned char itCmdBufferSize, CmdHandler_t cmdHandlerCallback, WriteByteToClient_t writeByteToClientCallback, ReadByteFromClient_t readByteFromClientCallback) {
	cmdBuffer = itCmdBuffer;
	cmdBufferSize = itCmdBufferSize;
	cmdBufferIndex = 0;
	cmdBufferFull = false;

	writeByteToClient = writeByteToClientCallback;
	readByteFromClient = readByteFromClientCallback;
	cmdHandler = cmdHandlerCallback;
}
#ifdef ITLIBRARY_EXPORTS
__declspec(dllexport) void itInit(unsigned char* itCmdBuffer, unsigned char itCmdBufferSize, CmdHandler_t cmdHandlerCallback, WriteByteToClient_t writeByteToClientCallback, ReadByteFromClient_t readByteFromClientCallback) {
	itInit_Implementation(itCmdBuffer, itCmdBufferSize, cmdHandlerCallback, writeByteToClientCallback, readByteFromClientCallback);
}
#else
void (*itInit)(unsigned char* itCmdBuffer, unsigned char itCmdBufferSize, CmdHandler_t cmdHandlerCallback, WriteByteToClient_t writeByteToClientCallback, ReadByteFromClient_t readByteFromClientCallback) = itInit_Implementation;
#endif

static void itTick_Implementation(void){
	//TODO: handle the errors
	unsigned char dataByte;
	ItError_t readByteError;
	ItError_t cmdBufferError;

	do{
		readByteError = readByteFromClient(&dataByte);
		if(readByteError == ItError_NoDataAvailable){
			return;
		}else if(readByteError == ItError_ClientUnavailable){
			return;
		}else if(readByteError != ItError_NoError){
			return;
		}
	
		if(dataByte == '\r'){
			cmdBufferError = cmdBufferAppend('\0');
			if(cmdBufferError == ItError_BufferFull){
				return;//TODO: inform client
			}else if(cmdBufferError != ItError_NoError){
				return;
			}
			sendAnswer();
			clearCmdBuffer();



			/*cmdHandlerError = cmdHandler(&result);
			if(cmdHandlerError == ItError::InvalidCommand){
				return;
			}else if(cmdHandlerError != ItError::NoError){
				return;
			}*/
			
			/*writeByteError = writeByteToClient((char*)&result, sizeof(result));
			if(writeByteError == ItError::ClientUnavailable){
				return;
			}else if(writeByteError == ItError::ClientWriteError){
				return;
			}else if(writeByteError != ItError::NoError){
				return;
			}*/
		}else{
			cmdBufferError = cmdBufferAppend(dataByte);
			if(cmdBufferError == ItError_BufferFull){
				return;//TODO: inform client
			}else if(cmdBufferError != ItError_NoError){
				return;
			}
		}
	}while(true);
}
#ifdef ITLIBRARY_EXPORTS
__declspec(dllexport) void itTick(void) {
	itTick_Implementation();
}
#else
void (*itTick)(void) = itTick_Implementation;
#endif

static ItError_t sendAnswer(void) {
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

static ItError_t sendTelegramContent(void) {//TODO: replace 0xAA and 0xBB by 0xCC and 0xAA-1 and 0xBB-1
	ItError_t err;

	//Telegram definition: telegrammStartId, telegramType(data, error, ...), valueName, valueDataTypeId, value, timeStampOfValue, telegramStopId
	err = sendTelegramType(TelegramType_SingleRequestAnswer);
	if (err != ItError_NoError) {
		return err;
	}

	ItCommandResult_t result;
	err = cmdHandler(&result);
	if (err != ItError_NoError) {
		return err;
	}

	err = sendValueName();
	if (err != ItError_NoError) {
		return err;
	}

	err = sendValueType(result.valueType);
	if (err != ItError_NoError) {
		return err;
	}

	err = sendValue(&result);
	if (err != ItError_NoError) {
		return err;
	}

	err = sendTimeStampOfValue(result.timestamp);
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
	for (unsigned char n = 0; n < strlen((char*)cmdBuffer)+1; n++) {
		err = sendContentByte(cmdBuffer[n]);
		if (err != ItError_NoError) {
			return err;
		}
	}
	return ItError_NoError;
}

static ItError_t sendValueType(ValueType_t valueType) {
	return sendContentByte(valueType);
}

static ItError_t sendValue(const ItCommandResult_t* const result) {
	ItError_t err;
	unsigned char* value;
	switch (result->valueType) {
	case ValueType_Int8:
		err = sendContentByte(result->valueInt8);
		if (err != ItError_NoError) {
			return err;
		}
		break;
	case ValueType_Uint8:
		err = sendContentByte(result->valueUint8);
		if (err != ItError_NoError) {
			return err;
		}
		break;
	case ValueType_Float:
		value = (unsigned char*)&(result->valueFloat);
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
	for (unsigned char n = 0; n < cmdBufferSize; n++) {
		cmdBuffer[n] = '\0';
	}
	cmdBufferIndex = 0;
	cmdBufferFull = false;
}

static ItError_t cmdBufferAppend(const char dataByte) {
	if (cmdBufferFull == true) {
		return ItError_BufferFull;
	}
	cmdBuffer[cmdBufferIndex] = dataByte;
	if (cmdBufferIndex < cmdBufferSize - 2) { //leave last byte for zero terminator
		cmdBufferIndex++;
	}
	else {
		cmdBufferFull = true;
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
