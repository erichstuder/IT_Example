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

#include "it.h"

enum ItError{
	NoError,
	BufferFull,
	ClientUnavailable,
	ClientWriteError
};

const byte TelegramStart = 0xAA;
const byte ReplacementMarker = 0xBB;
static ItError (*writeBytesToClientCallback)(const byte* buf, unsigned int bufLen);

struct OutBuffer{
	byte data[255];
	unsigned int writeIndex;
};

ItError createTelegram(OutBuffer outBuffer, char* signalName, double signalData, double timeStampOfSignalData);
ItError appendByteToBuffer(OutBuffer buffer, byte byteToAppend);
ItError appendCharArrayToBuffer(OutBuffer buffer, const char* array, unsigned int arrayLength);
ItError appendDoubleToBuffer(OutBuffer buffer, double doubleToAppend);
void initBuffer(OutBuffer buffer);
void sendError(const char* errMessage, ItError errId);


void itSetup(ItError (*writeBytesToClient)(const byte* buf, unsigned int bufLen)){
	writeBytesToClientCallback = writeBytesToClient;	
}


void itSendToClient(char* signalName, double signalData, double timeStampOfSignalData){
	OutBuffer outBuffer;
	ItError err;

	err = createTelegram(outBuffer, signalName, signalData, timeStampOfSignalData);
	if(err == BufferFull){
		sendError("BufferFull", err);
	}else if(err != NoError){
		sendError("Unexpected Error", err);
	}
	
	err = writeBytesToClientCallback(outBuffer.data, outBuffer.writeIndex);
	if(err == ClientUnavailable){
		//nothing done at the moment. Possible solution would be to have a buffer to store old telegrams
	}else if(err = ClientWriteError){
		sendError("ClientWriteError", err);
		for(;;);//endless loop to stop the program in case we can't send the error to the client
	}else if(err != NoError){
		sendError("Unexpected Error", err);
		for(;;);//endless loop to stop the program in case we can't send the error to the client
	}
}


void sendError(const char* errMessage, ItError errId){
	writeBytesToClientCallback(errMessage, strlen(errMessage));
	writeBytesToClientCallback((const byte*)(&errId), sizeof(errId));
}


ItError createTelegram(OutBuffer outBuffer, char* signalName, double signalData, double timeStampOfSignalData){
	ItError err;
	
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
	}

	return NoError;
}


void initBuffer(OutBuffer buffer){
	buffer.data[0] = TelegramStart;
	buffer.writeIndex = 2;
}


ItError appendDoubleToBuffer(OutBuffer outBuffer, double doubleToAppend){
	ItError err;
	
	union{
		double doubleValue;
		byte byteArray[sizeof(doubleValue)];
	}doubleToByteArray;
	doubleToByteArray.doubleValue = doubleToAppend;
	
	err = appendCharArrayToBuffer(outBuffer, doubleToByteArray.byteArray, sizeof(doubleToByteArray.byteArray));
	if(err != NoError){
		return err;
	}
	
	return NoError;
}


ItError appendCharArrayToBuffer(OutBuffer outBuffer, const char* array, unsigned int arrayLength){
	ItError err;
	
	for(unsigned int idx = 0; idx < arrayLength; idx++){
		err = appendByteToBuffer(outBuffer, array[idx]);
		if(err != NoError){
			return err;
		}
	}
	return NoError;
}


ItError appendByteToBuffer(OutBuffer buffer, byte byteToAppend){
	ItError err;
	
	if(buffer.writeIndex >= sizeof(buffer.data)){
		return BufferFull;
	}
	
	if(byteToAppend == TelegramStart || byteToAppend == ReplacementMarker){
		buffer.data[buffer.writeIndex] = ReplacementMarker;
		buffer.writeIndex++;
		
		err = appendByteToBuffer(buffer, byteToAppend-1);
		if(err != NoError){
			return err;
		}
	}else{
		buffer.data[buffer.writeIndex] = ReplacementMarker;
		buffer.writeIndex++;
	}
	
	buffer.data[1]=buffer.writeIndex;//set telegram length
	return NoError;
}
