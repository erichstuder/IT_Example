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

#include "math.h"

static float tickTime = 0;
static float frequency = 0;
static float level1 = 0;
static float level2 = 1;
static unsigned short currentTicks = 0;
static float* outputSignal = 0;

static void driveOutputSignal(void);
static void toggleOutputSignal(void);

void squareWaveTick(void) {
	unsigned short ticksUntilToggle;
	driveOutputSignal();
}

void squareWaveReset(void) {
	currentTicks = 0;
	outputSignal = &level1;
}

static void driveOutputSignal(void) {
	if (frequency == 0 || tickTime == 0) {
		return;
	}
	unsigned short ticksUntilToggle = (unsigned short)round(1 / (2 * frequency * tickTime));
	if (currentTicks >= ticksUntilToggle) {
		currentTicks = 0;
		toggleOutputSignal();
	}
	currentTicks++;
}

static void toggleOutputSignal(void) {
	if (outputSignal == &level1) {
		outputSignal = &level2;
	}
	else {
		outputSignal = &level1;
	}
}

float getSquareWaveSignal(void) {
	return *outputSignal;
}

void setSquareWaveTickTime(float time) {
	tickTime = time;
}

void setSquareWaveFrequency(float signalFrequency) {
	frequency = signalFrequency;
}

void setSquareWaveLevels(float signalLevel1, float signalLevel2) {
	level1 = signalLevel1;
	level2 = signalLevel2;
}
 