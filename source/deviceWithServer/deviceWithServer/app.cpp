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

extern "C" {
#include "it.h"
}

static const unsigned char ItCmdBufferSize = 30;
static unsigned char itCmdBuffer[ItCmdBufferSize];
static GetCurrentMillis_t getMillis;

static ItError_t itCmdHandler(double* result, unsigned long* timeStamp);

void appInit(WriteByteToClient_t writeByteToClient, ReadByteFromClient_t readByteFromClient, GetCurrentMillis_t getCurrentMillis){
	setSquareWaveTickTime(APP_SAMPLETIME);
	setSquareWaveFrequency(0.2f);
	setSquareWaveLevels(2, 10);

	setControllerKp(1);
	setControllerKi(1);

	itInit(itCmdBuffer, ItCmdBufferSize, itCmdHandler, writeByteToClient, readByteFromClient);

	getMillis = getCurrentMillis;
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

static ItError_t itCmdHandler(double* result, unsigned long* timeStamp){
	ItError_t err = ItError_NoError;
	if(strcmp((char*)itCmdBuffer, "desiredValue") == 0){
		*result = (double)getSquareWaveSignal();
		*timeStamp = getMillis();
	}else{
		*result = 0;
		*timeStamp = 0;
		err = ItError_InvalidCommand;
	}
	return err;
}
