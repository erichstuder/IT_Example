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
#include <string.h>

#include "app.h"
#include "squareWave.h"
#include "controller.h"
#include "plant.h"
#include "it.h"

static char itCmdBuffer[IT_CMD_BUFFER_SIZE];
static unsigned char itCmdBufferIndex = 0;
static bool itCmdBufferFull = false;

static void clearItCmdBuffer(void);
static ItError itCmdHandler(double* result);
static ItError itCmdBufferAppend(const char dataByte);

void appInit(WriteBytesToClient_t writeBytesToClient, ReadByteFromClient_t readByteFromClient){
	clearItCmdBuffer();

	setSquareWaveTickTime(1e-3);
	setSquareWaveFrequency(0.2);
	setSquareWaveLevels(2, 10);

	setControllerKp(1);
	setControllerKi(1);

	itInit(writeBytesToClient, readByteFromClient, itCmdHandler, itCmdBufferAppend);
}

void appTick(void){
	setControllerDesiredValue( getSquareWaveSignal() );
	setControllerActualValue( getPlantOut() );
	setPlantIn( getControllerSignal() );

	squareWaveTick();
	controllerTick();
	plantTick();
	itTick();
}

static void clearItCmdBuffer(void) {
	for (unsigned char n = 0; n < IT_CMD_BUFFER_SIZE; n++) {
		itCmdBuffer[n] = '\0';
	}
	itCmdBufferIndex = 0;
	itCmdBufferFull = false;
}

static ItError itCmdHandler(double* result){
	ItError err = ItError::NoError;
	if(strcmp(itCmdBuffer, "desiredValue") == 0){
		*result = (double)getSquareWaveSignal();
	}else{
		*result = 0;
		err = ItError::InvalidCommand;
	}
	clearItCmdBuffer();
	return err;
}

static ItError itCmdBufferAppend(const char dataByte){
	if(itCmdBufferFull == true){
		return ItError::BufferFull;
	}
	itCmdBuffer[itCmdBufferIndex] = dataByte;
	if(itCmdBufferIndex < IT_CMD_BUFFER_SIZE-2){ //leave last byte for zero terminator
		itCmdBufferIndex++;
	}else{
		itCmdBufferFull = true;
	}
	return ItError::NoError;
}
