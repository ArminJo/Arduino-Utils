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
 * Available filter sets are:
 * 0 Significant filters
 * 1 All Lowpass
 * 2 Low and Highpass
 * 3 Bandpass and Bandstop
 * 4 Lowpass 1, 3, 5, 8
 * 5 Lowpass 16 + 32
 * 6 Higher Order Lowpass 16
 * 7 Bi-Quad filters with damping 0
 *
 *  Links:
 *  Wikipedia:
 *  https://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average
 *  https://en.wikipedia.org/wiki/State_variable_filter
 *  https://en.wikipedia.org/wiki/Electronic_filter_topology#Biquad_filter_topology
 *
 *  Arduino Tutorial:
 *  https://www.norwegiancreations.com/2015/10/tutorial-potentiometers-with-arduino-and-filtering/
 *  https://www.norwegiancreations.com/2016/08/double-exponential-moving-average-filter-speeding-up-the-ema/
 *  https://www.norwegiancreations.com/2016/03/arduino-tutorial-simple-high-pass-band-pass-and-band-stop-filtering/
 *
 *  Tutorial including sounds and Biquad:
 *  https://arduinokickstompdrum.wordpress.com/2021/05/14/simple-filtering/
 *
 *  German sources:
 *  https://github.com/MakeMagazinDE/DigitaleFilter
 *  The German article behind paywall
 *  https://www.heise.de/select/make/2022/6/2224920235757833999
 *
 *  Some frequency / phase graphs:
 *  https://github.com/popcornell/Arduino-Multiplierless-EMA-filter
 *
 *  Frequency graph for shift 1 to 5
 *  https://www.dsprelated.com/blogimages/RickLyons/Exponential_Averaging_FIGURE2.gif
 *
 *
 *
 *  Copyright (C) 2020-2026  Armin Joachimsmeyer
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
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
#endif

/*
 * Storage for implementation of some fixed fast LOWPASS filters
 */
int16_t sLowpass1;
int16_t sLowpass2;
int16_t sLowpass3;
int16_t sLowpass4;
int16_t sLowpass5;
int16_t sLowpass6;
int16_t sDoubleLowpass3;
int16_t sDoubleLowpass4;
int16_t sDoubleLowpass5;
int16_t sTripleLowpass3;
// only required if we must deal with small values or high exponents (> 32)
int32_t sLowpass2_int32_shift8;
int32_t sLowpass3_int32_shift8;
int32_t sLowpass5_int32_shift8;
int32_t sLowpass8_int32_shift8; // The low pass value is in the upper word, the lower word holds the fraction
float sLowpass5_float;
float sLowpass8_float;

/*
 * Biquad
 */
struct BiquadFilter16Struct sBiQuad_int16;
struct BiquadFilter32Struct sBiQuad_int32;

/*
 * Variables for filter demo
 */
int16_t sInputValueForPrint;

uint32_t FilterSelectionArray[8] {
PRINT_SIGNIFICANT_FILTERS, PRINT_ALL_LOW_PASS, PRINT_LOW_HIGH_PASS, PRINT_BAND_PASS_AND_BAND_STOP, PRINT_LOW_PASS_1_3_5_8,
PRINT_LOW_PASS_16_32, PRINT_HIGHER_ORDER_LOW_PASS, PRINT_ALL_BI_QUAD };

const char *FilterSelectionStringArray[8] { "SIGNIFICANT_FILTERS", "ALL_LOW_PASS", "LOW_AND_HIGH_PASS", "BAND_PASS_AND_BAND_STOP",
        "LOW_PASS_1_3_5_8", "LOW_PASS_16_32", "HIGHER_ORDER_LOW_PASS_16", "BI_QUAD_FILTERS" };

/**************************************************************************************************************
 * Generic LOWPASS filter functions
 * Correct formula: alpha = 1 - e ^ -((2&pi; * CutoffFrequency) / SampleFrequency)
 * Simplified formula for small alpha is: CutoffFrequency = (SampleFrequency / (2&pi; * ((1/alpha) - 1));
 * For 1 kHz sampling rate, we get:
 * - For alpha 1/2 | shift 1 -> 160 Hz - correct value is 110 Hz
 * -   1/4 | >>2 -> 53 Hz   (=160 Hz / 3)  - 46 Hz    // 3 = ((1 / (1/4)) - 1)
 * -   1/8 | >>3 -> 22.7 Hz (=160 Hz / 7)  - 21.2 Hz  // 7 = ((1 / (1/8)) - 1)
 * -  1/16 | >>4 -> 10.6 Hz
 * -  1/32 | >>5 ->  5.13 Hz
 * -  1/64 | >>6 ->  2.54 Hz
 * - 1/128 | >>7 ->  1,26 Hz
 * - 1/256 | >>8 ->  0.624 Hz
 *
 **************************************************************************************************************/
/*
 * Generic function with parameter aShiftValue which is the divisor exponent
 * !!! This function is NOT slower than the LOWPASS functions with fixed shifts :-) !!!
 * Rounding is done by always adding a positive value before shifting, because shifted negative numbers are rounded towards -infinity
 * E.g. +13 to +15 >> 2 = 3 (rounded towards 0).  -13 to -15 >> 2 = -4 (rounded towards -infinity)
 * @param aShiftValue  Divisor exponent value i.e. >>2 = *1/4 = *1/2^2,   >>3 = *1/8 = *1/2^3
 */
