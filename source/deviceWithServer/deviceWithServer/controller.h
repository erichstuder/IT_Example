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

#ifndef CONTROLLER_H
#define CONTROLLER_H

extern void (*controllerTick)(void);
void controllerReset(void);
extern void (*setControllerDesiredValue)(float value);
extern void (*setControllerActualValue)(float value);
extern void (*setControllerKp)(float value);
float getControllerKp(void);
extern void (*setControllerKi)(float value);
float getControllerKi(void);
extern float (*getControllerSignal)(void);

#endif
