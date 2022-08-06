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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#ifndef _SIMPLE_EMA_FILTERS_H
#define _SIMPLE_EMA_FILTERS_H

#include <stdint.h>

#define VERSION_SIMPLE_EMA_FILTERS "1.0.0"
#define VERSION_SIMPLE_EMA_FILTERS_MAJOR 1
#define VERSION_SIMPLE_EMA_FILTERS_MINOR 0
// The change log is at the bottom of the file

// Definitions for aPrintMask
#define PRINT_INPUT         0x01
#define PRINT_EMA_1         0x02
#define PRINT_EMA_2         0x04
#define PRINT_EMA_3         0x08
#define PRINT_EMA_4         0x10
#define PRINT_EMA_5         0x20
#define PRINT_EMA_5_FLOAT   0x40
#define PRINT_EMA_8_FLOAT   0x80

#define PRINT_EMA_3_32      0x1000
#define PRINT_EMA_5_32      0x2000
#define PRINT_EMA_8_32      0x4000

#define PRINT_HIGH_PASS     0x100
#define PRINT_BAND_PASS_1_3 0x200
#define PRINT_BAND_PASS_3_4 0x400

#define PRINT_ALL_FILTERS           0x77FF
#define PRINT_ALL_SIMPLE_FILTERS    0x073F
#define PRINT_LOW_PASS_HI_RES       0x7000
#define PRINT_LOW_PASS_1_TO_8       0x403F // all 16 bit except 8 which is 32 bit
#define PRINT_LOW_PASS_1_3_5_8      (PRINT_INPUT | PRINT_EMA_1 | PRINT_EMA_3_32 | PRINT_EMA_5_32 | PRINT_EMA_8_32)
#define PRINT_LOW_PASS_5            (PRINT_INPUT | PRINT_EMA_5 | PRINT_EMA_5_32 | PRINT_EMA_FLOAT)
#define PRINT_LOW_PASS_16_32        (PRINT_INPUT | PRINT_EMA_3 | PRINT_EMA_3_32 | PRINT_EMA_5 | PRINT_EMA_5_32 | PRINT_EMA_5_FLOAT | PRINT_EMA_8_32| PRINT_EMA_8_FLOAT)

extern int16_t sInputValueForPrint;

extern int16_t sLowpass1;
extern int16_t sLowpass2;
extern int16_t sLowpass3;
extern int16_t sLowpass4;
extern int16_t sLowpass5;

extern int32_t sLowpass3_int32;
extern int32_t sLowpass5_int32;
extern int32_t sLowpass8_int32;

extern float sLowpass5_float;

void printFiltersCaption(uint16_t aPrintMask = PRINT_ALL_FILTERS);
void printFiltersResults(uint16_t aPrintMask = PRINT_ALL_FILTERS);
void doFiltersStep(int16_t aInputValue);

#endif // _SIMPLE_EMA_FILTERS_H