void doLowpassShift_int16(int16_t *aLowpassAccumulator_int16, int16_t aInputValue, uint8_t aShiftValue) {
    *aLowpassAccumulator_int16 += ((aInputValue - *aLowpassAccumulator_int16) + (1 << (aShiftValue - 1))) >> aShiftValue;
}

/**
 * Around 0.7 us slower than doLowpassShift_int16
 * @param aAlpha_shift8
 * - 64 = >>2 -> 53 Hz @1kHz sampling rate
 * - 32 = >>3 -> 22.7 Hz
 * - 20 = >>3.7 ->  12.48 Hz
 * - 16 = >>4 -> 10.6 Hz
 * - 10 = >>4.7 ->  6.24 Hz
 * -  8 = >>5 ->  5.13 Hz
 * -  4 = >>6 ->  2.54 Hz
 * -  2 = >>7 ->  1,26 Hz
 * -  1 = >>8 ->  0.624 Hz
 */
void doLowpass_int16(int16_t *aLowpassAccumulator_int16, int16_t aInputValue, uint8_t aAlpha_shift8) {
    *aLowpassAccumulator_int16 += (((int32_t) (aInputValue - *aLowpassAccumulator_int16) * aAlpha_shift8) + (aAlpha_shift8 / 2))
            >> 8;
}

/*
 * Has 12 db per octave and computes lowpass and double lowpass
 */
void doDoubleLowpass_int16(int16_t *aLowpassAccumulator_int16, int16_t *aDoubleLowpassAccumulator_int16, int16_t aInputValue,
        uint8_t aShiftValue) {
    *aLowpassAccumulator_int16 += ((aInputValue - *aLowpassAccumulator_int16) + (1 << (aShiftValue - 1))) >> aShiftValue;
    *aDoubleLowpassAccumulator_int16 += ((*aLowpassAccumulator_int16 - *aDoubleLowpassAccumulator_int16) + (1 << (aShiftValue - 1)))
            >> aShiftValue;
}

/*
 * Has 18 db per octave
 */
void doTripleLowpass_int16(int16_t *aLowpassAccumulator_int16, int16_t *aDoubleLowpassAccumulator_int16,
        int16_t *aTripleLowpassAccumulator_int16, int16_t aInputValue, uint8_t aShiftValue) {
    *aLowpassAccumulator_int16 += ((aInputValue - *aLowpassAccumulator_int16) + (1 << (aShiftValue - 1))) >> aShiftValue;
    *aDoubleLowpassAccumulator_int16 += ((*aLowpassAccumulator_int16 - *aDoubleLowpassAccumulator_int16) + (1 << (aShiftValue - 1)))
            >> aShiftValue;
    *aTripleLowpassAccumulator_int16 += ((*aDoubleLowpassAccumulator_int16 - *aTripleLowpassAccumulator_int16)
            + (1 << (aShiftValue - 1))) >> aShiftValue;
}

/******************************************************
 * int32 functions with (24,8) fixed point accumulator
 ******************************************************/
/**
 * Formula for processing time is: 1.75 us + 0.44 us per shift. i.e. 2.63 us for >> 2 and 4.81 us for >> 7. 2.13 for >> 8
 */
void doLowpassShift_int32_shift8(int32_t *aLowpassAccumulator_int32_shift8, int16_t aInputValue, uint8_t aShiftValue) {
    *aLowpassAccumulator_int32_shift8 += ((((int32_t) aInputValue << 8) - *aLowpassAccumulator_int32_shift8)) >> aShiftValue;
}

/**
 * 7.13 us
 * @param aAlpha_shift8
 * - 64 = >>2 -> 53 Hz @1kHz sampling rate
 * - 32 = >>3 -> 22.7 Hz
 * - 20 = >>3.7 ->  12.48 Hz
 * - 16 = >>4 -> 10.6 Hz
 * - 10 = >>4.7 ->  6.24 Hz
 * -  8 = >>5 ->  5.13 Hz
 * -  4 = >>6 ->  2.54 Hz
 * -  2 = >>7 ->  1,26 Hz
 * -  1 = >>8 ->  0.624 Hz
 */
#if defined(OPTIMIZE_WITH_INLINE_FUNCTIONS) // from 7.13 us down to 3.44 us to 4.75 us
__attribute__((always_inline)) inline
#endif
void doLowpass_int32_shift8(int32_t *aLowpassAccumulator_int32_shift8, int16_t aInputValue, uint8_t aAlpha_shift8) {
    *aLowpassAccumulator_int32_shift8 += (((((int32_t) aInputValue << 8) - *aLowpassAccumulator_int32_shift8)) * aAlpha_shift8)
            >> 8;
}

void doDoubleLowpass_int32_shift8(int32_t *aLowpassAccumulator_int32_shift8, int32_t *aDoubleLowpassAccumulator_int32_shift8,
        int16_t aInputValue, uint8_t aShiftValue) {
    *aLowpassAccumulator_int32_shift8 += ((((int32_t) aInputValue << 8) - *aLowpassAccumulator_int32_shift8)) >> aShiftValue;
    *aDoubleLowpassAccumulator_int32_shift8 += ((*aLowpassAccumulator_int32_shift8 - *aDoubleLowpassAccumulator_int32_shift8))
            >> aShiftValue;
}

/*
 * Function to get the value for the int32_shift8 functions / accumulators
 */
int16_t getValue_int32_shift8(int32_t *aAccumulator_int32_shift8) {
    return *aAccumulator_int32_shift8 >> 8;
}

