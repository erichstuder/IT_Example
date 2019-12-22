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
#include <Arduino.h>
#include "app.h"

static bool timerEvent = false;

static inline void setBuiltinLedOn(void);
static inline void setBuiltinLedOff(void);
static inline void timerSetup(void);
static inline ItError_t writeByteToUart(const unsigned char data);
static inline ItError_t readByteFromUart(unsigned char* const data);

void setup(void){
	timerSetup();

	Serial.begin(9600);
	appInit(writeByteToUart, readByteFromUart, millis);
}

void loop(void){
	if(!timerEvent){
		setBuiltinLedOff();
		return;
	}
	
	setBuiltinLedOn();
 
	timerEvent=false;
    appTick();
}

static inline void initBuiltinLed(void){
  pinMode(LED_BUILTIN, OUTPUT);
}

static inline void setBuiltinLedOn(void){
  initBuiltinLed();
  digitalWrite(LED_BUILTIN, HIGH);
}

static inline void setBuiltinLedOff(void){
  initBuiltinLed();
  digitalWrite(LED_BUILTIN, LOW);
}

static inline void timerSetup(void){
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

static inline ItError_t writeByteToUart(const unsigned char data){
	if(!Serial){ //TODO: brauchts das?
		return ItError_ClientUnavailable;
	}
	if(Serial.write(data) != 1){
		return ItError_ClientWriteError;
	}
	return ItError_NoError;
}

static inline bool bytesFromUartAvailable(void){
	if(!Serial){ //TODO: brauchts das?
		return false;
	}
	return Serial.available() > 0;
}

inline static ItError_t readByteFromUart(unsigned char* const data){
	if(!Serial){ //TODO: brauchts das?
		return ItError_ClientUnavailable;
	}
	int incomingByte = Serial.read();
	if(incomingByte == -1){
		return ItError_NoDataAvailable;
		
	}else{
		*data = (unsigned char)incomingByte;
	}
	return ItError_NoError;
}
