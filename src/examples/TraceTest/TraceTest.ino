/*
 *  TraceTest.cpp
 *
 *  Traces the call to digitalWrite(LED_BUILTIN,HIGH);
 *
 *  Copyright (C) 2015  Armin Joachimsmeyer
 *  Email: armin.joachimsmeyer@gmail.com
 *
 *  This file is part of Arduino-Utils https://github.com/ArminJo/Arduino-Utils.
 *
 *  Arduino-Utils is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#include <Arduino.h>

#include "Trace.cpp.h"

#define VERSION_EXAMPLE "1.0"
#define INFO

void setup() {
    // initialize the digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
#if defined(__AVR_ATmega32U4__)
        while (!Serial)
        ; //delay for Leonardo
#endif

    Serial.println(F("START " __FILE__ "\r\nVersion " VERSION_EXAMPLE " from " __DATE__));

    enableINT0InterruptOnFallingEdge();
    Serial.println(F("Each falling edge on PCI0 will print Program Counter"));
    printTextSectionAddresses();

}

void loop() {
    startTracingSignal();
    digitalWrite(LED_BUILTIN, HIGH);
    stopTracingSignal();
    digitalWrite(LED_BUILTIN, LOW);
}
