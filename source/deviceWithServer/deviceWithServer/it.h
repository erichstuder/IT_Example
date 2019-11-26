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

enum class ItError {
	NoError,
	BufferFull,
	ClientUnavailable,
	ClientWriteError,
	NoDataAvailable,
	InvalidCommand,
};

typedef ItError (*WriteBytesToClient_t)(const char* const byteArray, const unsigned char byteCount);
typedef ItError (*ReadByteFromClient_t)(char* const data);
typedef ItError (*CmdHandler_t)(double* result);
typedef ItError (*CmdBufferAppend_t)(const char dataByte);

extern void (*itInit)(WriteBytesToClient_t writeBytesToClientCallback, ReadByteFromClient_t readByteFromClientCallback, CmdHandler_t cmdHandlerCallback, CmdBufferAppend_t cmdBufferAppendCallback);
extern void (*itTick)(void);

#endif //IT_H