/********************************************************************
 * int32 functions with (16,16) fixed point accumulator
 * This has better resolution but can overflow at input values > 16k
 ********************************************************************/
void doLowpass_int32_shift16(int32_t *aLowpassAccumulator_int32_shift16, int16_t aInputValue, uint8_t aShiftValue) {
    *aLowpassAccumulator_int32_shift16 += ((((int32_t) aInputValue << 16) - *aLowpassAccumulator_int32_shift16)) >> aShiftValue;
}
void doDoubleLowpass_int32_shift16(int32_t *aLowpassAccumulator_int32_shift16, int32_t *aDoubleLowpassAccumulator_int32_shift16,
        int16_t aInputValue, uint8_t aShiftValue) {
    *aLowpassAccumulator_int32_shift16 += ((((int32_t) aInputValue << 16) - *aLowpassAccumulator_int32_shift16)) >> aShiftValue;
    *aDoubleLowpassAccumulator_int32_shift16 += ((*aLowpassAccumulator_int32_shift16 - *aDoubleLowpassAccumulator_int32_shift16))
            >> aShiftValue;
}

/*
 * Function to get the value for the int32_shift16 functions / accumulators
 * Or declare accumulator as LongUnion and call filters with LongUnion.Long and get result with LongUnion.Word.HighWord
 */
int16_t getValue_int32_shift16(int32_t *aAccumulator_int32_shift16) {
    return *aAccumulator_int32_shift16 >> 16;
}

/******************
 * float functions
 ******************/
void doLowpass_float(float *aLowpassAccumulator_float, int16_t aInputValue, float aAlpha) {
    *aLowpassAccumulator_float += (aInputValue - *aLowpassAccumulator_float) * aAlpha;
}
void doDoubleLowpass_float(float *aLowpassAccumulator_float, float *aDoubleLowpassAccumulator_float, int16_t aInputValue,
        float aAlpha) {
    *aLowpassAccumulator_float += (aInputValue - *aLowpassAccumulator_float) * aAlpha;
    *aDoubleLowpassAccumulator_float += (*aLowpassAccumulator_float - *aDoubleLowpassAccumulator_float) * aAlpha;
}

/*******************************************
 * Fast LOWPASS functions with fixed shifts
 *******************************************/
// alpha = 1/2, cutoff frequency 160 Hz @1kHz sampling rate
void doLowpass1_int16(int16_t *aLowpassAccumulator_int16, int16_t aInputValue) {
    *aLowpassAccumulator_int16 += (aInputValue - *aLowpassAccumulator_int16) >> 1;
}
// An unsigned input value requires a cast
void doLowpass1_int16(int16_t *aLowpassAccumulator_int16, uint16_t aInputValue) {
    *aLowpassAccumulator_int16 += ((int16_t) (aInputValue - *aLowpassAccumulator_int16)) >> 1;
}

// alpha = 1/4, cutoff frequency 53 Hz @1kHz
void doLowpass2_int16(int16_t *aLowpassAccumulator_int16, int16_t aInputValue) {
    *aLowpassAccumulator_int16 += ((aInputValue - *aLowpassAccumulator_int16) + (1 << 1)) >> 2;
}
// int32 functions with (24,8) fixed point accumulator
void doLowpass2_int32_shift8(int32_t *aLowpassAccumulator_int32_shift8, int16_t aInputValue) {
    *aLowpassAccumulator_int32_shift8 += ((((int32_t) aInputValue << 8) - *aLowpassAccumulator_int32_shift8)) >> 2;
}

//alpha = 1/8, cutoff frequency 22.7 Hz @1kHz
/*
 * Simple LOWPASS has 6 db per octave
 */
void doLowpass3_int16(int16_t *aLowpassAccumulator_int16, int16_t aInputValue) {
    *aLowpassAccumulator_int16 += ((aInputValue - *aLowpassAccumulator_int16) + (1 << 2)) >> 3;
}
/*
 * Double LOWPASS has 12 db per octave
 */
void doDoubleLowpass3_int16(int16_t *aLowpassAccumulator_int16, int16_t *aDoubleLowpassAccumulator_int16, int16_t aInputValue) {
    *aLowpassAccumulator_int16 += ((aInputValue - *aLowpassAccumulator_int16) + (1 << 2)) >> 3;
    *aDoubleLowpassAccumulator_int16 += ((*aLowpassAccumulator_int16 - *aDoubleLowpassAccumulator_int16) + (1 << 2)) >> 3;
}
/*
 * Triple LOWPASS has 18 db per octave
 */
void doTripleLowpass3_int16(int16_t *aLowpassAccumulator_int16, int16_t *aDoubleLowpassAccumulator_int16,
        int16_t *aTripleLowpassAccumulator_int16, int16_t aInputValue) {
    *aLowpassAccumulator_int16 += ((aInputValue - *aLowpassAccumulator_int16) + (1 << 2)) >> 3;
    *aDoubleLowpassAccumulator_int16 += ((*aLowpassAccumulator_int16 - *aDoubleLowpassAccumulator_int16) + (1 << 2)) >> 3;
    *aTripleLowpassAccumulator_int16 += ((*aDoubleLowpassAccumulator_int16 - *aTripleLowpassAccumulator_int16) + (1 << 2)) >> 3;
}
// int32 functions with (24,8) fixed point accumulator
void doLowpass3_int32_shift8(int32_t *aLowpassAccumulator_int32_shift8, int16_t aInputValue) {
    *aLowpassAccumulator_int32_shift8 += ((((int32_t) aInputValue << 8) - *aLowpassAccumulator_int32_shift8)) >> 3;
}

