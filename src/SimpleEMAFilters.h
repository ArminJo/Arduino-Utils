/*
 * SimpleEMAFilters.h
 *
 *  Copyright (C) 2020-2021  Armin Joachimsmeyer
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

#define VERSION_SIMPLE_EMA_FILTERS "1.1.0"
#define VERSION_SIMPLE_EMA_FILTERS_MAJOR 1
#define VERSION_SIMPLE_EMA_FILTERS_MINOR 1
#define VERSION_SIMPLE_EMA_FILTERS_PATCH 0
// The change log is at the bottom of the file

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
#define PRINT_REJECT_3_4    0x4000

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

// Sets of filters used below
#define PRINT_HIGHER_ORDER_LOW_PASS     (PRINT_DOUBLE_LP_3 | PRINT_DOUBLE_LP_4 | PRINT_TRIPLE_LP_3)
#define PRINT_LOW_PASS_1_TO_8       (PRINT_LP_1_TO_5 | PRINT_LP_8_32) // all 16 bit except 8 which is 32 bit
#define PRINT_LOW_PASS_1_3_5_8      (PRINT_LP_1 | PRINT_LP_3_32 | PRINT_LP_5_32 | PRINT_LP_8_32)
#define PRINT_LOW_PASS_16_32        (PRINT_LP_3 | PRINT_LP_3_32 | PRINT_LP_5 | PRINT_LP_5_32 | PRINT_LP_5_FLOAT | PRINT_LP_8_32| PRINT_LP_8_FLOAT) // To compare different resolutions

#define PRINT_ALL_LOW_PASS          (PRINT_LOW_PASS_1_TO_8 | PRINT_LP_3_32 | PRINT_LP_5_32 | PRINT_HIGHER_ORDER_LOW_PASS | PRINT_BQ_LP_32)
#define PRINT_LOW_HIGH_PASS         (PRINT_LP_1 | PRINT_HIGH_PASS_1_16 | PRINT_LP_3 | PRINT_HIGH_PASS_3_16) // For comparison of low and high pass
#define PRINT_BAND_AND_REJECT_PASS  (PRINT_LP_3 | PRINT_BAND_PASS_1_3 |PRINT_BAND_PASS_1_5 | PRINT_BAND_PASS_3_4 | PRINT_BAND_PASS_3_5 | PRINT_REJECT_3_4)
#define PRINT_SIGNIFICANT_FILTERS   (PRINT_LOW_PASS_1_3_5_8 | PRINT_HIGHER_ORDER_LOW_PASS | PRINT_HIGH_PASS_3_16 | PRINT_BAND_PASS_3_4 | PRINT_REJECT_3_4 | PRINT_BQ_LP_32) // To show what is possible with EMA filters

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

extern int32_t sLowpass3_int32;
extern int32_t sLowpass5_int32;
extern int32_t sLowpass8_int32;

extern float sLowpass5_float;
extern float sLowpass8_float;

extern int16_t sBiQuadHighpass_int16;
extern int16_t sBiQuadBandpass_int16;
extern int16_t sBiQuadLowpass_int16;

extern int32_t sBiQuadHighpass_int32_shift8;
extern int32_t sBiQuadBandpass_int32_shift8;
extern int32_t sBiQuadLowpass_int32_shift8;

void printFiltersCaption(uint8_t aFilterSelection);
void printFiltersResults(uint32_t aPrintMask = PRINT_ALL_FILTERS);
void resetFilters();
void doFiltersStep(int16_t aInputValue);

#endif // _SIMPLE_EMA_FILTERS_H
