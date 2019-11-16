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

const int DelayLength = 10;
float delay[DelayLength] = { 0.0f };
float plantIn, plantOut;

void plantTick(void){
	plantOut = delay[DelayLength-1];
	int n;
	for(n=DelayLength-1; n>0; n--){
		delay[n] = delay[n-1];
	}
	delay[0] = plantIn;
}

void setPlantIn(float value){
	plantIn = value;
}

float getPlantOut(void){
	return plantOut;
}