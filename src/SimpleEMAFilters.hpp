/*
 *  SimpleEMAFilters.hpp
 *
 *  This program helps to test your signal with up to 10 different filters and show it in Arduino Serial Plotter.
 *
 *  Computes 5 EMA filters, 1 high and band pass filter with exponents 1 to 5
 *  as well as 1 fixed point (around 2 digits) and 1 floating point filter with exponent 5.
 *  It includes print functions compatible with Arduino Serial Plotter.
 *  For a 1 kHz sampling rate (1/1000s sampling interval) we get the following equivalent cutoff (-3db) frequencies:
 * 1/2 -> 160 Hz
 * 1/4 -> 53 Hz (160 / 3)
 * 1/8 -> 22.7 Hz (160 / 7)
 * 1/16 -> 10.6 Hz
 * 1/32 -> 5.13 Hz
 * 1/256 -> 0.624 Hz (160 / 255)
 *
 *  Links:
 *  https://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average
 *  https://www.dsprelated.com/blogimages/RickLyons/Exponential_Averaging_FIGURE2.gif
 *  https://www.norwegiancreations.com/2015/10/tutorial-potentiometers-with-arduino-and-filtering/
 *  https://www.norwegiancreations.com/2016/08/double-exponential-moving-average-filter-speeding-up-the-ema/
 *  https://www.norwegiancreations.com/2016/03/arduino-tutorial-simple-high-pass-band-pass-and-band-stop-filtering/
 *  https://arduinokickstompdrum.wordpress.com/2021/05/14/simple-filtering/
 *  https://github.com/popcornell/Arduino-Multiplierless-EMA-filter
 *  https://github.com/MakeMagazinDE/DigitaleFilter
 *
 *  Copyright (C) 2020-2023  Armin Joachimsmeyer
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
 *  along with this program. If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#ifndef _SIMPLE_EMA_FILTERS_HPP
#define _SIMPLE_EMA_FILTERS_HPP

#include <Arduino.h>

#include "SimpleEMAFilters.h"
#include "digitalWriteFast.h"

//#define MEASURE_TIMING
#if !defined(TIMING_OUT_PIN)
#define TIMING_OUT_PIN  LED_BUILTIN
#endif
#if defined(MEASURE_TIMING)
#define timingPinHigh()   digitalWriteFast(TIMING_OUT_PIN, HIGH)
#define timingPinLow()    digitalWriteFast(TIMING_OUT_PIN, LOW)
#else
#define timingPinHigh()
#define timingPinLow()
#endif

int16_t sInputValueForPrint;

int16_t sLowpass1;
int16_t sLowpass2;
int16_t sDoubleLowpass2;
int16_t sLowpass3;
int16_t sDoubleLowpass3;
int16_t sTripleLowpass3;
int16_t sLowpass4;
int16_t sDoubleLowpass4;
int16_t sLowpass5;
int16_t sDoubleLowpass5;

// only required if we must deal with small values or high exponents (< 32)
int32_t sLowpass3_int32;
int32_t sLowpass5_int32;
int32_t sLowpass8_int32; // The low pass value is in the upper word, the lower word holds the fraction
float sLowpass5_float;
float sLowpass8_float;

/*
 * Convenience functions
 */
void doLowpass1_int16(int16_t *aLowpassAccumulator_int16, int16_t aInputValue) {
    *aLowpassAccumulator_int16 += (aInputValue - *aLowpassAccumulator_int16) >> 1; // alpha = 0.5, cutoff frequency 160 Hz @1kHz sampling rate
}
// An unsigned input value requires a cast
void doLowpass1_int16(int16_t *aLowpassAccumulator_int16, uint16_t aInputValue) {
    *aLowpassAccumulator_int16 += ((int16_t) (aInputValue - *aLowpassAccumulator_int16)) >> 1; // alpha = 0.5, cutoff frequency 160 Hz @1kHz sampling rate
}

void doLowpass2_int16(int16_t *aLowpassAccumulator_int16, int16_t aInputValue) {
    *aLowpassAccumulator_int16 += ((aInputValue - *aLowpassAccumulator_int16) + (1 << 1)) >> 2; // alpha = 0.25, cutoff frequency 53 Hz @1kHz
}

/*
 * alpha = 0.125, cutoff frequency 22.7 Hz @1kHz
 * Has 6 db per octave
 */
