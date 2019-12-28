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

static GetCurrentMillis_t getMillis;

static ItSignal_t itSignals[] = {
	{
		"desiredValue",
		ItValueType_Float,
		(void (*)(void)) getSquareWaveSignal,
		NULL,
	},
	{
		"time",
		ItValueType_Ulong,
		(void (*)(void)) getMillis,
		NULL,
	},
};
static const unsigned char ItSignalCount = sizeof(itSignals) / sizeof(itSignals[0]);
static char itInputBuffer[30];

static ReadByteFromUart_t readByteFromUart;
static WriteByteToUart_t writeByteToUart;

static ItError_t readByteFromUart_withItError(char* const data);
static ItError_t writeByteToUart_withItError(const unsigned char data);

void appInit_Implementation(AppCallbacks_t callbacks){
	setSquareWaveTickTime(APP_SAMPLETIME);
	setSquareWaveFrequency(0.2f);
	setSquareWaveLevels(2, 10);

	setControllerKp(1);
	setControllerKi(1);

	ItCallbacks_t itCallbacks;
	itCallbacks.byteFromClientAvailable = callbacks.byteFromUartAvailable;
	readByteFromUart = callbacks.readByteFromUart;
	itCallbacks.readByteFromClient = readByteFromUart_withItError;
	writeByteToUart = callbacks.writeByteToUart;
	itCallbacks.writeByteToClient = writeByteToUart_withItError;
	itCallbacks.getTimestamp = callbacks.getCurrentMillis;
	ItParameters_t itParameters;
	itParameters.itInputBuffer = itInputBuffer;
	itParameters.itInputBufferSize = sizeof(itInputBuffer);
	itParameters.itSignals = itSignals;
	itParameters.itSignalCount = ItSignalCount;
	itInit(&itParameters, &itCallbacks);
}
void (*appInit)(AppCallbacks_t callbacks) = appInit_Implementation;

static ItError_t readByteFromUart_withItError(char* const data) {
	AppError err = readByteFromUart(data);
	if (err == AppError::UartUnavailable) {
		return ItError_ClientUnavailable;
	}
	else if (err == AppError::NoDataAvailable) {
		return ItError_NoDataAvailable;
	}
	else if (err != AppError::NoError) {
		return ItError_Unknown;
	}
	return ItError_NoError;
}

static ItError_t writeByteToUart_withItError(const unsigned char data) {
	AppError err = writeByteToUart(data);
	if (err == AppError::UartUnavailable) {
		return ItError_ClientUnavailable;
	}
	else if (err == AppError::UartWriteError) {
		return ItError_ClientWriteError;
	}
	else if (err != AppError::NoError) {
		return ItError_Unknown;
	}
	return ItError_NoError;
}

void appTick_Implementation(void){
	setControllerDesiredValue( getSquareWaveSignal() );
	setControllerActualValue( getPlantOut() );
	setPlantIn( getControllerSignal() );

	squareWaveTick();
	controllerTick();
	plantTick();
	itTick();
}
void (*appTick)(void) = appTick_Implementation;
