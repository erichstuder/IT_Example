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

#include <math.h>
#include "app.h"

extern "C" void appTick(struct appIn_T appIn, struct appOut_T* appOut){
	unsigned long millis_ms = appIn.millis_ms;
	appOut->squareMillis = (unsigned long long)millis_ms * (unsigned long long)millis_ms;
	appOut->sqrtMillis = sqrt(millis_ms);
}