void doLowpass3_int16(int16_t *aLowpassAccumulator_int16, int16_t aInputValue) {
    *aLowpassAccumulator_int16 += ((aInputValue - *aLowpassAccumulator_int16) + (1 << 2)) >> 3;
}
/*
 * Has 12 db per octave
 */
void doDoubleLowpass3_int16(int16_t *aLowpassAccumulator_int16, int16_t *aDoubleLowpassAccumulator_int16, int16_t aInputValue) {
    *aLowpassAccumulator_int16 += ((aInputValue - *aLowpassAccumulator_int16) + (1 << 2)) >> 3;
    *aDoubleLowpassAccumulator_int16 += ((*aLowpassAccumulator_int16 - *aDoubleLowpassAccumulator_int16) + (1 << 2)) >> 3;
}

/*
 * Has 18 db per octave
 */
void doTripleLowpass3_int16(int16_t *aLowpassAccumulator_int16, int16_t *aDoubleLowpassAccumulator_int16, int16_t *aTripleLowpassAccumulator_int16, int16_t aInputValue) {
    *aLowpassAccumulator_int16 += ((aInputValue - *aLowpassAccumulator_int16) + (1 << 2)) >> 3;
    *aDoubleLowpassAccumulator_int16 += ((*aLowpassAccumulator_int16 - *aDoubleLowpassAccumulator_int16) + (1 << 2)) >> 3;
    *aTripleLowpassAccumulator_int16 += ((*aDoubleLowpassAccumulator_int16 - *aTripleLowpassAccumulator_int16) + (1 << 2)) >> 3;
}

void doLowpass3_int32(int32_t *aLowpassAccumulator_int32, int16_t aInputValue) {
    *aLowpassAccumulator_int32 += (((int32_t) aInputValue << 8) - *aLowpassAccumulator_int32) >> 3;
}

/*
 * Has 6 db per octave
 */
void doLowpass4_int16(int16_t *aLowpassAccumulator_int16, int16_t aInputValue) {
    *aLowpassAccumulator_int16 += ((aInputValue - *aLowpassAccumulator_int16) + (1 << 3)) >> 4; // alpha = 0.0625, cutoff frequency 10.6 Hz @1kHz
}

/*
 * Has 12 db per octave
 */
void doDoubleLowpass4_int16(int16_t *aLowpassAccumulator_int16, int16_t *aDoubleLowpassAccumulator_int16, int16_t aInputValue) {
    *aLowpassAccumulator_int16 += ((aInputValue - *aLowpassAccumulator_int16) + (1 << 3)) >> 4; // alpha = 0.0625, cutoff frequency 10.6 Hz @1kHz
    *aDoubleLowpassAccumulator_int16 += ((*aLowpassAccumulator_int16 - *aDoubleLowpassAccumulator_int16) + (1 << 3)) >> 4; // alpha = 0.0625, cutoff frequency 10.6 Hz @1kHz
}

// alpha = 1/32 0.03125, cutoff frequency 5.13 Hz @1kHz
void doLowpass5_int16(int16_t *aLowpassAccumulator_int16, int16_t aInputValue) {
    *aLowpassAccumulator_int16 += ((aInputValue - *aLowpassAccumulator_int16) + (1 << 4)) >> 5; // maximum is value - ((1 << 4) -1)
}
void doLowpass5_int32(int32_t *aLowpassAccumulator_int32, int16_t aInputValue) {
    *aLowpassAccumulator_int32 += (((int32_t) aInputValue << 8) - *aLowpassAccumulator_int32) >> 5;
}
void doLowpass5_float(float *aLowpassAccumulator_float, int16_t aInputValue) {
    *aLowpassAccumulator_float += (aInputValue - *aLowpassAccumulator_float) / 32.0; // 24 to 34 us
}

