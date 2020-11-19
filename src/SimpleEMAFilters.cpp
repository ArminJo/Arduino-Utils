/*
 *  SimpleEMAFilters.cpp
 *
 *  This program helps to test your signal with up to 10 different filters and show it in Arduino Serial Plotter.
 *
 *  Computes 5 EMA filters, 1 high and band pass filter with exponents 1 to 5
 *  as well as 1 fixed point (around 2 digits) and 1 floating point filter with exponent 5.
 *  It includes print functions compatible with Arduino Serial Plotter.
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
#include "digitalWriteFast.h"

#define MEASURE_TIMING
#define TIMING_OUT_PIN  LED_BUILTIN

int16_t sInputValueForPrint;

int16_t sLowpass1;
int16_t sLowpass2;
int16_t sLowpass3;
int16_t sLowpass4;
int16_t sLowpass5;

// only required if we must deal with small values or high exponents (< 32)
int32_t sLowpassShift8;
int32_t sLowpass8FP;
float sLowpassFloat;

void printFiltersCaption(uint16_t aPrintMask) {
    if (aPrintMask & PRINT_INPUT) {
        Serial.print(F("Input "));
    }
    if (aPrintMask & PRINT_EMA_1) {
        Serial.print(F("Lowpass1 "));
    }
    if (aPrintMask & PRINT_EMA_2) {
        Serial.print(F("Lowpass2 "));
    }
    if (aPrintMask & PRINT_EMA_3) {
        Serial.print(F("Lowpass3 "));
    }
    if (aPrintMask & PRINT_EMA_4) {
        Serial.print(F("Lowpass4 "));
    }
    if (aPrintMask & PRINT_EMA_5) {
        Serial.print(F("Lowpass5 "));
    }
    if (aPrintMask & PRINT_EMA_FIX) {
        Serial.print(F("LowpassFP5 "));
    }
    if (aPrintMask & PRINT_EMA_FLOAT) {
        Serial.print(F("LowpassFloat5 "));
    }
    if (aPrintMask & PRINT_EMA_8) {
        Serial.print(F("Lowpass8 "));
    }
    if (aPrintMask & PRINT_HIGH_PASS) {
        Serial.print(F("Highpass1 "));
    }
    if (aPrintMask & PRINT_BAND_PASS_1_3) {
        Serial.print(F("Bandpass1_3 "));
    }
    if (aPrintMask & PRINT_BAND_PASS_3_4) {
        Serial.print(F("Bandpass3_4 "));
    }
    Serial.println();
}

void printFiltersResults(uint16_t aPrintMask) {
    if (aPrintMask & PRINT_INPUT) {
        Serial.print(sInputValueForPrint);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_EMA_1) {
        Serial.print(sLowpass1);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_EMA_2) {
        Serial.print(sLowpass2);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_EMA_3) {
        Serial.print(sLowpass3);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_EMA_4) {
        Serial.print(sLowpass4);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_EMA_5) {
        Serial.print(sLowpass5);
        Serial.print(" ");
    }

    if (aPrintMask & PRINT_EMA_FIX) {
        Serial.print(sLowpassShift8 >> 8);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_EMA_FLOAT) {
        Serial.print(sLowpassFloat);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_EMA_8) {
        Serial.print(sLowpass8FP >> 16);
        Serial.print(" ");
    }

    if (aPrintMask & PRINT_HIGH_PASS) {
        Serial.print(sInputValueForPrint - sLowpass1);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_BAND_PASS_1_3) {
        Serial.print(sLowpass1 - sLowpass3);
        Serial.print(" ");
    }

    if (aPrintMask & PRINT_BAND_PASS_3_4) {
        Serial.print(sLowpass3 - sLowpass4);
        Serial.print(" ");
    }
    Serial.println();
}

void testFilters(int16_t aInputValue) {
    sInputValueForPrint = aInputValue;

#ifdef MEASURE_TIMING
    pinMode(TIMING_OUT_PIN, OUTPUT);
#endif
    sLowpass1 += (aInputValue - sLowpass1) >> 1; // 1 us

// "+ (1 << 2)" to avoid to much rounding errors and it costs only 1 clock extra
    sLowpass2 += ((aInputValue - sLowpass2) + (1 << 1)) >> 2; // 1.1 us

    sLowpass3 += ((aInputValue - sLowpass3) + (1 << 2)) >> 3; // 1.8 us

    sLowpass4 += ((aInputValue - sLowpass4) + (1 << 3)) >> 4; // 2.2 us

    sLowpass5 += ((aInputValue - sLowpass5) + (1 << 4)) >> 5; // 2.5 us

#ifdef MEASURE_TIMING
    digitalWriteFast(TIMING_OUT_PIN, HIGH);
#endif
    sLowpassShift8 += ((((int32_t) aInputValue) << 8) - sLowpassShift8) >> 5; // Fixed point 4.2 us
#ifdef MEASURE_TIMING
    digitalWriteFast(TIMING_OUT_PIN, LOW);
#endif

#ifdef MEASURE_TIMING
    digitalWriteFast(TIMING_OUT_PIN, HIGH);
#endif
    sLowpass8FP += ((((int32_t) aInputValue) << 16) - sLowpass8FP) >> 8; // Fixed point 2.0 us
#ifdef MEASURE_TIMING
    digitalWriteFast(TIMING_OUT_PIN, LOW);
#endif

#ifdef MEASURE_TIMING
    digitalWriteFast(TIMING_OUT_PIN, HIGH);
#endif
    sLowpassFloat += (aInputValue - sLowpassFloat) / 32.0; // 24 to 34 us
#ifdef MEASURE_TIMING
    digitalWriteFast(TIMING_OUT_PIN, LOW);
#endif

}