// alpha = 1/16, cutoff frequency 10.6 Hz @1kHz
void doLowpass4_int16(int16_t *aLowpassAccumulator_int16, int16_t aInputValue) {
    *aLowpassAccumulator_int16 += ((aInputValue - *aLowpassAccumulator_int16) + (1 << 3)) >> 4;
}
void doDoubleLowpass4_int16(int16_t *aLowpassAccumulator_int16, int16_t *aDoubleLowpassAccumulator_int16, int16_t aInputValue) {
    *aLowpassAccumulator_int16 += ((aInputValue - *aLowpassAccumulator_int16) + (1 << 3)) >> 4;
    *aDoubleLowpassAccumulator_int16 += ((*aLowpassAccumulator_int16 - *aDoubleLowpassAccumulator_int16) + (1 << 3)) >> 4;
}

// alpha = 1/32, cutoff frequency 5.13 Hz @1kHz
void doLowpass5_int16(int16_t *aLowpassAccumulator_int16, int16_t aInputValue) {
    *aLowpassAccumulator_int16 += ((aInputValue - *aLowpassAccumulator_int16) + (1 << 4)) >> 5; // maximum is value - ((1 << 4) -1)
}
void doLowpass5_int32_shift8(int32_t *aLowpassAccumulator_int32_shift8, int16_t aInputValue) {
    *aLowpassAccumulator_int32_shift8 += (((int32_t) aInputValue << 8) - *aLowpassAccumulator_int32_shift8) >> 5;
}
void doLowpass5_float(float *aLowpassAccumulator_float, int16_t aInputValue) {
    *aLowpassAccumulator_float += (aInputValue - *aLowpassAccumulator_float) / 32.0; // 24 to 34 us
}

// alpha = 1/256 = 0.0039, cutoff frequency 0.624 Hz @1kHz
void doLowpass8_int32_shift8(int32_t *aLowpassAccumulator_int32_shift8, int16_t aInputValue) {
    *aLowpassAccumulator_int32_shift8 += ((((int32_t) aInputValue << 8) - *aLowpassAccumulator_int32_shift8)) >> 8;
}
void doLowpass8_int32_shift16(int32_t *aLowpassAccumulator_int32_shift16, int16_t aInputValue) {
    *aLowpassAccumulator_int32_shift16 += ((((int32_t) aInputValue << 16) - *aLowpassAccumulator_int32_shift16)) >> 8;
}
void doLowpass8_float(float *aLowpassAccumulator_float, int16_t aInputValue) {
    *aLowpassAccumulator_float += (aInputValue - *aLowpassAccumulator_float) / 256.0; // 24 to 34 us
}

/**********************************
 * Generic BIQUAD filter functions
 **********************************/
#if defined(OPTIMIZE_WITH_INLINE_FUNCTIONS)  // from 13 us down to 8.5 us
__attribute__((always_inline)) inline
#endif
void doBiquad_int16(struct BiquadFilter16Struct *BiquadFilter16Ptr, int16_t aInputValue) {
    BiquadFilter16Ptr->BiQuadHighpass = aInputValue
            - (((int32_t) BiquadFilter16Ptr->BiQuadBandpass * BiquadFilter16Ptr->DampingFactor_shift8) >> 8)
            - BiquadFilter16Ptr->BiQuadLowpass; // HP = Input - (BP * DampingCoefficient) - LP
// rounding with + 128 has significant result!
    BiquadFilter16Ptr->BiQuadBandpass = BiquadFilter16Ptr->BiQuadBandpass
            + ((((int32_t) BiquadFilter16Ptr->BiQuadHighpass * BiquadFilter16Ptr->Alpha_shift8) + 128) >> 8); // BP = BP + (HP * FrequencyCoefficient)
    BiquadFilter16Ptr->BiQuadLowpass = BiquadFilter16Ptr->BiQuadLowpass
            + ((((int32_t) BiquadFilter16Ptr->BiQuadBandpass * BiquadFilter16Ptr->Alpha_shift8) + 128) >> 8); // LP = LP + (BP * FrequencyCoefficient)
}
#if defined(OPTIMIZE_WITH_INLINE_FUNCTIONS)  // from 21 us down to ?? us
__attribute__((always_inline)) inline
#endif
void doBiquad_int32(struct BiquadFilter32Struct *BiquadFilter32Ptr, int16_t aInputValue) {
    BiquadFilter32Ptr->BiQuadHighpass_shift8 = ((int32_t) aInputValue << 8)
            - ((BiquadFilter32Ptr->BiQuadBandpass_shift8 * BiquadFilter32Ptr->DampingFactor_shift8) >> 8)
            - BiquadFilter32Ptr->BiQuadLowpass_shift8; // HP = Input - (BP * DampingCoefficient) - LP
    BiquadFilter32Ptr->BiQuadBandpass_shift8 = BiquadFilter32Ptr->BiQuadBandpass_shift8
            + ((BiquadFilter32Ptr->BiQuadHighpass_shift8 * BiquadFilter32Ptr->Alpha_shift8) >> 8); // BP = BP + (HP * FrequencyCoefficient)
    BiquadFilter32Ptr->BiQuadLowpass_shift8 = BiquadFilter32Ptr->BiQuadLowpass_shift8
            + ((BiquadFilter32Ptr->BiQuadBandpass_shift8 * BiquadFilter32Ptr->Alpha_shift8) >> 8); // LP = LP + (BP * FrequencyCoefficient)
}

