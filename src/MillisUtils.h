/*
 * MillisUtils.h
 *
 *  Copyright (C) 2016-2020  Armin Joachimsmeyer
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
 *  along with this program. If not, see <https://www.gnu.org/licenses/gpl-3.0.html>.
 *
 */

#ifndef _MILLIS_UTILS_H
#define _MILLIS_UTILS_H

#include <Arduino.h>

#include <stdint.h>
#if defined(__AVR__)

/*
 * storage for millis value to enable compensation for interrupt disable at signal acquisition etc.
 */
#if !defined(timer0_millis) && (defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)  || defined(__AVR_ATtiny87__) || defined(__AVR_ATtiny167__))
#define timer0_millis millis_timer_millis // The ATTinyCore libraries use this variable name in wiring.c
#endif

/*
 * Register and bit of the timer overflow interrupt used for millis().
 * Own names, to avoid redefining TIMSK / TOIE for all following includes.
 */
#define MILLIS_TOIE     TOIE0   // Assume timer 0 for millis()

#if defined(TIMSK0)
#define MILLIS_TIMSK    TIMSK0  // e.g. ATmega328, ATtiny167
#elif defined(TIMSK)
#define MILLIS_TIMSK    TIMSK   // e.g. ATtiny85, ATmega8
#endif

extern volatile unsigned long timer0_millis;

void delayAndCallFunctionEveryMillis(unsigned int aDelayMillis, void (*aDelayCallback)(void));

#if defined(MILLIS_TIMSK)
void disableMillisInterrupt();
void enableMillisInterrupt(uint16_t aMillisToAddForCompensation = 0);
#endif
void addToMillis(uint16_t aMillisToAdd);

void speedTestWith1kCalls(Print *aSerial, void (*aFunctionUnderTest)(void));
#endif //  defined(__AVR__)

#if ! defined(TEENSYDUINO)
void delayMilliseconds(unsigned int aMillis);
bool areMillisGone(unsigned int aMillis);
bool areMillisGone(unsigned int aMillis, unsigned long * aLastMillisPtr);
#endif // ! defined(TEENSYDUINO)

#endif // _MILLIS_UTILS_H
