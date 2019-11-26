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

#include "it.h"
#include "arduino.h" //debug

const unsigned char TelegramStart = 0xAA;
const unsigned char ReplacementMarker = 0xBB;
static WriteBytesToClient_t writeBytesToClient;
static ReadByteFromClient_t readByteFromClient;
static CmdHandler_t cmdHandler;
static CmdBufferAppend_t cmdBufferAppend;

struct OutBuffer{
	unsigned char data[255];
	unsigned int writeIndex;
};

ItError_t createTelegram(OutBuffer* outBuffer, char* signalName, double signalData, double timeStampOfSignalData);
ItError_t appendByteToBuffer(OutBuffer* outBuffer, unsigned char byteToAppend);
ItError_t appendCharArrayToBuffer(OutBuffer* outBuffer, const char* array, unsigned int arrayLength);
ItError_t appendDoubleToBuffer(OutBuffer* outBuffer, double doubleToAppend);
void initBuffer(OutBuffer* outBuffer);
void sendError(const char* errMessage, ItError_t errId);

void itInit(WriteBytesToClient_t writeBytesToClientCallback, ReadByteFromClient_t readByteFromClientCallback, CmdHandler_t cmdHandlerCallback, CmdBufferAppend_t cmdBufferAppendCallback){
	writeBytesToClient = writeBytesToClientCallback;
	readByteFromClient = readByteFromClientCallback;
	cmdHandler = cmdHandlerCallback;
	cmdBufferAppend = cmdBufferAppendCallback;
}

void itTick(void){
	//TODO: handle the errors
	double result;
	char dataByte;
	ItError_t readByteError;
	ItError_t cmdHandlerError;
	ItError_t writeBytesError;
	ItError_t cmdBufferError;

	do{
		readByteError = readByteFromClient(&dataByte);
		if(readByteError == NoDataAvailable){
			return;
		}else if(readByteError == ClientUnavailable){
			return;
		}else if(readByteError != NoError){
			return;
		}
	
		if(dataByte == '\r'){
			cmdHandlerError = cmdHandler(&result);
	
			Serial.println("it.cpp:");//debug
			//Serial.println(sizeof(result));//debug
			
			if(cmdHandlerError == InvalidCommand){
				return;
			}else if(cmdHandlerError != NoError){
				return;
			}
			
			writeBytesError = writeBytesToClient((char*)&result, sizeof(result));
			if(writeBytesError == ClientUnavailable){
				return;
			}else if(writeBytesError == ClientWriteError){
				return;
			}else if(writeBytesError != NoError){
				return;
			}
		}else{
			cmdBufferError = cmdBufferAppend(dataByte);
			if(cmdBufferError == BufferFull){
				return;
			}else if(cmdBufferError != NoError){
				return;
			}
		}
	}while(true); //TODO: refactor
}

void itSendToClient(char* signalName, double signalData, double timeStampOfSignalData){
	OutBuffer outBuffer;
	ItError_t err;

	err = createTelegram(&outBuffer, signalName, signalData, timeStampOfSignalData);
	if(err == BufferFull){
		sendError("BufferFull", err);
	}else if(err != NoError){
		sendError("Unexpected Error", err);
	}
	
 	err = writeBytesToClient(outBuffer.data, outBuffer.writeIndex);
	if(err == ClientUnavailable){
		//nothing done at the moment. Possible solution would be to have a buffer to store old telegrams
	}else if(err == ClientWriteError){
		sendError("ClientWriteError", err);
		for(;;);//endless loop to stop the program in case we can't send the error to the client
	}else if(err != NoError){
		sendError("Unexpected Error", err);
		for(;;);//endless loop to stop the program in case we can't send the error to the client
	}
}

void sendError(const char* errMessage, ItError_t errId){
  	/*TODO: comment in again
  	writeBytesToClientCallback(errMessage, strlen(errMessage));
	writeBytesToClientCallback((const byte*)(&errId), sizeof(errId));*/
}

//TODO:
//- CRC
//- zero terminator at end of signal name string
ItError_t createTelegram(OutBuffer* outBuffer, char* signalName, double signalData, double timeStampOfSignalData){
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

	return NoError;
}

void initBuffer(OutBuffer* outBuffer){
	outBuffer->data[0] = TelegramStart;
	outBuffer->writeIndex = 2;
}

ItError_t appendDoubleToBuffer(OutBuffer* outBuffer, double doubleToAppend){
	ItError_t err;
	
	union{
		double doubleValue;
		unsigned char byteArray[sizeof(doubleValue)];
	}doubleToByteArray;
	doubleToByteArray.doubleValue = doubleToAppend;
	
	err = appendCharArrayToBuffer(outBuffer, doubleToByteArray.byteArray, sizeof(doubleToByteArray.byteArray));
	if(err != NoError){
		return err;
	}
	
	return NoError;
}

ItError_t appendCharArrayToBuffer(OutBuffer* outBuffer, const char* array, unsigned int arrayLength){
	ItError_t err;
	
	for(unsigned int idx = 0; idx < arrayLength; idx++){
		err = appendByteToBuffer(outBuffer, array[idx]);
		if(err != NoError){
			return err;
		}
	}
	return NoError;
}

ItError_t appendByteToBuffer(OutBuffer* outBuffer, unsigned char byteToAppend){
	ItError_t err;
	
	if(outBuffer->writeIndex >= sizeof(outBuffer->data)){
		return BufferFull;
	}
	
	if((byteToAppend == TelegramStart) || (byteToAppend == ReplacementMarker)){
		outBuffer->data[outBuffer->writeIndex] = ReplacementMarker;
		outBuffer->writeIndex++;
		
		err = appendByteToBuffer(outBuffer, byteToAppend-1);
		if(err != NoError){
			return err;
		}
	}else{
		outBuffer->data[outBuffer->writeIndex] = byteToAppend;
		outBuffer->writeIndex++;
	}
	
	outBuffer->data[1]=outBuffer->writeIndex;//set telegram length
	return NoError;
}
