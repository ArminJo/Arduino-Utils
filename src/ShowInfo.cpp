/*
 * ShowInfo.cpp
 *
 *  Copyright (C) 2018-2026  Armin Joachimsmeyer
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

#if defined(__AVR__)

#include <Arduino.h>

#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny167__) || defined(__AVR_ATtiny87__)
#include "ATtinySerialOut.h"
#include "ADCUtils.h" // for ADC_GND_CHANNEL_MUX etc.
#endif

// Based on https://playground.arduino.cc/Main/ShowInfo

#include <avr/boot.h>

// Helper function for sketch size.
// The sketch size is runtime calculated.
// From user "Coding Badly" in his post:
//   http://arduino.cc/forum/index.php/topic,115870.msg872309.html#msg872309
// Changed into unsigned long for code size larger than 64kB.
//
// This function returns the sketch size
// for a size between 0 and 32k. If the code
// size is larger (for example with an Arduino Mega),
// the return value is not valid.
//
unsigned long sketchSize(void) {
    extern int _etext;
    extern int _edata;

    return ((unsigned long) (&_etext) + ((unsigned long) (&_edata) - RAMSTART));
}

void Information(void) {
#if !(defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny167__) || defined(__AVR_ATtiny87__))
    int data1, data2, data3, data4;
    unsigned long ul;
    float percentage;

    Serial.println();
#if !defined(__AVR_ATmega32U4__)
    Serial.println(F("Information"));
    Serial.println(F("-----------"));

    Serial.print(F("sketch Size = "));
    ul = sketchSize();
    Serial.print(ul, DEC);
    Serial.print(F(" ("));
    percentage = (float) ul / ((float) FLASHEND + 1.0) * 100.0;
    Serial.print(percentage, 0);
    Serial.println(F("%)"));
#endif

#if defined(ARDUINO)
    Serial.print(F("ARDUINO = "));
    Serial.print(ARDUINO);
    Serial.print(F(" Arduino version = "));
    Serial.print(ARDUINO / 10000);         // 1
    Serial.print('.');
    Serial.print((ARDUINO / 100) % 100);   // 8
    Serial.print('.');
    Serial.println(ARDUINO % 100);         // 19
#endif

    Serial.print(F("__VERSION__ = "));
    Serial.println(F(__VERSION__));

    Serial.print(F("__DATE__    = "));
    Serial.println(F(__DATE__));

    Serial.print(F("__TIME__    = "));
    Serial.println(F(__TIME__));

    Serial.print(F("__AVR_LIBC_VERSION_STRING__ = "));
    Serial.println(F(__AVR_LIBC_VERSION_STRING__));

    Serial.print(F("__FILE__    = "));
    Serial.println(F(__FILE__));

    Serial.print(F("__STDC__    = "));
    Serial.println(__STDC__, DEC);

#if !defined(__AVR_ATmega32U4__)
    Serial.print(F("OSCCAL = "));
    Serial.println(OSCCAL, DEC);

    Serial.print(F("GPIOR0 = 0x"));
    Serial.println(GPIOR0, HEX);

    Serial.print(F("GPIOR1 = 0x"));
    Serial.println(GPIOR1, HEX);

    Serial.print(F("GPIOR2 = 0x"));
    Serial.println(GPIOR2, HEX);
#endif

    Serial.print(F("RAMEND   = 0x"));
    Serial.println(RAMEND, HEX);

    Serial.print(F("XRAMEND  = 0x"));
    Serial.println(XRAMEND, HEX);

    Serial.print(F("E2END    = 0x"));
    Serial.println(E2END, HEX);

    Serial.print(F("FLASHEND = 0x"));
    Serial.println(FLASHEND, HEX);

    noInterrupts();
    data1 = boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
    data2 = boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);
    data3 = boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS);
    data4 = boot_lock_fuse_bits_get(GET_LOCK_BITS);
    interrupts();

    Serial.print(F("LOW FUSE      = 0x"));
    Serial.println(data1, HEX);

    Serial.print(F("HIGH FUSE     = 0x"));
    Serial.println(data2, HEX);

    Serial.print(F("EXTENDED FUSE = 0x"));
    Serial.println(data3, HEX);

    Serial.print(F("LOCK BITS     = 0x"));
    Serial.println(data4, HEX);

    Serial.print(F("Processor according to compiler = "));
#if defined(__AVR_ATtiny45__)
    Serial.println(F("__AVR_ATtiny45__"));
#elif defined(__AVR_ATtiny85__)
    Serial.println(F("__AVR_ATtiny85__"));
#elif defined(__AVR_ATtiny87__)
    Serial.println(F("__AVR_ATtiny87__"));
#elif defined(__AVR_ATtiny167__)
    Serial.println(F("__AVR_ATtiny167__"));
#elif defined(__AVR_ATtiny2313__)
    Serial.println(F("__AVR_ATtiny2313__"));
#elif defined(__AVR_ATtiny2313A__)
    Serial.println(F("__AVR_ATtiny2313A__"));
#elif defined(__AVR_ATmega48__)
    Serial.println(F("__AVR_ATmega48__"));
#elif defined(__AVR_ATmega48A__)
    Serial.println(F("__AVR_ATmega48A__"));
#elif defined(__AVR_ATmega48P__)
    Serial.println(F("__AVR_ATmega48P__"));
#elif defined(__AVR_ATmega8__)
    Serial.println(F("__AVR_ATmega8__"));
#elif defined(__AVR_ATmega8U2__)
    Serial.println(F("__AVR_ATmega8U2__"));
#elif defined(__AVR_ATmega88__)
    Serial.println(F("__AVR_ATmega88__"));
#elif defined(__AVR_ATmega88A__)
    Serial.println(F("__AVR_ATmega88A__"));
#elif defined(__AVR_ATmega88P__)
    Serial.println(F("__AVR_ATmega88P__"));
#elif defined(__AVR_ATmega88PA__)
    Serial.println(F("__AVR_ATmega88PA__"));
#elif defined(__AVR_ATmega16__)
    Serial.println(F("__AVR_ATmega16__"));
#elif defined(__AVR_ATmega168__)
    Serial.println(F("__AVR_ATmega168__"));
#elif defined(__AVR_ATmega168A__)
    Serial.println(F("__AVR_ATmega168A__"));
#elif defined(__AVR_ATmega168P__)
    Serial.println(F("__AVR_ATmega168P__"));
#elif defined(__AVR_ATmega32__)
    Serial.println(F("__AVR_ATmega32__"));
#elif defined(__AVR_ATmega328__)
    Serial.println(F("__AVR_ATmega328__"));
#elif defined(__AVR_ATmega328P__)
    Serial.println(F("__AVR_ATmega328P__"));
#elif defined(__AVR_ATmega32U2__)
    Serial.println(F("__AVR_ATmega32U2__"));
#elif defined(__AVR_ATmega32U4__)
    Serial.println(F("__AVR_ATmega32U4__"));
#elif defined(__AVR_ATmega32U6__)
    Serial.println(F("__AVR_ATmega32U6__"));
#elif defined(__AVR_ATmega128__)
    Serial.println(F("__AVR_ATmega128__"));
#elif defined(__AVR_ATmega1280__)
    Serial.println(F("__AVR_ATmega1280__"));
#elif defined(__AVR_ATmega2560__)
    Serial.println(F("__AVR_ATmega2560__"));
#endif

#if !defined(SIGRD)
    Serial.println(F("SIGRD : not defined -> let's make it 5 and see what happens"));
#define SIGRD 5
#endif
    Serial.print(F("SIGRD = "));
    Serial.println(SIGRD, DEC);

    Serial.print(F("Signature = 0x"));

    data1 = boot_signature_byte_get(0x00);
    data2 = boot_signature_byte_get(0x02);
    data3 = boot_signature_byte_get(0x04);
    data4 = boot_signature_byte_get(0x01);

    Serial.print(data1, HEX);
    Serial.print(F(", 0x"));
    Serial.print(data2, HEX);
    Serial.print(F(", 0x"));
    Serial.println(data3, HEX);

    Serial.print(F("calibration = "));
    Serial.println(data4, DEC);

#if !defined(__AVR_ATmega32U4__)
    Serial.print(F("Number of seconds since start = "));
    Serial.println(millis() / 1000L, DEC);
#endif

    Serial.println(F("UTF-8 test:"));
    Serial.println(F("Micro µ"));
    Serial.println(F(" Euro €"));
    Serial.println(F("  (c) ©"));

    Serial.println(F("-----------"));
#endif // !defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny167__) || defined(__AVR_ATtiny87__)
}

void printMCUSR(uint8_t aMCUSRContent) {
#if defined(WDRF)
    if (aMCUSRContent & (1 << WDRF)) {
        Serial.print(F(" Watchdog"));
    }
#endif
#if defined(BORF)
    if (aMCUSRContent & (1 << BORF)) {
        Serial.print(F(" Brownout"));
    }
#endif
#if defined(EXTRF)
    if (aMCUSRContent & (1 << EXTRF)) {
        Serial.print(F(" Reset"));
    }
#endif
#if defined(PORF)
    if (aMCUSRContent & (1 << PORF)) {
        Serial.print(F(" PowerOn"));
    }
#endif
    Serial.println();
}

/*
 * Fuse byte that contains BODLEVEL2:0
 * ATtinies: high fuse; most ATmegas: extended fuse. In both cases bits 2:0.
 */