/**
 *
 * @param aDampingFactor_shift8     256 is DampingCoefficient 1 (damping) , 128 is 1/2, 512 is 2 etc. 0 -> no damping
 * @param aAlpha_shift8             256 = 1 or shift 0, 128 = 1/2 >>1, 64 = 1/4, 32 = 1/8, 16 = 1/16, 4 = 1/64 >>6, 1 = >>8
 *                                  Values > 128 makes no sense, so we could also use aAlpha_shift9, but this is quite exotic
 */
void initBiquad16(struct BiquadFilter16Struct *BiquadFilter16Ptr, int16_t aDampingFactor_shift8, uint8_t aAlpha_shift8) {
    BiquadFilter16Ptr->DampingFactor_shift8 = aDampingFactor_shift8;
    BiquadFilter16Ptr->Alpha_shift8 = aAlpha_shift8;
}
void initBiquad32(struct BiquadFilter32Struct *BiquadFilter32Ptr, int16_t aDampingFactor_shift8, uint8_t aAlpha_shift8) {
    BiquadFilter32Ptr->DampingFactor_shift8 = aDampingFactor_shift8;
    BiquadFilter32Ptr->Alpha_shift8 = aAlpha_shift8;
}

void resetBiquad16(struct BiquadFilter16Struct *BiquadFilter16Ptr) {
    BiquadFilter16Ptr->BiQuadBandpass = 0;
    BiquadFilter16Ptr->BiQuadLowpass = 0;
}
void resetBiquad32(struct BiquadFilter32Struct *BiquadFilter32Ptr) {
    BiquadFilter32Ptr->BiQuadBandpass_shift8 = 0;
    BiquadFilter32Ptr->BiQuadLowpass_shift8 = 0;
}

/*****************
 * Demo functions
 *****************/

void resetFilters() {
    sLowpass1 = 0;
    sLowpass2 = 0;
    sLowpass3 = 0;
    sLowpass4 = 0;
    sLowpass5 = 0;

    sDoubleLowpass3 = 0;
    sDoubleLowpass4 = 0;
    sDoubleLowpass5 = 0;

    sTripleLowpass3 = 0;

    sLowpass3_int32_shift8 = 0;
    sLowpass5_int32_shift8 = 0;
    sLowpass8_int32_shift8 = 0;

    sLowpass5_float = 0;
    sLowpass8_float = 0;

    resetBiquad16(&sBiQuad_int16);
    resetBiquad32(&sBiQuad_int32);
}

/*
 * Timings for the 16 MHz Arduino-Nano are taken with Salea and @16 MHz sample frequency
 */
