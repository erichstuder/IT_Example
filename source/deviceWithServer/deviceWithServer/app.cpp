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

static ItSignal_t itSignals[] = {
	{
		"desiredValue",
		ItValueType_Float,
		(void (*)(void)) getSquareWaveSignal,
		NULL,
	},
	{
		"actualValue",
		ItValueType_Float,
		(void (*)(void)) getPlantOut,
		NULL,
	},
	{
		"Kp",
		ItValueType_Float,
		(void (*)(void)) getControllerKp,
		(void (*)(void)) setControllerKp,
	},
	{
		"Ki",
		ItValueType_Float,
		(void (*)(void)) getControllerKi,
		(void (*)(void)) setControllerKi,
	},
};
static const unsigned char ItSignalCount = sizeof(itSignals) / sizeof(itSignals[0]);
static char itInputBuffer[30];

void appInit_Implementation(AppCallbacks_t callbacks){
	setSquareWaveTickTime(float(APP_SAMPLETIME_US / 1e6));
	setSquareWaveFrequency(0.5f);
	setSquareWaveLevels(2, 10);

	setControllerKp(0.0f);
	setControllerKi(0.05f);

	ItCallbacks_t itCallbacks;
	itCallbacks.byteFromClientAvailable = callbacks.byteFromUartAvailable;
	itCallbacks.readByteFromClient = callbacks.readByteFromUart;
	itCallbacks.writeByteToClient = callbacks.writeByteToUart;
	itCallbacks.getTimestamp = callbacks.getCurrentMicros;
	ItParameters_t itParameters;
	itParameters.itInputBuffer = itInputBuffer;
	itParameters.itInputBufferSize = sizeof(itInputBuffer);
	itParameters.itSignals = itSignals;
	itParameters.itSignalCount = ItSignalCount;
	itInit(&itParameters, &itCallbacks);
}
void (*appInit)(AppCallbacks_t callbacks) = appInit_Implementation;

void appTick_Implementation(void){
	squareWaveTick();
	setControllerDesiredValue(getSquareWaveSignal());
	setControllerActualValue(getPlantOut());
	controllerTick();
	setPlantIn(getControllerSignal());
	plantTick();

	itTick();
}
void (*appTick)(void) = appTick_Implementation;