#if defined(__AVR_ATmega48__) || defined(__AVR_ATmega48A__) || defined(__AVR_ATmega48P__) || defined(__AVR_ATmega48PB__) \
    || defined(__AVR_ATmega88__) || defined(__AVR_ATmega88A__) || defined(__AVR_ATmega88P__) || defined(__AVR_ATmega88PA__) || defined(__AVR_ATmega88PB__) \
    || defined(__AVR_ATmega168__) || defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega168PA__) || defined(__AVR_ATmega168PB__) \
    || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__) \
    || defined(__AVR_ATmega164P__) || defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega324PB__) \
    || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) \
    || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega32U4__)
#define GET_BOD_LEVEL_FUSE_BITS   GET_EXTENDED_FUSE_BITS
#else
#define GET_BOD_LEVEL_FUSE_BITS   GET_HIGH_FUSE_BITS  // ATtiny25/45/85, 24/44/84, 87/167, 2313
#endif
#define BOD_LEVEL_MASK  0x07 // BODLEVEL2:0 are always bits 2:0

/*
 * @param aFuseBitsContainingBODLevel  High fuse on ATtinies, extended fuse on most ATmegas (see GET_BOD_LEVEL_FUSE_BITS)
 */
void printBODLevel(uint8_t aFuseBitsContainingBODLevel) {
    Serial.print(F("Brown-out="));
    switch (aFuseBitsContainingBODLevel & BOD_LEVEL_MASK) {
    // 0-3 are reserved codes
    case 4:
        Serial.print(F("4.3V"));
        break;
    case 5:
        Serial.print(F("2.7V"));
        break;
    case 6:
        Serial.print(F("1.8V"));
        break;
    case 7:
        Serial.print(F("disabled"));
        break;
    default:
        Serial.print(F("reserved"));
        break;
    }
    Serial.println();
}

