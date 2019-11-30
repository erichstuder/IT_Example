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

#include "it.h"
//#include "arduino.h" //debug

const unsigned char TelegramStart = 0xAA;
const unsigned char TelegramEnd= 0xBB;
const unsigned char ReplacementMarker = 0xCC;

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

enum class TelegramType {
	SingleRequestAnswer = 0x01,
	Logging             = 0x02,
	Error               = 0x03,
};

enum class ValueType {
	Int8   = 0x01,
	Uint8  = 0x02,
	Double = 0x03,
};

static ItError sendAnswer(void);
static ItError sendContentByte(unsigned char data);
static ItError sendTelegramStart(void);
static ItError sendTelegramContent(void);
static ItError sendTelegramType(TelegramType type);
static ItError sendValueName(void);
static ItError sendValueDataType(void);
static ItError sendValue(double value);
static ItError sendTelegramEnd(void);
static ItError sendTimeStampOfValue(unsigned long timeStampOfValue);
static ItError cmdBufferAppend(const char dataByte);
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
void (*itInit)(unsigned char* itCmdBuffer, unsigned char itCmdBufferSize, CmdHandler_t cmdHandlerCallback, WriteByteToClient_t writeByteToClientCallback, ReadByteFromClient_t readByteFromClientCallback) = itInit_Implementation;

static void itTick_Implementation(void){
	//TODO: handle the errors
	unsigned char dataByte;
	ItError readByteError;
	ItError cmdBufferError;

	do{
		readByteError = readByteFromClient(&dataByte);
		if(readByteError == ItError::NoDataAvailable){
			return;
		}else if(readByteError == ItError::ClientUnavailable){
			return;
		}else if(readByteError != ItError::NoError){
			return;
		}
	
		if(dataByte == '\r'){
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
			if(cmdBufferError == ItError::BufferFull){
				return;//TODO: inform client
			}else if(cmdBufferError != ItError::NoError){
				return;
			}
		}
	}while(true);
}
void (*itTick)(void) = itTick_Implementation;

static ItError sendAnswer(void) {
	ItError err = sendTelegramStart();
	if (err != ItError::NoError) {
		return err;
	}

	err = sendTelegramContent();
	if (err != ItError::NoError) {
		return err;
	}

	return sendTelegramEnd();
}

static ItError sendTelegramStart(void) {
	return writeByteToClient(TelegramStart);
}

static ItError sendTelegramContent(void) {//TODO: replace 0xAA and 0xBB by 0xCC and 0xAA-1 and 0xBB-1
	ItError err;

	//Telegram definition: telegrammStartId, telegramType(data, error, ...), valueName, valueDataTypeId, value, timeStampOfValue, telegramStopId
	err = sendTelegramType(TelegramType::SingleRequestAnswer);
	if (err != ItError::NoError) {
		return err;
	}

	double value;
	unsigned long timeStampOfValue;
	err = cmdHandler(&value, &timeStampOfValue);
	if (err != ItError::NoError) {
		return err;
	}

	err = sendValueName();
	if (err != ItError::NoError) {
		return err;
	}

	err = sendValueDataType();
	if (err != ItError::NoError) {
		return err;
	}

	err = sendValue(value);
	if (err != ItError::NoError) {
		return err;
	}

	err = sendTimeStampOfValue(timeStampOfValue);
	if (err != ItError::NoError) {
		return err;
	}

	return ItError::NoError;
}

static ItError sendTelegramType(TelegramType type) {
	return sendContentByte((unsigned char)type);//TODO: ist casten die saubere L�sung?
}

static ItError sendContentByte(unsigned char data) {
	switch (data) {
		case TelegramStart:
		case TelegramEnd:
		case ReplacementMarker:
			ItError err = writeByteToClient(ReplacementMarker);
			if (err != ItError::NoError) {
				return err;
			}
			data--;
			break;
	}
	return writeByteToClient(data);
}

static ItError sendValueName(void) {
	ItError err;
	for (unsigned char n = 0; n < strlen((char*)cmdBuffer); n++) {
		err = sendContentByte(cmdBuffer[n]);
		if (err != ItError::NoError) {
			return err;
		}
	}
	return ItError::NoError;
}

static ItError sendValueDataType(void) {
	return sendContentByte((unsigned char)ValueType::Double);
}

static ItError sendValue(double value) {
	const unsigned char ValueSize = sizeof(value);
	union {
		double value;
		unsigned char valueByteArray[ValueSize];
	} data;
	data.value = value;
	ItError err;
	for (unsigned char n = 0; n < ValueSize; n++) {
		err = sendContentByte(data.valueByteArray[n]);
		if (err != ItError::NoError) {
			return err;
		}
	}
	return ItError::NoError;
}

static ItError sendTimeStampOfValue(unsigned long timeStampOfValue) {
	const unsigned char StampSize = sizeof(timeStampOfValue);
	union {
		unsigned long value;
		unsigned char valueByteArray[StampSize];
	} data;
	data.value = timeStampOfValue;
	ItError err;
	for (unsigned char n = 0; n < StampSize; n++) {
		err = sendContentByte(data.valueByteArray[n]);
		if (err != ItError::NoError) {
			return err;
		}
	}
	return ItError::NoError;
}

static ItError sendTelegramEnd(void) {
	return writeByteToClient(TelegramEnd);
}

static void clearCmdBuffer(void) {
	for (unsigned char n = 0; n < cmdBufferSize; n++) {
		cmdBuffer[n] = '\0';
	}
	cmdBufferIndex = 0;
	cmdBufferFull = false;
}

static ItError cmdBufferAppend(const char dataByte) {
	if (cmdBufferFull == true) {
		return ItError::BufferFull;
	}
	cmdBuffer[cmdBufferIndex] = dataByte;
	if (cmdBufferIndex < cmdBufferSize - 2) { //leave last byte for zero terminator
		cmdBufferIndex++;
	}
	else {
		cmdBufferFull = true;
	}
	return ItError::NoError;
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
