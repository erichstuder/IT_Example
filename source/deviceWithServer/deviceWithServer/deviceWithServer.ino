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

//Board: Arduino Leonardo

#include "squareWave.h"
#include "controller.h"
#include "plant.h"
#include "app.h"
#include "it.h"

boolean timerEvent;
unsigned char cnt = 0;
//AppIn_T appIn;
//AppOut_T appOut;

ItError_t writeBytesToUart(const unsigned char* const byteArray, const unsigned int byteCount);
ItError_t readByteFromUart(unsigned char* const data);

void setup(void){
	timerSetup();

	//debugging infrastructure
	Serial.begin(9600);
	appInit(writeBytesToUart, readByteFromUart);
}

void loop(void){
	if(!timerEvent){
		setBuiltinLedOff();
		return;
	}

	/*if(cnt < 5){//debug
		cnt++;
	}else{
		cnt = 0;
		Serial.println("5s timerEvent");
	}*/
	
	setBuiltinLedOn();
 
	timerEvent=false;
//	appIn.millis_ms = millis();
//	appTick(appIn, &appOut);
    appTick();

    #ifdef IT_ENABLED
        itSendToClient("squareMillis", appOut.squareMillis, millis());
        itSendToClient("sqrtMillis", appOut.sqrtMillis, millis());
    #endif
}

void setBuiltinLedOn(void){
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void setBuiltinLedOff(void){
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void timerSetup(void){
	timerEvent=false;
	#if APP_SAMPLETIME == 1
		TCCR1A = 0; //for any reason, this must be done!!
		TCCR1B = _BV(WGM12) | _BV(CS12) | _BV(CS10); //match on value of OCR1A and divide clock by 1024
		OCR1A = 15625; //1000ms
		TIMSK1 = _BV(OCIE1A); //enable interrupt
	/*#elif APP_SAMPLETIME == 1e-3
		TCCR1A = 0;
		TCCR1B = _BV(WGM12) | _BV(CS10); //match on value of OCR1A and divide clock by 1
		OCR1A = 16000; //1ms
		TIMSK1 = _BV(OCIE1A); //enable interrupt*/
	#else
		#error APP_SAMPLETIME not supported
	#endif
}

ISR(TIMER1_COMPA_vect){
	timerEvent = true;
}

ItError_t writeBytesToUart(const char* const byteArray, const unsigned int byteCount){
	if(!Serial){
		return ClientUnavailable;
	}
	if(Serial.write(byteArray, byteCount) != byteCount){
		return ClientWriteError;
	}
	return NoError;
}

ItError_t readByteFromUart(char* const data){
	if(!Serial){ //TODO: brauchts das?
		return ClientUnavailable;
	}
	if(Serial.available() > 0){
		*data = (unsigned char) Serial.read();
	}else{
		return NoDataAvailable;
	}
	return NoError;
}