void printBODLevel() {
    printBODLevel(boot_lock_fuse_bits_get(GET_BOD_LEVEL_FUSE_BITS));
}

#if defined(__AVR_ATtiny85__)
/*
 * Output description for all fuses except "DebugWire enabled"
 */
void printFuses(void) {
    uint8_t tLowFuseBits = boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
    Serial.println();
    Serial.print(F("LowFuses=0x"));
    Serial.println(tLowFuseBits, HEX);

    Serial.print(F("Clock divide by 8"));
    if (tLowFuseBits & ~FUSE_CKDIV8) {
        Serial.print(F(" not"));
    }
    Serial.println(F(" enabled")); // enabled is default

    Serial.print(F("Clock output"));
    if (tLowFuseBits & ~FUSE_CKOUT) {
        Serial.print(F(" not"));
    }
    Serial.println(F(" enabled")); // enabled is default

    Serial.print(F("Clock select="));
    uint8_t tClockSelectBits = tLowFuseBits & (~FUSE_CKSEL3 | ~FUSE_CKSEL2 | ~FUSE_CKSEL1 | ~FUSE_CKSEL0 );
    switch (tClockSelectBits) {
    case 1:
        Serial.print(F("16MHz PLL"));
        break;
    case 2:
        Serial.print(F("8MHz")); // default
        break;
    case 3:
        Serial.print(F("6.4MHz"));
        break;
    case 4:
        Serial.print(F("128kHz"));
        break;
    case 6:
        Serial.print(F("32.768kHz"));
        break;
    default:
        Serial.print(F("External"));
        break;
    }

    Serial.println();
    Serial.print(F("Start-up time="));
    uint8_t tStartUptimeBits = tLowFuseBits & (~FUSE_SUT1 | ~FUSE_SUT0 );
    if (tClockSelectBits == 1) {
        /*
         * PLL Clock has other interpretation of tStartUptimeBits
         */
        Serial.print(F("(4ms from reset) + 14 CK "));
        if (tLowFuseBits & ~FUSE_SUT0) {
            Serial.print(F("+ 16384 CK")); // -> 1 ms for 16 MHz clock
        } else {
            Serial.print(F(" + 1024 CK"));
        }
        if (tLowFuseBits & ~FUSE_SUT1) {
            Serial.print(F(" + 64ms")); // default
        } else {
            Serial.print(F(" + 4ms"));
        }
    } else {
        /*
         * Internal Calibrated RC Oscillator Clock
         */
        Serial.print(F("6 CK (+ 14 CK"));
        switch (tStartUptimeBits) {
        case 0x10:
            Serial.print(F(" + 4ms"));
            break;
        case 0x20:
            Serial.print(F(" + 64ms")); // default
            break;
        default:
            break;
        }
        Serial.print(F(" from reset)"));

    }

    uint8_t tHighFuseBits = boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);
    Serial.println();
    Serial.println();
    Serial.print(F("HighFuses=0x"));
    Serial.println(tHighFuseBits, HEX);

    Serial.print(F("Reset"));
    if (tHighFuseBits & ~FUSE_RSTDISBL) {
        Serial.print(F(" not"));
    }
    Serial.println(F(" disabled"));

    Serial.print(F("Serial programming"));
    if (tHighFuseBits & ~FUSE_SPIEN) {
        Serial.print(F(" not"));
    }
    Serial.println(F(" enabled"));

    Serial.print(F("Watchdog always on"));
    if (tHighFuseBits & ~FUSE_WDTON) {
        Serial.print(F(" not"));
    }
    Serial.println(F(" enabled"));

    Serial.print(F("Preserve EEPROM"));
    if (tHighFuseBits & ~FUSE_EESAVE) {
        Serial.print(F(" not"));
    }
    Serial.println(F(" enabled"));

    printBODLevel(boot_lock_fuse_bits_get(GET_BOD_LEVEL_FUSE_BITS));

    uint8_t tExtFuseBits = boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS);
    Serial.println();
    Serial.print(F("ExtFuses=0x"));
    Serial.println(tExtFuseBits, HEX);
