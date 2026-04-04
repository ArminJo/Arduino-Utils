/*
 * SimpleEMAFilters.h
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

#ifndef _SIMPLE_EMA_FILTERS_H
#define _SIMPLE_EMA_FILTERS_H

#include <stdint.h>

void doLowpass_int16(int16_t *aLowpassAccumulator_int16, int16_t aInputValue, uint8_t aAlpha_shift8);
void doLowpass_int32_shift8(int32_t *aLowpassAccumulator_int32_shift8, int16_t aInputValue, uint8_t aAlpha_shift8);

struct BiquadFilter16Struct {
    int16_t BiQuadLowpass = 0;
    int16_t BiQuadBandpass = 0;
    int16_t BiQuadHighpass;         // Is always computed new from input and old BiQuadLowpass and BiQuadBandpass values
    int16_t DampingFactor_shift8;   // 256 is DampingCoefficient 1 (damping) , 128 is 1/2, 512 is 2 etc. 0 -> no damping
    uint8_t Alpha_shift8;           // 256 = 1 or shift 0, 128 = 1/2 >>1, 64 = 1/4, 32 = 1/8, 16 = 1/16, 4 = 1/64 >>6, 1 = >>8
};

struct BiquadFilter32Struct {
    int32_t BiQuadLowpass_shift8 = 0;
    int32_t BiQuadBandpass_shift8 = 0;
    int32_t BiQuadHighpass_shift8;  // Is always computed new from input and old BiQuadLowpass and BiQuadBandpass values
    int16_t DampingFactor_shift8;   // 256 is DampingCoefficient 1 (damping) , 128 is 1/2, 512 is 2 etc. 0 -> no damping
    uint8_t Alpha_shift8;           // 256 = 1 or shift 0, 128 = 1/2 >>1, 64 = 1/4, 32 = 1/8, 16 = 1/16, 4 = 1/64 >>6, 1 = >>8
};

void doBiquad_int16(struct BiquadFilter16Struct *BiquadFilter16Ptr, int16_t aInputValue);
void doBiquad_int32(struct BiquadFilter32Struct *BiquadFilter32Ptr, int16_t aInputValue);
void initBiquad16(struct BiquadFilter16Struct *BiquadFilter16Ptr, int16_t aDampingFactor_shift8, uint8_t aAlpha_shift8);
void initBiquad32(struct BiquadFilter32Struct *BiquadFilter32Ptr, int16_t aDampingFactor_shift8, uint8_t aAlpha_shift8);
void resetBiquad16(struct BiquadFilter16Struct *BiquadFilter16Ptr);
void resetBiquad32(struct BiquadFilter32Struct *BiquadFilter32Ptr);

#define VERSION_SIMPLE_EMA_FILTERS "2.0.0"
#define VERSION_SIMPLE_EMA_FILTERS_MAJOR 2
#define VERSION_SIMPLE_EMA_FILTERS_MINOR 0
#define VERSION_SIMPLE_EMA_FILTERS_PATCH 0
// The change log is at the bottom of the file

/*
 * Definitions for demo example
 */
// Definitions for aPrintMask
#define PRINT_LP_1          0x01
#define PRINT_LP_2          0x02
#define PRINT_LP_3          0x04
#define PRINT_LP_4          0x08
#define PRINT_LP_5          0x10
#define PRINT_LP_1_TO_5     0x1F

#define PRINT_LP_5_FLOAT    0x20
#define PRINT_LP_8_FLOAT    0x40

#define PRINT_LP_3_32       0x100
#define PRINT_LP_5_32       0x200
#define PRINT_LP_8_32       0x400

#define PRINT_HIGH_PASS_1_16   0x1000
#define PRINT_HIGH_PASS_3_16   0x2000
#define PRINT_BAND_STOP_3_4    0x4000

#define PRINT_BAND_PASS_1_3  0x10000
#define PRINT_BAND_PASS_1_5  0x30000
#define PRINT_BAND_PASS_3_4  0x40000
#define PRINT_BAND_PASS_3_5  0x80000

#define PRINT_DOUBLE_LP_3   0x100000
#define PRINT_DOUBLE_LP_4   0x200000
#define PRINT_TRIPLE_LP_3   0x400000

