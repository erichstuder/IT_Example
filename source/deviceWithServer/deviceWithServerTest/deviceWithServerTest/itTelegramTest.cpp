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
 * along with this program.  If not, see <://www.gnu.org/licenses/>.
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTestExt/MockSupport_c.h"
extern "C" {
#include "itTelegram.h"
}

ItError_t writeByteToClient_Mock(const unsigned char byte) {
    mock_c()->actualCall("writeByteToClient_Mock")
        ->withIntParameters("byte", byte);
#pragma warning(push)
#pragma warning(disable : 26812)
    return ItError_NoError;
#pragma warning(pop)
}

TEST_GROUP(ItTelegramTest) {
    void setup() {
        itTelegramInit(writeByteToClient_Mock);
    }

    void teardown() {
        mock().checkExpectations();
        mock().clear();
    }
};

TEST(ItTelegramTest, Int8) {
    unsigned char expectedTelegram[] = {0xAA, 0x01, 'm', 'y', 'V', 'a', 'l', 'u', 'e', '\0', 1, 0x23, 0x78, 0x56, 0x34, 0x12, 0xBB};
    mock().strictOrder();
    for (int n = 0; n < sizeof(expectedTelegram); n++) {
        mock().expectOneCall("writeByteToClient_Mock")
            .withParameter("byte", expectedTelegram[n]);
    }
    itSendValueTelegram_Int8("myValue", 0x23, 0x12345678);
}

TEST(ItTelegramTest, Uint8) {
    unsigned char expectedTelegram[] = { 0xAA, 0x01, 'm', 'y', 'V', 'a', 'l', 'u', 'e', '\0', 2, 0xFF, 0xDD, 0xCC, 0xCB, 0xCC, 0xBA, 0xCC, 0xA9, 0xBB };
    mock().strictOrder();
    for (int n = 0; n < sizeof(expectedTelegram); n++) {
        mock().expectOneCall("writeByteToClient_Mock")
            .withParameter("byte", expectedTelegram[n]);
    }
    itSendValueTelegram_Uint8("myValue", 0xFF, 0xAABBCCDD);
}

TEST(ItTelegramTest, Ulong) {
    unsigned char expectedTelegram[] = { 0xAA, 0x01, 'm', 'y', 'V', 'a', 'l', 'u', 'e', '\0', 3, 0x33, 0x22, 0x11, 0x00, 0xEE, 0xEE, 0xEE, 0xEE, 0xBB };
    mock().strictOrder();
    for (int n = 0; n < sizeof(expectedTelegram); n++) {
        mock().expectOneCall("writeByteToClient_Mock")
            .withParameter("byte", expectedTelegram[n]);
    }
    itSendValueTelegram_Ulong("myValue", 0x00112233, 0xEEEEEEEE);
}

TEST(ItTelegramTest, Float) {
    unsigned char expectedTelegram[] = { 0xAA, 0x01, 'm', 'y', 'V', 'a', 'l', 'u', 'e', '\0', 4, 0x48, 0xe1, 0xcc, 0xA9, 0x40, 0x66, 0x66, 0x66, 0x66, 0xBB };
    mock().strictOrder();
    for (int n = 0; n < sizeof(expectedTelegram); n++) {
        mock().expectOneCall("writeByteToClient_Mock")
            .withParameter("byte", expectedTelegram[n]);
    }
    itSendValueTelegram_Float("myValue", 5.34f, 0x66666666);
}

TEST(ItTelegramTest, String) {
    unsigned char expectedTelegram[] = { 0xAA, 0x02, 'h', 'e', 'l', 'l', 'o', '\0', 0xBB };
    mock().strictOrder();
    for (int n = 0; n < sizeof(expectedTelegram); n++) {
        mock().expectOneCall("writeByteToClient_Mock")
            .withParameter("byte", expectedTelegram[n]);
    }
    itSendStringTelegram("hello");
}
