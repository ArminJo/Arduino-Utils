/*
 *  MillisUtils.cpp
 *
 *  Unifies millis() timer handling for Digispark, AttinyCore and Arduino cores.
 *  - Start, stop and modify milliseconds timer and value.
 *  - Functions to compensate millis() timer value after long lasting ISR etc..
 *  - Blocking delayMilliseconds() function for use in noInterrupts context like ISR.
 *
 *  Copyright (C) 2016-2026  Armin Joachimsmeyer
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

#include <Arduino.h>
#include "MillisUtils.h"

#if defined(__AVR__)
#include <util/atomic.h> // for ATOMIC_BLOCK

#if !defined(cbi)
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#if !defined(sbi)
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void delayAndCallFunctionEveryMillis(unsigned int aDelayMillis, void (*aDelayCallback)(void)) {
    uint32_t tStartMillis = millis();
    do {
        if (aDelayCallback != nullptr) {
            aDelayCallback();
        }
        delay(1);
    } while (millis() - tStartMillis < aDelayMillis);
}

/*
 *
 */
void addToMillis(uint16_t aMillisToAdd) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        timer0_millis += aMillisToAdd;
    }
}

#if defined(MILLIS_TIMSK)
/*
 * disable Timer0 (millis()) overflow interrupt
 * since the loop last exactly a multiple of 1024 micros, add a few statements between disabling and enabling
 */
void disableMillisInterrupt() {
    cbi(MILLIS_TIMSK, MILLIS_TOIE);
}

/*
 * Enable timer 0 overflow interrupt and compensate for disabled timer, if still disabled.
 * Modifying timer0_millis is safe here, since the millis ISR is still disabled.
 */
void enableMillisInterrupt(uint16_t aMillisToAddForCompensation) {
    if ((MILLIS_TIMSK & _BV(MILLIS_TOIE)) == 0) {
        // still disabled -> compensate
        timer0_millis += aMillisToAddForCompensation;
    }
    sbi(MILLIS_TIMSK, MILLIS_TOIE);
}
#endif // defined(MILLIS_TIMSK)
#endif //  defined(__AVR__)

#if ! defined(TEENSYDUINO)
void delayMilliseconds(unsigned int aMillis) {
    for (unsigned int i = 0; i < aMillis; ++i) {
        delayMicroseconds(1000);
    }
}

/*
 * returns true if aMillis were gone after the last return of true
 * Can be used as a correct non blocking replacement for delay()
 * Simple version, which can only be used at one place in code because of static variable.
 */
bool areMillisGone(unsigned int aMillis) {
    static unsigned long sLastMillis;
    if (millis() - sLastMillis >= aMillis) {
        sLastMillis = millis();
        return true;
    }
    return false;
}

bool areMillisGone(unsigned int aMillis, unsigned long *aLastMillisPtr) {
    if (millis() - *aLastMillisPtr >= aMillis) {
        *aLastMillisPtr = millis();
        return true;
    }
    return false;
}
#endif // ! defined(TEENSYDUINO)

/*
 * Function for speedTest
 * calling a function consisting of just __asm__ volatile ("nop"); gives 0 to 1 micro second
 * Use of Serial. makes it incompatible with BlueDisplay library.
 */
void speedTestWith1kCalls(Print *aSerial, void (*aFunctionUnderTest)(void)) {
    uint32_t tMillisStart = millis();
    for (uint_fast8_t i = 0; i < 100; ++i) {
        // unroll 10 times
        aFunctionUnderTest();
        aFunctionUnderTest();
        aFunctionUnderTest();
        aFunctionUnderTest();
        aFunctionUnderTest();
        aFunctionUnderTest();
        aFunctionUnderTest();
        aFunctionUnderTest();
        aFunctionUnderTest();
        aFunctionUnderTest();
    }
    uint32_t tMillisRequired = millis() - tMillisStart;
    aSerial->print(F("Function call takes "));
    if (tMillisRequired >= 1000) {
        aSerial->print(tMillisRequired / 1000);
        aSerial->print(".");
        aSerial->print((tMillisRequired % 1000) / 100);
        aSerial->print(F(" milli"));
    } else {
        aSerial->print(tMillisRequired);
        aSerial->print(F(" micro"));
    }
    aSerial->println(F(" seconds."));
}