#if defined(FUSE_SELFPRGEN)
    Serial.print(F("Self programming"));
    if (tExtFuseBits & ~FUSE_SELFPRGEN) {
        Serial.print(F(" not"));
    }
    Serial.println(F(" enabled"));
#endif
    Serial.println();
}
#endif

void printBODSFlagExistence() {
#if defined(BODS) && defined(BODSE)
    /*
     * Turn off the brown-out detector - this works only for ATtini85 revision C, which I have not seen in the wild.
     */
    uint8_t tMcucrValue = MCUCR | _BV(BODS) | _BV(BODSE);  // set to one
    MCUCR = tMcucrValue; // set both flags to one
    MCUCR = tMcucrValue & ~_BV(BODSE); // reset BODSE within 4 clock cycles
    tMcucrValue = MCUCR;
    Serial.print(F("BODS flag"));
    if (!(tMcucrValue & _BV(BODS))) {
        Serial.print(F(" not"));
    }
    Serial.println(F(" existent"));
#else
    Serial.println(F("BODS flag not defined for this CPU"));
#endif
}

#if !defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny167__) || defined(__AVR_ATtiny87__)
/*
 * Short version using printHex and saving Flash
 */
void TimerCommonRegisterDump(void) {
#if defined(TIMSK)
    Serial.print(F("TIMSK="));
    Serial.printlnHex(TIMSK);
#endif
#if defined(TIFR)
    Serial.print(F("TIFR="));
    Serial.printlnHex(TIFR);
#endif
    Serial.print(F("GTCCR="));
    Serial.printlnHex(GTCCR);
}

