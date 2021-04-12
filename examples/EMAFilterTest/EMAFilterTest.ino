/*
 *  EMAFilterTest.cpp
 *
 *  Generates a square wave or noise and prints 10 filtered values
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

#include "SimpleEMAFilters.h"

//#define TEST_WITH_RANDOM_INPUT

// Choose which filter to show
#ifdef TEST_WITH_RANDOM_INPUT
#define FILTER_TO_SHOW      PRINT_LOW_PASS_1_3_5
#else
#define FILTER_TO_SHOW      PRINT_ALL_FILTERS
//#define FILTER_TO_SHOW      PRINT_LOW_PASS_1_TO_5
//#define FILTER_TO_SHOW      PRINT_LOW_PASS_5_FIX_FLOAT
#endif

// Adjust to see behavior for small values
#define SQUARE_WAVE_VALUE 100 // Here the 3 lines for filter5 are almost identical
//#define SQUARE_WAVE_VALUE 20

#define VERSION_EXAMPLE "1.0"

void setup() {

    Serial.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || defined(SERIAL_PORT_USBVIRTUAL)  || defined(ARDUINO_attiny3217)
    delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
#endif

//    Serial.println(F("START " __FILE__ "\r\nVersion " VERSION_EXAMPLE " from " __DATE__));

    // Print caption for Arduino Plotter
    printFiltersCaption(FILTER_TO_SHOW);
}

void loop() {
    static uint8_t sLoopCount = 0;

    int16_t tInput;
#ifdef TEST_WITH_RANDOM_INPUT
    tInput = random(SQUARE_WAVE_VALUE);
#else
    if (sLoopCount & 0x20) {
        tInput = -SQUARE_WAVE_VALUE;
    } else {
        tInput = SQUARE_WAVE_VALUE;
    }
#endif

    sLoopCount++;

    testFilters(tInput);

#ifdef TEST_WITH_RANDOM_INPUT
    // print every value additionally 3 times to make results more visible
    for (int i = 0; i < 3; ++i) {
        printFiltersResults(FILTER_TO_SHOW);
    }
#endif
    printFiltersResults(FILTER_TO_SHOW);

    delay(10);
}