#define PRINT_BQ_LP_16      0x1000000
#define PRINT_BQ_HP_16      0x2000000
#define PRINT_BQ_BP_16      0x4000000

#define PRINT_BQ_LP_32      0x10000000
#define PRINT_BQ_HP_32      0x20000000
#define PRINT_BQ_BP_32      0x40000000

// Sets of filters used in FilterSelectionArray
#define PRINT_HIGHER_ORDER_LOW_PASS     (PRINT_DOUBLE_LP_3 | PRINT_DOUBLE_LP_4 | PRINT_TRIPLE_LP_3)
#define PRINT_LOW_PASS_1_TO_8       (PRINT_LP_1_TO_5 | PRINT_LP_8_32) // all 16 bit except 8 which is 32 bit
#define PRINT_LOW_PASS_1_3_5_8      (PRINT_LP_1 | PRINT_LP_3_32 | PRINT_LP_5_32 | PRINT_LP_8_32)
#define PRINT_LOW_PASS_16_32        (PRINT_LP_3 | PRINT_LP_3_32 | PRINT_LP_5 | PRINT_LP_5_32 | PRINT_LP_5_FLOAT | PRINT_LP_8_32| PRINT_LP_8_FLOAT) // To compare different resolutions

#define PRINT_ALL_LOW_PASS          (PRINT_LOW_PASS_1_TO_8 | PRINT_LP_3_32 | PRINT_LP_5_32 | PRINT_HIGHER_ORDER_LOW_PASS | PRINT_BQ_LP_16)
#define PRINT_LOW_HIGH_PASS         (PRINT_LP_1 | PRINT_HIGH_PASS_1_16 | PRINT_LP_3 | PRINT_HIGH_PASS_3_16) // For comparison of low and high pass
#define PRINT_BAND_PASS_AND_BAND_STOP (PRINT_LP_3 | PRINT_BAND_PASS_1_3 |PRINT_BAND_PASS_1_5 | PRINT_BAND_PASS_3_4 | PRINT_BAND_PASS_3_5 | PRINT_BAND_STOP_3_4)
#define PRINT_SIGNIFICANT_FILTERS   (PRINT_LOW_PASS_1_3_5_8 | PRINT_HIGHER_ORDER_LOW_PASS | PRINT_HIGH_PASS_3_16 | PRINT_BAND_PASS_3_4 | PRINT_BAND_STOP_3_4 | PRINT_BQ_LP_16) // To show what is possible with EMA filters

#define PRINT_BI_QUAD_16            (PRINT_BQ_LP_16 | PRINT_BQ_HP_16 | PRINT_BQ_BP_16)
#define PRINT_BI_QUAD_32            (PRINT_BQ_LP_32 | PRINT_BQ_HP_32 | PRINT_BQ_BP_32)
#define PRINT_ALL_BI_QUAD           (PRINT_BI_QUAD_16 | PRINT_BI_QUAD_32)

#define PRINT_ALL_FILTERS           0xFFFFFFFF

extern int16_t sInputValueForPrint;

extern int16_t sLowpass1;
extern int16_t sLowpass2;
extern int16_t sLowpass3;
extern int16_t sLowpass4;
extern int16_t sLowpass5;

extern int16_t sDoubleLowpass3;
extern int16_t sDoubleLowpass4;
extern int16_t sDoubleLowpass5;

extern int16_t sTripleLowpass3;

extern int32_t sLowpass3_int32_shift8;
extern int32_t sLowpass5_int32_shift8;
extern int32_t sLowpass8_int32_shift8;

extern float sLowpass5_float;
extern float sLowpass8_float;

extern struct BiquadFilter16Struct sBiQuad_int16;
extern struct BiquadFilter32Struct sBiQuad_int32;

void resetFilters();
void doFiltersTimingTest(int16_t aInputValue);
void doFiltersStep(int16_t aInputValue);
void printFiltersCaption(uint8_t aFilterSelection);
void printFiltersResults(uint32_t aPrintMask = PRINT_ALL_FILTERS);

#endif // _SIMPLE_EMA_FILTERS_H
