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

#ifndef IT_TELEGRAM_H
#define IT_TELEGRAM_H

#include "itError.h"

#define TelegramStart 0xAA
#define TelegramEnd 0xBB
#define ReplacementMarker 0xCC

typedef ItError_t(*WriteByteToClient_t) (const unsigned char data);

void itTelegramInit(WriteByteToClient_t writeByteToClientCallback);
ItError_t itSendValueTelegram_Int8(const char* const valueName, signed char value, unsigned long timestamp);
ItError_t itSendValueTelegram_Uint8(const char* const valueName, unsigned char value, unsigned long timestamp);
ItError_t itSendValueTelegram_Ulong(const char* const valueName, unsigned long value, unsigned long timestamp);
ItError_t itSendValueTelegram_Float(const char* const valueName, float value, unsigned long timestamp);
ItError_t itSendStringTelegram(const char* const str);

#endif