void doLowpass8_int32(int32_t *aLowpassAccumulator_int32, int16_t aInputValue) {
    *aLowpassAccumulator_int32 += ((((int32_t) aInputValue << 8) - *aLowpassAccumulator_int32) + (1 << 7)) >> 8; // alpha = 1/256 = 0.0039, cutoff frequency 0.624 Hz @1kHz
}
void doLowpass8_int32_shift8(int32_t *aLowpassAccumulator_int32, int16_t aInputValue) {
    *aLowpassAccumulator_int32 += ((((int32_t) aInputValue << 16) - *aLowpassAccumulator_int32) + (1 << 7)) >> 8; // alpha = 1/256 = 0.0039, cutoff frequency 0.624 Hz @1kHz
}
void doLowpass8_float(float *aLowpassAccumulator_float, int16_t aInputValue) {
    *aLowpassAccumulator_float += (aInputValue - *aLowpassAccumulator_float) / 256.0; // 24 to 34 us
}

/*
 * Generic function with parameter aDivisorExponent or aDivisor
 */
void doLowpassGeneric_int16(int16_t *aLowpassAccumulator_int16, int16_t aInputValue, uint8_t aDivisorExponent) {
    *aLowpassAccumulator_int16 += ((aInputValue - *aLowpassAccumulator_int16) + (1 << (aDivisorExponent - 1))) >> aDivisorExponent;
}
void doLowpassGeneric_int32(int32_t *aLowpassAccumulator_int32, int16_t aInputValue, uint8_t aDivisorExponent) {
    *aLowpassAccumulator_int32 += ((((int32_t) aInputValue << 8) - *aLowpassAccumulator_int32) + (1 << (aDivisorExponent - 1)))
            >> aDivisorExponent;
}
// This has better resolution but can overflow at input values > 16k
void doLowpassGeneric_int32_shift8(int32_t *aLowpassAccumulator_int32, int16_t aInputValue, uint8_t aDivisorExponent) {
    *aLowpassAccumulator_int32 += ((((int32_t) aInputValue << 16) - *aLowpassAccumulator_int32) + (1 << (aDivisorExponent - 1)))
            >> aDivisorExponent;
}
void doLowpassGeneric_float(float *aLowpassAccumulator_float, int16_t aInputValue, float aDivisor) {
    *aLowpassAccumulator_float += (aInputValue - *aLowpassAccumulator_float) / aDivisor; // 24 to 34 us
}

/*
 * Functions to get the values for the int32 functions / accumulators
 */
int16_t getLowpass_int32(int32_t *aLowpassAccumulator_int32) {
    return *aLowpassAccumulator_int32 >> 8;
}

int16_t getLowpass_int32_shift8(int32_t *aLowpassAccumulator_int32) {
    return *aLowpassAccumulator_int32 >> 16;
}

/*
 * The main test function
 */
void doFiltersStep(int16_t aInputValue) {
    sInputValueForPrint = aInputValue;

#if defined(MEASURE_TIMING)
    pinMode(TIMING_OUT_PIN, OUTPUT);
#endif
    /*
     * int16_t low pass values
     */
    sLowpass1 += (aInputValue - sLowpass1) >> 1; // 1 us, alpha = 0.5, cutoff frequency 160 Hz @1kHz sampling rate

// "+ (1 << 2)" to avoid to much rounding errors and it costs only 1 clock extra
    sLowpass2 += ((aInputValue - sLowpass2) + (1 << 1)) >> 2; // 1.1 us, alpha = 0.25, cutoff frequency 53 Hz @1kHz
    sDoubleLowpass2 += ((sLowpass2 - sDoubleLowpass2) + (1 << 1)) >> 2;

    sLowpass3 += ((aInputValue - sLowpass3) + (1 << 2)) >> 3; // 1.8 us, alpha = 0.125, cutoff frequency 22.7 Hz @1kHz
    sDoubleLowpass3 += ((sLowpass3 - sDoubleLowpass3) + (1 << 2)) >> 3;
    sTripleLowpass3 += ((sDoubleLowpass3 - sTripleLowpass3) + (1 << 2)) >> 3;

    sLowpass4 += ((aInputValue - sLowpass4) + (1 << 3)) >> 4; // 2.2 us, alpha = 0.0625, cutoff frequency 10.6 Hz @1kHz
    sDoubleLowpass4 += ((sLowpass4 - sDoubleLowpass4) + (1 << 3)) >> 4;

    sLowpass5 += ((aInputValue - sLowpass5) + (1 << 4)) >> 5; // 2.5 us, alpha = 1/32 0.03125, cutoff frequency 5.13 Hz @1kHz
    sDoubleLowpass5 += ((sLowpass5 - sDoubleLowpass5) + (1 << 4)) >> 5;

    /*
     * int32_t low pass values for higher exponents
     */
    timingPinHigh();
    sLowpass3_int32 += ((((int32_t) aInputValue) << 8) - sLowpass3_int32) >> 3; // Fixed point
    timingPinLow();

    /*
     * int32_t low pass values for higher resolution
     */
    timingPinHigh();
    sLowpass5_int32 += ((((int32_t) aInputValue) << 8) - sLowpass5_int32) >> 5; // Fixed point 4.2 us
    timingPinLow();

    timingPinHigh();
    // alpha = 1/256 = 0.0039, cutoff frequency 0.624 Hz @1kHz
    sLowpass8_int32 += (((((int32_t) aInputValue) << 8) - sLowpass8_int32) + (1 << 7)) >> 8; // Fixed point 2.0 us because of fast shift :-)
    timingPinLow();

    /*
     * float low pass values with exponent 5 and 8
     */
    timingPinHigh();
    sLowpass5_float += (aInputValue - sLowpass5_float) / 32.0; // 24 to 34 us
    timingPinLow();

    timingPinHigh();
    sLowpass8_float += (aInputValue - sLowpass8_float) / 256.0; //
    timingPinLow();
}