void doFiltersTimingTest(int16_t aInputValue) {
#if defined(MEASURE_TIMING)
    noInterrupts();
    // set Biquad filter coefficients
    initBiquad16(&sBiQuad_int16, 256, 32); // damping factor 1 -> damping, 64 ->shift 2
    initBiquad32(&sBiQuad_int32, 64, 32); // damping factor 1/4 -> low damping, 32 ->shift 3

    pinMode(TIMING_OUT_PIN, OUTPUT);

    /*
     * 16 bit functions
     */
    timingPinHigh();
    sLowpass1 += (aInputValue - sLowpass1) >> 1; // 1.06 us, alpha = 0.5, cutoff frequency 160 Hz @1kHz sampling rate
    timingPinLow();timingPinHigh();
    sLowpass1 += (aInputValue - sLowpass1) >> 1; // 1.06 us, alpha = 0.5, cutoff frequency 160 Hz @1kHz sampling rate

    timingPinLow();timingPinHigh();
    sLowpass2 += ((aInputValue - sLowpass2) + (1 << 1)) >> 2;   // 1.31 us, alpha = 0.25, cutoff frequency 53 Hz @1kHz
    timingPinLow();timingPinHigh();
    doLowpass2_int16(&sLowpass2, aInputValue);                  // 1.31 us

    timingPinLow();timingPinHigh();
    sLowpass3 += ((aInputValue - sLowpass3) + (1 << 2)) >> 3;   // 2.00 us, alpha = 0.125, cutoff frequency 22.7 Hz @1kHz
    timingPinLow();timingPinHigh();
    doLowpass3_int16(&sLowpass3, aInputValue);                  // 2.00 us

    timingPinLow();timingPinHigh();
    sLowpass4 += ((aInputValue - sLowpass4) + (1 << 3)) >> 4;   // 2.31 us, alpha = 0.0625, cutoff frequency 10.6 Hz @1kHz
    timingPinLow();timingPinHigh();
    doLowpass4_int16(&sLowpass4, aInputValue);                  // 2.31 us,

    timingPinLow();timingPinHigh();
    sLowpass5 += ((aInputValue - sLowpass5) + (1 << 4)) >> 5;   // 2.63 us, alpha = 1/32 0.03125, cutoff frequency 5.13 Hz @1kHz
    timingPinLow();timingPinHigh();
    doLowpass5_int16(&sLowpass5, aInputValue);                  // 2.63 us

    timingPinLow();timingPinHigh();
    sLowpass6 += ((aInputValue - sLowpass5) + (1 << 5)) >> 6;   //  us

    /*
     * 16 bit generic functions
     */
    timingPinLow();
    delayMicroseconds(5);
    timingPinHigh();
    doLowpassShift_int16(&sLowpass2, aInputValue, 2);   // 1.31 us
    timingPinLow();timingPinHigh();
    doLowpass_int16(&sLowpass2, aInputValue, 64);       // 2.06 us  - 64 = 1/4 = >>2
    timingPinLow();timingPinHigh();
    doLowpassShift_int16(&sLowpass5, aInputValue, 4);   // 2.31 us
    timingPinLow();timingPinHigh();
    doLowpass_int16(&sLowpass5, aInputValue, 16);       //  us  - 16 = 1/16 = >>4
    timingPinLow();timingPinHigh();
    doLowpassShift_int16(&sLowpass5, aInputValue, 6);   //  us
    timingPinLow();timingPinHigh();
    doLowpass_int16(&sLowpass5, aInputValue, 4);        // us  - 4 = 1/64 = >>6

    /*
     * 32 bit functions / fixed point
     */
    timingPinLow();
    delayMicroseconds(5);
    int32_t tInputValue32_shift8 = (int32_t) aInputValue << 8;
    timingPinHigh();
    sLowpass2_int32_shift8 += (tInputValue32_shift8 - sLowpass2_int32_shift8) >> 2; // 2.63 us
    timingPinLow();timingPinHigh();
    doLowpass2_int32_shift8(&sLowpass2_int32_shift8, aInputValue);                  //  2.63us
    timingPinLow();timingPinHigh();
    sLowpass3_int32_shift8 += (tInputValue32_shift8 - sLowpass3_int32_shift8) >> 3; // 3.06 us
    timingPinLow();timingPinHigh();
    doLowpass3_int32_shift8(&sLowpass3_int32_shift8, aInputValue);                  // 3.06 us

    timingPinLow();timingPinHigh();
    sLowpass5_int32_shift8 += (tInputValue32_shift8 - sLowpass5_int32_shift8) >> 5; // 3.94 us
    timingPinLow();timingPinHigh();
    doLowpass5_int32_shift8(&sLowpass5_int32_shift8, aInputValue);                  // 3.94 us
    timingPinLow();timingPinHigh();
    sLowpass5_int32_shift8 += (tInputValue32_shift8 - sLowpass5_int32_shift8) >> 7; // 4.81 us
    timingPinLow();timingPinHigh();
    sLowpass8_int32_shift8 += (tInputValue32_shift8 - sLowpass8_int32_shift8) >> 8; // 2.13 us

    /*
     * 32 bit generic functions
     */
    timingPinLow();
    delayMicroseconds(5);
    timingPinHigh();
    doLowpassShift_int32_shift8(&sLowpass2_int32_shift8, tInputValue32_shift8, 2);  // 3.18 us
    timingPinLow();timingPinHigh();
    doLowpass_int32_shift8(&sLowpass2_int32_shift8, tInputValue32_shift8, 64);      // 3.62 to 7.13 us depending on compiler optimization (inline or O3) - 64 = 1/4 = >>2
    timingPinLow();timingPinHigh();
    doLowpassShift_int32_shift8(&sLowpass2_int32_shift8, tInputValue32_shift8, 3);  // 3.06 us
    timingPinLow();timingPinHigh();
    doLowpass_int32_shift8(&sLowpass2_int32_shift8, tInputValue32_shift8, 32);      // 7.13 us  - 32 = 1/8 = >>3
    timingPinLow();timingPinHigh();
    doLowpassShift_int32_shift8(&sLowpass5_int32_shift8, tInputValue32_shift8, 5);  // 3.94 us
    timingPinLow();timingPinHigh();
    doLowpass_int32_shift8(&sLowpass8_int32_shift8, tInputValue32_shift8, 8);       // 2.88 us to 7.13 us depending on compiler optimization (inline or O3) - 8 = 1/32 = >>5
    timingPinLow();timingPinHigh();
    doLowpass_int32_shift8(&sLowpass2_int32_shift8, tInputValue32_shift8, 64);      // 7.13 us  - 64 = 1/4 = >>2

    /*
     * Float functions
     */
    timingPinLow();
    delayMicroseconds(5);
    timingPinHigh();
    doLowpass5_float(&sLowpass5_float, aInputValue);    // 23.00 us | 28.56 us for 2. pass (negative input value?)
    timingPinLow();timingPinHigh();
    doLowpass8_float(&sLowpass8_float, aInputValue);    // 20,00 us | 25.19 us for 2. pass (negative input value?)

    /*
     * Biquad functions
     */
    timingPinLow();
    delayMicroseconds(5);
// set Biquad filter coefficients
    initBiquad16(&sBiQuad_int16, 256, 32);
    initBiquad32(&sBiQuad_int32, 64, 32);
    timingPinHigh();
    doBiquad_int16(&sBiQuad_int16, aInputValue);    // 12.94 us - damping factor 1 -> damping, 64 ->shift 2
    timingPinLow();timingPinHigh();
    doBiquad_int32(&sBiQuad_int32, aInputValue);    // 21.81 us - damping factor 1/4 -> low damping, 32 ->shift 3

    timingPinLow();
    interrupts();
#endif
}

/*
 * The main demo function
 */
