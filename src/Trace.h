/*
 *  Trace.h
 *
 *  Attach button at pin 2 and press it if you want to see actual program counter.
 *  OR / AND
 *  Connect pin 11 to pin 2 and use startTracingSignal() and stopTracingSignal().
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
#ifndef TRACE_H_
#define TRACE_H_

#include <Arduino.h>
#include "digitalWriteFast.h"

/*
 * Comment this out, if you checked the NUMBER_OF_PUSH above.
 * Otherwise this number is determined at enableINT0InterruptOnFallingEdge() and stored in a global variable,
 * which can accidently be can be overwritten, by a program out of control.
 */
//#define USE_FIXED_NUMBER_OF_PUSH
#if defined(USE_FIXED_NUMBER_OF_PUSH)
/*
 * The amount of pushes for this ISR is compiler and compiler flag dependent
 * I saw 15, 17, 19 and 20 pushes. They can be found by looking for <__vector_1> in the lss file.
 */
#define NUMBER_OF_PUSH 15
//#define NUMBER_OF_PUSH 17
//#define NUMBER_OF_PUSH 19
#else
extern uint8_t sPushAdjust;
/*
 * Minimal number of pushs as start for detection curren value
 * I saw 15, 17, 19 and 20 pushes. They can be found by looking for <__vector_1> in the lss file.
 */
#define NUMBER_OF_PUSH_MIN 15 //
#define INIT_VALUE_OF_PUSH_ADJUST 71
#endif

extern char __init;
extern char _etext;

void enableINT0InterruptOnFallingEdge();
void printTextSectionAddresses();

void startTracingSignal();
void stopTracingSignal();

void sendUSART(char aChar);
void sendUnsignedByteHex(uint8_t aByte);
void sendUnsignedIntegerHex(uint16_t aInteger);
void sendLineFeed();
void sendPCHex(uint16_t aPC);
void sendHex(uint16_t aInteger, char aName);
void sendHexNoInterrupts(uint16_t aInteger, char aName);
char nibbleToHex(uint8_t aByte);

#endif // TRACE_H_

#pragma once

