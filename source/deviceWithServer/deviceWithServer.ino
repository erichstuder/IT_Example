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

//enable debugging with IT
#define IT_ENABLED

#include "app/app.h"
#include "app/app.cpp"
#include "it/it.h"

boolean timerEvent;
struct appIn_T appIn;
struct appOut_T appOut;

void setup(){
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);
//	Serial.begin(9600);
	itSetup();
	setupTimer();
}

void loop(){
	if(!timerEvent){
		return;
	}
	#ifdef IT_ENABLED
		itToClient("timeNow", millis());
	#endif
	
	timerEvent=false;
	appIn.millis_ms = millis();
	appTick(appIn, &appOut);
/*	while(!Serial && !timerEventPending()){
		; // wait for serial port to connect. Needed for native USB port only
	}
	if(Serial){
		Serial.println(appIn.millis_ms);
		Serial.println((long)appOut.squareMillis);
		Serial.println(appOut.sqrtMillis);	
		Serial.println("");
	}*/
}

void setupTimer(){
	timerEvent=false;
	#if APP_SAMPLETIME==1
		TCCR1A = 0; //for any reason, this must be done!!
		TCCR1B = _BV(WGM12) | _BV(CS12) | _BV(CS10); //match on value of OCR1A and divide clock by 1024
		OCR1A = 15625; //1000ms
		TIMSK1 = _BV(OCIE1A); //enable interrupt
	#else
		#error APP_SAMPLETIME not supported
	#endif
}

boolean timerEventPending(){
	return timerEvent;
}

ISR(TIMER1_COMPA_vect){
	timerEvent=true;
}
