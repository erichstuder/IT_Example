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

#include "controller.h"

static float signal;
static float desiredValue;
static float actualValue;
static float Kp;
static float Ki;
static float integratorValue;
bool isInitialized = false;

void controllerTick_Implementation(void) {
	if (!isInitialized) {
		controllerReset();
		isInitialized = true;
	}
	float controlError = desiredValue - actualValue;
	integratorValue += Ki * controlError;
	signal = controlError * Kp + integratorValue;
}
void (*controllerTick)(void) = controllerTick_Implementation;

void controllerReset(void) {
	signal = 0;
	integratorValue = 0;
}

void setControllerDesiredValue_Implementation(float value) {
	desiredValue = value;
}
void (*setControllerDesiredValue)(float value) = setControllerDesiredValue_Implementation;

void setControllerActualValue_Implementation(float value) {
	actualValue = value;
}
void (*setControllerActualValue)(float value) = setControllerActualValue_Implementation;

void setControllerKp_Implementation(float value) {
	Kp = value;
}
void (*setControllerKp)(float value) = setControllerKp_Implementation;

void setControllerKi_Implementation(float value) {
	Ki = value;
}
void (*setControllerKi)(float value) = setControllerKi_Implementation;

float getControllerSignal_Implementation(void) {
	return signal;
}
float (*getControllerSignal)(void) = getControllerSignal_Implementation;