void doFiltersStep(int16_t aInputValue) {
    // set Biquad filter coefficients
    initBiquad16(&sBiQuad_int16, 256, 32); // damping factor 1 -> damping, 64 ->shift 2
    initBiquad32(&sBiQuad_int32, 64, 32); // damping factor 1/4 -> low damping, 32 ->shift 3

    sInputValueForPrint = aInputValue;

    /*
     * int16_t low pass values
     */
    sLowpass1 += (aInputValue - sLowpass1) >> 1; // 1 us, alpha = 0.5, cutoff frequency 160 Hz @1kHz sampling rate

// "+ (1 << 2)" to avoid to much rounding errors and it costs only 1 clock extra
//    sLowpass2 += ((aInputValue - sLowpass2) + (1 << 1)) >> 2; // 1.1 us, alpha = 0.25, cutoff frequency 53 Hz @1kHz
//    doLowpass2_int16(&sLowpass2, aInputValue);
//    replaced by generic function:
    doLowpass_int16(&sLowpass2, aInputValue, 64); //64 = 1/4 = >>2

//    sLowpass3 += ((aInputValue - sLowpass3) + (1 << 2)) >> 3; // 1.8 us, alpha = 0.125, cutoff frequency 22.7 Hz @1kHz
//    sDoubleLowpass3 += ((sLowpass3 - sDoubleLowpass3) + (1 << 2)) >> 3;
// replaced by:
//    doLowpass3_int16(&sLowpass3, aInputValue);
//    doLowpass3_int16(&sDoubleLowpass3, sLowpass3);
// replaced by fixed function:
    doDoubleLowpass3_int16(&sLowpass3, &sDoubleLowpass3, aInputValue);

//    sTripleLowpass3 += ((sDoubleLowpass3 - sTripleLowpass3) + (1 << 2)) >> 3;
    doLowpass3_int16(&sTripleLowpass3, sDoubleLowpass3);

//    sLowpass4 += ((aInputValue - sLowpass4) + (1 << 3)) >> 4; // 2.2 us, alpha = 0.0625, cutoff frequency 10.6 Hz @1kHz
    doLowpass4_int16(&sLowpass4, aInputValue);
//    sDoubleLowpass4 += ((sLowpass4 - sDoubleLowpass4) + (1 << 3)) >> 4;
    doLowpass4_int16(&sDoubleLowpass4, sLowpass4);

//    sLowpass5 += ((aInputValue - sLowpass5) + (1 << 4)) >> 5; // 2.5 us, alpha = 1/32 0.03125, cutoff frequency 5.13 Hz @1kHz
    doLowpass5_int16(&sLowpass5, aInputValue);
//    sDoubleLowpass5 += ((sLowpass5 - sDoubleLowpass5) + (1 << 4)) >> 5;
    doLowpass5_int16(&sDoubleLowpass5, sLowpass5);

    /*
     * int32_t low pass values for higher exponents
     */
//    int32_t tInputValue32_shift8 = (int32_t) aInputValue << 8;
//    sLowpass3_int32_shift8 += (tInputValue32_shift8 - sLowpass3_int32_shift8) >> 3; // Fixed point
//    doLowpass3_int32_shift8(&sLowpass3_int32_shift8, aInputValue);
//    replaced by generic function:
    doLowpassShift_int32_shift8(&sLowpass3_int32_shift8, aInputValue, 3);

    /*
     * int32_t low pass values for higher resolution
     */
//    sLowpass5_int32_shift8 += (tInputValue32_shift8 - sLowpass5_int32_shift8) >> 5; // Fixed point 4.2 us
//    replaced by generic function:
    doLowpass_int32_shift8(&sLowpass5_int32_shift8, aInputValue, 8);

// alpha = 1/256 = 0.0039, cutoff frequency 0.624 Hz @1kHz
//    sLowpass8_int32_shift8 += ((tInputValue32_shift8 - sLowpass8_int32_shift8) + (1 << 7)) >> 8; // Fixed point 2.0 us because of fast shift :-)
    doLowpass8_int32_shift8(&sLowpass8_int32_shift8, aInputValue);

    /*
     * float low pass values with exponent 5 and 8
     */
//    sLowpass5_float += (aInputValue - sLowpass5_float) / 32.0; // 24 to 34 us
    doLowpass5_float(&sLowpass5_float, aInputValue);

//    sLowpass8_float += (aInputValue - sLowpass8_float) / 256.0;
    doLowpass8_float(&sLowpass8_float, aInputValue);

    /*
     * Biquad or State Variable Filter
     */
    doBiquad_int16(&sBiQuad_int16, aInputValue);
    doBiquad_int32(&sBiQuad_int32, aInputValue);
}

/************************
 * Demo print functions
 ************************/
/*
 * Use "P" instead of "pass", since we have problems with the length of caption
 */
