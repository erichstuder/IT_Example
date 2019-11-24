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

#ifndef IT_H
#define IT_H

typedef enum{
	NoError,
	BufferFull,
	ClientUnavailable,
	ClientWriteError,
	NoDataAvailable,
	InvalidCommand,
}ItError_t;

typedef ItError_t (*WriteBytesToClient_t)(const char* const byteArray, const unsigned int byteCount);
typedef ItError_t (*ReadByteFromClient_t)(char* const data);
typedef ItError_t (*InputHandler_t)(const char* const dataIn, double* result);
void itInit(WriteBytesToClient_t writeBytesToClientCallback, ReadByteFromClient_t readByteFromClientCallback, InputHandler_t inputHandlerCallback);

void itTick(void);

#endif //IT_H
