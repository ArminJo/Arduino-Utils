/*
 *  HCSR04Demo
 *  .cpp
 *
 *  Gets the USDistance and give tone feedback. Test 2 and 1 pin mode of HY-SRF05 and (modified) HC-SR04.
 *
 *  You can modify the HC-SR04 modules to 1 Pin mode by:
 *  Old module with 3 16 pin chips: Connect Trigger and Echo direct or use a resistor < 4.7 kOhm.
 *        If you remove both 10 kOhm pullup resistors you can use a connecting resistor < 47 kOhm, but I suggest to use 10 kOhm which is more reliable.
 *  Old module with 3 16 pin chips but with no pullup resistors near the connector row: Connect Trigger and Echo with a resistor > 200 Ohm. Use 10 kOhm.
 *  New module with 1 16 pin and 2 8 pin chips: Connect Trigger and Echo by a resistor > 200 Ohm and < 22 kOhm.
 *  All modules: Connect Trigger and Echo by a resistor of 4.7 kOhm.
 *  Some old HY-SRF05 modules of mine cannot be converted, since the output signal going low triggers the next measurement.
 *
 *  Copyright (C) 2020  Armin Joachimsmeyer
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

#include "HCSR04.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define VERSION_EXAMPLE "1.0"

#define USE_1_PIN_MODE_DETECTION_PIN 6 // connect this to ground to enable 1 pin mode

const uint8_t TRIGGER_OUT_PIN = 4;
const uint8_t ECHO_IN_PIN = 5;
const uint8_t SPEAKER_PIN = 11;

void setup() {
    // initialize the digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(USE_1_PIN_MODE_DETECTION_PIN, INPUT_PULLUP);

    Serial.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || defined(SERIAL_PORT_USBVIRTUAL)
    delay(2000); // To be able to connect Serial monitor after reset and before first printout
#endif

    Serial.println(F("START " __FILE__ "\r\nVersion " VERSION_EXAMPLE " from " __DATE__));

    if (digitalRead(USE_1_PIN_MODE_DETECTION_PIN)) {
        Serial.print(F("Pin " STR(USE_1_PIN_MODE_DETECTION_PIN) " is high -> use 2 pin (standard HCSR04) mode at pins "));
        Serial.print(TRIGGER_OUT_PIN);
        Serial.print(F(" (trigger) and pin "));
        Serial.print(ECHO_IN_PIN);
        Serial.println(F(" (echo)"));

        initUSDistancePins(TRIGGER_OUT_PIN, ECHO_IN_PIN);
    } else {
        Serial.print(F("Pin " STR(USE_1_PIN_MODE_DETECTION_PIN) " is low -> Use 1 pin mode at pin "));
        Serial.println(TRIGGER_OUT_PIN);
        initUSDistancePin(TRIGGER_OUT_PIN);
    }
}

void loop() {
    int tCentimeter = getUSDistanceAsCentiMeterWithCentimeterTimeout(300);
    // print distance
    if (tCentimeter >= 300) {
        Serial.println("timeout");
        noTone(SPEAKER_PIN);
    } else {
        Serial.print("cm=");
        Serial.println(tCentimeter);
        tone(SPEAKER_PIN, 500 + tCentimeter * 10);
    }
    delay(200);

    /*
     * Switch between modes. Only for testing, it makes no sense in regular operation.
     */
    if (digitalRead(USE_1_PIN_MODE_DETECTION_PIN)) {
        sHCSR04Mode = HCSR04_MODE_USE_2_PINS;
        pinMode(TRIGGER_OUT_PIN, OUTPUT);
    } else {
        sHCSR04Mode = HCSR04_MODE_USE_1_PIN;
    }
}