void printFiltersCaption(uint8_t aFilterSelection) {
    uint32_t aPrintMask = FilterSelectionArray[aFilterSelection];
    Serial.print(F("Input "));

    if (aPrintMask & PRINT_LP_1) {
        Serial.print(F("LowP1_16 "));
    }
    if (aPrintMask & PRINT_LP_2) {
        Serial.print(F("LowP2_16 "));
    }
    if (aPrintMask & PRINT_LP_3) {
        Serial.print(F("LowP3_16 "));
    }
    if (aPrintMask & PRINT_LP_3_32) {
        Serial.print(F("LowP3_32 "));
    }
    if (aPrintMask & PRINT_DOUBLE_LP_3) {
        Serial.print(F("DoubleLowP3_16 "));
    }
    if (aPrintMask & PRINT_TRIPLE_LP_3) {
        Serial.print(F("TripleLowP3_16 "));
    }
    if (aPrintMask & PRINT_LP_4) {
        Serial.print(F("LowP4_16 "));
    }
    if (aPrintMask & PRINT_DOUBLE_LP_4) {
        Serial.print(F("DoubleLowP4_16 "));
    }
    if (aPrintMask & PRINT_LP_5) {
        Serial.print(F("LowP5_16 "));
    }
    if (aPrintMask & PRINT_LP_5_32) {
        Serial.print(F("LowP5_32 "));
    }
    if (aPrintMask & PRINT_LP_5_FLOAT) {
        Serial.print(F("LowP5_float "));
    }
    if (aPrintMask & PRINT_LP_8_32) {
        Serial.print(F("LowP8_32 "));
    }
    if (aPrintMask & PRINT_LP_8_FLOAT) {
        Serial.print(F("LowP8_float "));
    }
    if (aPrintMask & PRINT_HIGH_PASS_1_16) {
        Serial.print(F("HighP1_16 "));
    }
    if (aPrintMask & PRINT_HIGH_PASS_3_16) {
        Serial.print(F("HighP3_16 "));
    }

    if (aPrintMask & PRINT_BAND_PASS_1_3) {
        Serial.print(F("BandP1_3_16 "));
    }
    if (aPrintMask & PRINT_BAND_PASS_1_5) {
        Serial.print(F("BandP1_5_16 "));
    }
    if (aPrintMask & PRINT_BAND_PASS_3_4) {
        Serial.print(F("BandP3_4_16 "));
    }
    if (aPrintMask & PRINT_BAND_PASS_3_5) {
        Serial.print(F("BandP3_5_16 "));
    }

    if (aPrintMask & PRINT_BAND_STOP_3_4) {
        Serial.print(F("BandStop3_4_16 "));
    }

    if (aPrintMask & PRINT_BQ_LP_16) {
        Serial.print(F("BiQuadLowP2_16 "));
    }
    if (aPrintMask & PRINT_BQ_HP_16) {
        Serial.print(F("BiQuadHighP2_16 "));
    }
    if (aPrintMask & PRINT_BQ_BP_16) {
        Serial.print(F("BiQuadBandP2_16 "));
    }

    if (aPrintMask & PRINT_BQ_LP_32) {
        Serial.print(F("BiQuadLowP3_32 "));
    }
    if (aPrintMask & PRINT_BQ_HP_32) {
        Serial.print(F("BiQuadHighP3_32 "));
    }
    if (aPrintMask & PRINT_BQ_BP_32) {
        Serial.print(F("BiQuadBandP3_32 "));
    }

    Serial.print(F("__"));
    Serial.print(FilterSelectionStringArray[aFilterSelection]);
    Serial.println(F("__"));
}

void printFiltersResults(uint32_t aPrintMask) {
    Serial.print(sInputValueForPrint);
    Serial.print(" ");

    if (aPrintMask & PRINT_LP_1) {
        Serial.print(sLowpass1);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_LP_2) {
        Serial.print(sLowpass2);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_LP_3) {
        Serial.print(sLowpass3);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_LP_3_32) {
        Serial.print(sLowpass3_int32_shift8 >> 8);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_DOUBLE_LP_3) {
        Serial.print(sDoubleLowpass3);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_TRIPLE_LP_3) {
        Serial.print(sTripleLowpass3);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_LP_4) {
        Serial.print(sLowpass4);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_DOUBLE_LP_4) {
        Serial.print(sDoubleLowpass4);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_LP_5) {
        Serial.print(sLowpass5);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_LP_5_32) {
        Serial.print(sLowpass5_int32_shift8 >> 8);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_LP_5_FLOAT) {
        Serial.print(sLowpass5_float);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_LP_8_32) {
        Serial.print(sLowpass8_int32_shift8 >> 8);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_LP_8_FLOAT) {
        Serial.print(sLowpass8_float);
        Serial.print(" ");
    }

    if (aPrintMask & PRINT_HIGH_PASS_1_16) {
        Serial.print(sInputValueForPrint - sLowpass1);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_HIGH_PASS_3_16) {
        Serial.print(sInputValueForPrint - sLowpass3);
        Serial.print(" ");
    }

    if (aPrintMask & PRINT_BAND_PASS_1_3) {
        Serial.print(sLowpass1 - sLowpass3);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_BAND_PASS_1_5) {
        Serial.print(sLowpass1 - sLowpass5);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_BAND_PASS_3_4) {
        Serial.print(sLowpass3 - sLowpass4);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_BAND_PASS_3_5) {
        Serial.print(sLowpass3 - sLowpass5);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_BAND_STOP_3_4) {
        Serial.print(sInputValueForPrint - (sLowpass3 - sLowpass4));
        Serial.print(" ");
    }

    if (aPrintMask & PRINT_BQ_LP_16) {
        Serial.print(sBiQuad_int16.BiQuadLowpass);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_BQ_HP_16) {
        Serial.print(sBiQuad_int16.BiQuadHighpass);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_BQ_BP_16) {
        Serial.print(sBiQuad_int16.BiQuadBandpass);
        Serial.print(" ");
    }

    if (aPrintMask & PRINT_BQ_LP_32) {
        Serial.print(sBiQuad_int32.BiQuadLowpass_shift8 >> 8);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_BQ_HP_32) {
        Serial.print(sBiQuad_int32.BiQuadHighpass_shift8 >> 8);
        Serial.print(" ");
    }
    if (aPrintMask & PRINT_BQ_BP_32) {
        Serial.print(sBiQuad_int32.BiQuadBandpass_shift8 >> 8);
        Serial.print(" ");
    }

    Serial.println();
}

#endif // _SIMPLE_EMA_FILTERS_HPP