void printFiltersCaption(uint32_t aPrintMask) {
    if (aPrintMask & PRINT_INPUT) {
        Serial.print(F("Input "));
    }
    if (aPrintMask & PRINT_EMA_1) {
        Serial.print(F("Lowpass1 "));
    }
    if (aPrintMask & PRINT_EMA_2) {
        Serial.print(F("Lowpass2 "));
    }
    if (aPrintMask & PRINT_2EMA_2) {
        Serial.print(F("DoubleLowpass2 "));
    }
    if (aPrintMask & PRINT_EMA_3) {
        Serial.print(F("Lowpass3 "));
    }
    if (aPrintMask & PRINT_EMA_3_32) {
        Serial.print(F("Lowpass3_int32 "));
    }
    if (aPrintMask & PRINT_2EMA_3) {
        Serial.print(F("DoubleLowpass3 "));
    }
    if (aPrintMask & PRINT_3EMA_3) {
        Serial.print(F("TripleLowpass3 "));
    }
    if (aPrintMask & PRINT_EMA_4) {
        Serial.print(F("Lowpass4 "));
    }
    if (aPrintMask & PRINT_2EMA_4) {
        Serial.print(F("DoubleLowpass4 "));
    }
    if (aPrintMask & PRINT_EMA_5) {
        Serial.print(F("Lowpass5 "));
    }
    if (aPrintMask & PRINT_EMA_5_32) {
        Serial.print(F("Lowpass5_int32 "));
    }
    if (aPrintMask & PRINT_EMA_5_FLOAT) {
        Serial.print(F("Lowpass5_float "));
    }
    if (aPrintMask & PRINT_2EMA_5) {
        Serial.print(F("DoubleLowpass5 "));
    }
    if (aPrintMask & PRINT_EMA_8_32) {
        Serial.print(F("Lowpass8_int32 "));
    }
    if (aPrintMask & PRINT_EMA_8_FLOAT) {
        Serial.print(F("Lowpass8_float "));
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

void printFiltersResults(uint32_t aPrintMask) {
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
    if (aPrintMask & PRINT_2EMA_2) {
        Serial.print(sDoubleLowpass2);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_EMA_3) {
        Serial.print(sLowpass3);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_EMA_3_32) {
        Serial.print(sLowpass3_int32 >> 8);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_2EMA_3) {
        Serial.print(sDoubleLowpass3);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_3EMA_3) {
        Serial.print(sTripleLowpass3);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_EMA_4) {
        Serial.print(sLowpass4);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_2EMA_4) {
        Serial.print(sDoubleLowpass4);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_EMA_5) {
        Serial.print(sLowpass5);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_EMA_5_32) {
        Serial.print(sLowpass5_int32 >> 8);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_EMA_5_FLOAT) {
        Serial.print(sLowpass5_float);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_2EMA_5) {
        Serial.print(sDoubleLowpass5);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_EMA_8_32) {
        Serial.print(sLowpass8_int32 >> 8);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_EMA_8_FLOAT) {
        Serial.print(sLowpass8_float);
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

#endif // _SIMPLE_EMA_FILTERS_HPP
