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

typedef ItError (*WriteByteToClient_t)(const unsigned char data);
typedef ItError (*ReadByteFromClient_t)(unsigned char* const data);
typedef ItError (*CmdHandler_t)(double* result, unsigned long* timeStamp);
typedef ItError (*CmdBufferAppend_t)(const unsigned char dataByte);

extern void (*itInit)(unsigned char* itCmdBuffer, unsigned char itCmdBufferSize, CmdHandler_t cmdHandlerCallback, WriteByteToClient_t writeByteToClientCallback, ReadByteFromClient_t readByteFromClientCallback);
extern void (*itTick)(void);

#endif //IT_H
