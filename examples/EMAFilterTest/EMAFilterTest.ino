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
//#define TEST_WITH_SINE_INPUT
//#define TEST_WITH_TRIANGLE_INPUT

// Choose which filter to show
#if defined(TEST_WITH_RANDOM_INPUT)
#define FILTER_TO_SHOW      PRINT_LOW_PASS_1_3_5
#elif defined(TEST_WITH_SINE_INPUT) || defined(TEST_WITH_TRIANGLE_INPUT)
#define FILTER_TO_SHOW      PRINT_ALL_SIMPLE_FILTERS
#else
#define FILTER_TO_SHOW      PRINT_ALL_FILTERS
//#define FILTER_TO_SHOW      PRINT_LOW_PASS_1_TO_5
//#define FILTER_TO_SHOW      PRINT_LOW_PASS_5_FIX_FLOAT
#endif

// Adjust to see behavior for small values
#define MAXIMUM_INPUT_VALUE 100 // Here the 3 lines for filter5 are almost identical
//#define MAXIMUM_INPUT_VALUE 16385L // Here we get overflows for square wave at `InputValue - Lowpass3` while changing sign
//#define MAXIMUM_INPUT_VALUE 16384L // Here we get overflows for square wave at `InputValue - Lowpass3` while changing from - to + value
//#define MAXIMUM_INPUT_VALUE 16383L // The maximum value without overflows for square wave and fast 16 bit filters
//#define MAXIMUM_INPUT_VALUE 20

#define INPUT_PERIOD_SAMPLES 50 // Signal with 50 Samples per period

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

/*
 * Create a signal with a period of 50, which is equivalent to a frequency of 20 Hz for a 1 kHz sampling frequency.
 * Except for random :-)
 */
void loop() {
    static uint8_t sLoopCount = 0; // 0 to 49
    static int16_t sInput;

#ifdef TEST_WITH_RANDOM_INPUT
    sInput = random(MAXIMUM_INPUT_VALUE);

#elif defined(TEST_WITH_SINE_INPUT)
    sInput = MAXIMUM_INPUT_VALUE * sin(((float) sLoopCount * TWO_PI) / 50.0);

#elif defined(TEST_WITH_TRIANGLE_INPUT)
    if (sLoopCount < INPUT_PERIOD_SAMPLES / 2) {
        sInput = (((MAXIMUM_INPUT_VALUE * 2) * sLoopCount) / (INPUT_PERIOD_SAMPLES / 2)) - MAXIMUM_INPUT_VALUE;
    } else {
        sInput = (3 * MAXIMUM_INPUT_VALUE) - (((MAXIMUM_INPUT_VALUE * 2) * sLoopCount) / (INPUT_PERIOD_SAMPLES / 2));
    }
#else
    if (sLoopCount < INPUT_PERIOD_SAMPLES / 2) {
        sInput = MAXIMUM_INPUT_VALUE;
    } else {
        sInput = -MAXIMUM_INPUT_VALUE;
    }
#endif

    doFiltersStep(sInput);
    sLoopCount++;
    if (sLoopCount >= INPUT_PERIOD_SAMPLES) {
        sLoopCount = 0;
    }

#ifdef TEST_WITH_RANDOM_INPUT
    // print every value additionally 3 times to make results more visible
    for (int i = 0; i < 3; ++i) {
        printFiltersResults(FILTER_TO_SHOW);
    }
#endif
    printFiltersResults(FILTER_TO_SHOW);

    delay(10);
}
