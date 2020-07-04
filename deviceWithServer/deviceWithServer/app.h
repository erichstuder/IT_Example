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

#ifndef APP_H
#define APP_H

#include "src/IT_Server/IT_Server/itError.h"

#define APP_SAMPLETIME_US 1000

typedef unsigned long (*GetCurrentMicros_t) (void);
typedef bool (*ByteFromUartAvailable_t) (void);
typedef ItError_t (*ReadByteFromUart_t) (char* const data);
typedef ItError_t (*WriteByteToUart_t) (const unsigned char data);

typedef struct {
	ByteFromUartAvailable_t byteFromUartAvailable;
	ReadByteFromUart_t readByteFromUart;
	WriteByteToUart_t writeByteToUart;
	GetCurrentMicros_t getCurrentMicros;
} AppCallbacks_t;

extern void (*appInit)(AppCallbacks_t callbacks);
extern void (*appTick)(void);

#endif