void Timer0RegisterDump(void) {
    Serial.println(F("Timer0 register dump:"));
    Serial.print(F("TCCR0A="));
    Serial.printlnHex(TCCR0A);
    Serial.print(F("TCCR0B="));
    Serial.printlnHex(TCCR0B);
    Serial.print(F("OCR0A="));
    Serial.printlnHex(OCR0A);
#if defined(OCR0B)
    Serial.print(F("OCR0B="));
    Serial.printlnHex(OCR0B);
#endif
#if defined(ASSR)
    Serial.print(F("ASSR="));
    Serial.printlnHex(ASSR);
#endif

    Serial.print(F("TCNT0="));
    Serial.printlnHex(TCNT0);

#if defined(TIMSK0)
    Serial.print(F("TIMSK0="));
    Serial.printlnHex(TIMSK0);
#endif
#if defined(TIFR0)
    Serial.print(F("TIFR0="));
    Serial.printlnHex(TIFR0);
#endif

    TimerCommonRegisterDump();
    Serial.println();
}

void Timer1RegisterDump(void) {
    Serial.println(F("Timer1 register dump:"));
#if defined(TCCR1)
    Serial.print(F("TCCR1="));
    Serial.printlnHex(TCCR1);
#endif
#if defined(TCCR1A)
    Serial.print(F("TCCR1A="));
    Serial.printlnHex(TCCR1A);
#endif
#if defined(TCCR1B)
    Serial.print(F("TCCR1B="));
    Serial.printlnHex(TCCR1B);
#endif
#if defined(TCCR1C)
    Serial.print(F("TCCR1C="));
    Serial.printlnHex(TCCR1C);
#endif
#if defined(TCCR1D)
    Serial.print(F("TCCR1D="));
    Serial.printlnHex(TCCR1D);
#endif
#if defined(TCCR1E)
    Serial.print(F("TCCR1E="));
    Serial.printlnHex(TCCR1E);
#endif
    Serial.print(F("OCR1A="));
    Serial.printlnHex(OCR1A);
    Serial.print(F("OCR1B="));
    Serial.printlnHex(OCR1B);
#if defined(OCR1C)
    Serial.print(F("OCR1C="));
    Serial.printlnHex(OCR1C);
#endif
    Serial.print(F("TCNT1="));
    Serial.printlnHex(TCNT1);
#if defined(TIMSK1)
    Serial.print(F("TIMSK1="));
    Serial.printlnHex(TIMSK1);
#endif
#if defined(TIFR1)
    Serial.print(F("TIFR1="));
    Serial.printlnHex(TIFR1);
#endif

    TimerCommonRegisterDump();

    Serial.println();
}

void TimerRegisterDump(void) {
    Timer0RegisterDump();
    Timer1RegisterDump();
}

