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

#include "plant.h"

static const int DeadTimeLength = 20;
static float deadTime[DeadTimeLength + 1] = { 0 };

void plantTick_Implementation(void){
    for (int n = DeadTimeLength; n > 0; n--) {
        deadTime[n] = deadTime[n - 1];
    }
}
void (*plantTick)(void) = plantTick_Implementation;

void setPlantIn_Implementation(float value){
	deadTime[0] = value;
}
void (*setPlantIn)(float value) = setPlantIn_Implementation;

float getPlantOut_Implementation(void){
	return deadTime[DeadTimeLength];
}
float (*getPlantOut)(void) = getPlantOut_Implementation;