void ADCChannelDump(void) {
    Serial.println(F("ADC channel dump:"));
#if defined(A0)
    Serial.print(F("A0="));
    Serial.println(analogRead(A0));
#endif
#if defined(A1)
    Serial.print(F("A1="));
    Serial.println(analogRead(A1));
#endif
#if defined(A2)
    Serial.print(F("A2="));
    Serial.println(analogRead(A2));
#endif
#if defined(A3)
    Serial.print(F("A3="));
    Serial.println(analogRead(A3));
#endif
    Serial.print(F("1.1V="));
    Serial.println(analogRead(ADC_1_1_VOLT_CHANNEL_MUX));
    Serial.print(F("GND="));
    Serial.println(analogRead(ADC_GND_CHANNEL_MUX));
    Serial.print(F("Tmp="));
    Serial.println(analogRead(ADC_TEMPERATURE_CHANNEL_MUX));
    Serial.print(F("ADMUX="));
    Serial.printlnHex(ADMUX);
}
#else // defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny167__) || defined(__AVR_ATtiny87__)
void Timer0RegisterDump(void) {
    Serial.println(F("Timer0 Register dump:"));
#if defined(TCCR0A)
    Serial.print(F("TIMER0 TCCR0A = 0x"));
    Serial.println(TCCR0A, HEX);
#endif
    Serial.print(F("TIMER0 TCCR0B = 0x"));
    Serial.println(TCCR0B, HEX);
    Serial.print(F("TIMER0 OCR0A  = 0x"));
    Serial.println(OCR0A, HEX);
    Serial.print(F("TIMER0 OCR0B  = 0x"));
    Serial.println(OCR0B, HEX);
#if defined(TIMSK0)
    Serial.print(F("TIMER0 TIMSK0 = 0x"));
    Serial.println(TIMSK0, HEX);
#endif
    Serial.print(F("TIMER0 TCNT0  = 0x"));
    Serial.println(TCNT0, HEX);
#if defined(TIFR0)
    Serial.print(F("TIMER0 TIFR0  = 0x"));
    Serial.println(TIFR0, HEX);
#endif
}

#if defined(TCCR1A)
void Timer1RegisterDump(void) {
    Serial.println(F("Timer1 Register dump:"));
    Serial.print(F("TIMER1 TCCR1A = 0x"));
    Serial.println(TCCR1A, HEX);
    Serial.print(F("TIMER1 TCCR1B = 0x"));
    Serial.println(TCCR1B, HEX);
    Serial.print(F("TIMER1 TCCR1C = 0x"));
    Serial.println(TCCR1C, HEX);
    Serial.print(F("TIMER1 OCR1A  = 0x"));
    Serial.println(OCR1A, HEX);
    Serial.print(F("TIMER1 OCR1B  = 0x"));
    Serial.println(OCR1B, HEX);
    Serial.print(F("TIMER1 TIMSK1 = 0x"));
    Serial.println(TIMSK1, HEX);
    Serial.print(F("TIMER1 TCNT1  = 0x"));
    Serial.println(TCNT1, HEX);
    Serial.print(F("TIMER1 ICR1   = 0x"));
    Serial.println(ICR1, HEX);
    Serial.print(F("TIMER1 TIFR1  = 0x"));
    Serial.println(TIFR1, HEX);
}
#endif

#if defined(TCCR2A)
void Timer2RegisterDump(void) {
    Serial.println(F("Timer2 Register dump:"));
    Serial.print(F("TIMER2 TCCR2A = 0x"));
    Serial.println(TCCR2A, HEX);
    Serial.print(F("TIMER2 TCCR2B = 0x"));
    Serial.println(TCCR2B, HEX);
    Serial.print(F("TIMER2 OCR2A  = 0x"));
    Serial.println(OCR2A, HEX);
    Serial.print(F("TIMER2 OCR2B  = 0x"));
    Serial.println(OCR2B, HEX);
    Serial.print(F("TIMER2 TIMSK2 = 0x"));
    Serial.println(TIMSK2, HEX);
    Serial.print(F("TIMER2 TCNT2  = 0x"));
    Serial.println(TCNT2, HEX);
    Serial.print(F("TIMER2 TIFR2  = 0x"));
    Serial.println(TIFR2, HEX);
    Serial.print(F("TIMER2 ASSR   = 0x"));
    Serial.println(ASSR, HEX);
    Serial.print(F("TIMERn GTCCR  = 0x"));
    Serial.println(GTCCR, HEX);
}
#endif

void TimerRegisterDump(void) {
    Timer0RegisterDump();
#if defined(TCCR1A)
    Timer1RegisterDump();
#endif
#if defined(TCCR2A)
    Timer2RegisterDump();
#endif
}
#endif // defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny167__) || defined(__AVR_ATtiny87__)
#endif // !defined(__AVR_ATmega32U4__)
#endif // defined(__AVR__)
