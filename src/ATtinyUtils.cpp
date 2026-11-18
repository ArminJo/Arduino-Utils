/*
 * ATtinyUtils.cpp
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

#if defined(__AVR__) && (defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny87__) || defined(__AVR_ATtiny167__))

//#define DEBUG
#if defined(DEBUG)
// Should be first include to avoid unwanted use of Serial object defined in HardwareSerial
#include "ATtinySerialOut.h"
#endif

#include "ATtinyUtils.h"

#include <avr/boot.h>  // required for boot_signature_byte_get()
#include <avr/power.h> // required for clock_prescale_set()
#include <avr/sleep.h> // required for isBODSFlagExistent()
#include "digitalWriteFast.h"

/*
 * Use port pin number (PB0-PB5) not case or other pin number
 */
void toggleFastPortB(uint8_t aOutputPinNumber) {
    PINB = (1 << aOutputPinNumber);
}

// for constant values we can also use: digitalWriteFast()
void digitalWriteFastPortB(uint8_t aOutputPinNumber, bool aValue) {
    if (aValue) {
        PORTB |= (1 << aOutputPinNumber);
    } else {
        PORTB &= ~(1 << aOutputPinNumber);
    }
}

// for constant values we can also use: digitalReadFast()
bool digitalReadFastPortB(uint8_t aInputPinNumber) {
    return (PINB & (1 << aInputPinNumber));
}

// not for INPUT_PULLUP - can be done by setting to input and adding digitalWriteFastPortB(aOutputPinNumber,1);
void pinModeFastPortB(uint8_t aOutputPinNumber, uint8_t aMode) {
    (aMode ? DDRB |= (1 << aOutputPinNumber) /* OUTPUT */: DDRB &= ~(1 << aOutputPinNumber));
}

#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)

/*
 * Generates frequency signal and inverted signal to connect buzzer between 2 pins in order to increase volume.
 * PWM mode is chosen, because the inverted outputs are only enabled in this mode.
 * If you do not require both outputs, simply disable the "pinModeFast(PBX, OUTPUT);" statements for the unused outputs.
 * initialize outputs and use PWM Mode
 * @param aUseOutputB  - true:  output at Pin3/2 - PB4/PB3 - OCR1B/!OCR1B
 *                     - false: output at Pin6/5 - PB1/PB0 - OCR1A/!OCR1A
 */
void toneWithTimer1PWM(uint16_t aFrequencyHertz, bool aUseOutputB) {
    uint8_t tPrescaler = 0x01;
    uint32_t tOCR = F_CPU / aFrequencyHertz; // uint32_t to avoid overflow at low frequencies
    while (tOCR > 0x100 && tPrescaler < 0x0F) {
        tPrescaler++;
        tOCR >>= 1;
    }

    OCR1C = tOCR - 1; // The frequency of the PWM will be Timer Clock 1 frequency divided by (OCR1C value + 1).

    if (aUseOutputB) {
        OCR1B = tOCR / 2; // 50% PWM
        pinModeFast(PB3, OUTPUT);
        pinModeFast(PB4, OUTPUT);
        GTCCR = (1 << PWM1B) | (1 << COM1B0); // PWM Mode with OCR1B (PB4) + !OCR1B (PB3) outputs enabled
        TCCR1 = tPrescaler;
    } else {
        OCR1A = tOCR / 2; // 50% PWM
        pinModeFast(PB1, OUTPUT);
        pinModeFast(PB0, OUTPUT);
        GTCCR = 0;
        TCCR1 = (1 << PWM1A) | (1 << COM1A0) | tPrescaler; // PWM Mode with OCR1A (PB1) + !OCR1A (PB0) outputs enabled
    }
}

void noToneWithTimer1PWM() {
    TCCR1 = 0; // Disconnect pins and stop timer
}

// no check for aFrequency == 0!
void periodicInterruptWithTimer1(uint16_t aFrequency) {
    uint8_t tPrescaler = 0x01;
    uint32_t tOCR = F_CPU / aFrequency; // uint32_t to avoid overflow at low frequencies
    while (tOCR > 0x100 && tPrescaler < 0x0F) {
        tPrescaler++;
        tOCR >>= 1;
    }
    OCR1C = tOCR - 1;                     // period
    OCR1A = tOCR - 1;                     // interrupt point inside the period
    TCCR1 = (1 << CTC1) | tPrescaler;     // assign, don't OR
    TIMSK |= (1 << OCIE1A);
}
#endif

uint8_t getBODLevelFuses() {
    return boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS) & 0x07; // BODLEVEL2..0 = bit 2..0
}

bool isBODLevelBelow2_7VoltOrDisabled() {
    return (getBODLevelFuses() >= 6);
}

/*
 * @return true if BODS flag is existent - should be true for ATtiny85 revision C and later
 */
bool isBODSFlagExistent() {
    sleep_bod_disable()
    ;
    /*
     * Check if flag can be set - this works only for ATtini85 revision C, which is quite unpopular (2019) on Chinese boards.
     */
    return MCUCR & _BV(BODS); // BODS is cleared by hardware after 3 clock cycles after sleep_bod_disable()
}

/*
 * Code to change clock settings to get the right CPU frequency if not already done by core. (menu clock select etc.)
 * Call it if you want to use the standard ATtiny library, BUT do not call it, if you need Digispark USB functions available for 16 MHz.
 */
void changeDigisparkClock() {
    uint8_t tLowFuse = boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    if ((tLowFuse & 0x0F) == 0x01) { // CKSEL3..0 = 0001 -> PLL clock (16 MHz)
#elif defined(__AVR_ATtiny87__) || defined(__AVR_ATtiny167__)
    if ((tLowFuse & 0x0E) == 0x0E) { // // CKSEL3..1 = 111x -> external crystal >= 8 MHz
#endif
        /*
         * Here we have High Frequency (PLL) Clock ( 16 MHz)
         */
#if (F_CPU == 1000000)
        // Divide 16 MHz clock by 16 for Digispark Boards to get the requested 1 MHz
        clock_prescale_set(clock_div_16);
//        CLKPR = (1 << CLKPCE);  // unlock function
//        CLKPR = (1 << CLKPS2); // 0x04 -> divide by 16
#elif (F_CPU == 8000000)
        // Divide 16 MHz clock by 2 for Digispark Boards to get the requested 8 MHz
        clock_prescale_set(clock_div_2);
//        CLKPR = (1 << CLKPCE);  // unlock function
//        CLKPR = (1 << CLKPS0); // 0x01 -> divide by 2
#endif
    }

    /*
     * Code to reset micronucleus OCCAL tweak
     */
#if !defined(SIGRD)
#define  SIGRD  5 // required for boot_signature_byte_get()
#endif
    uint8_t tStoredOSCCAL = boot_signature_byte_get(1);
    if (OSCCAL != tStoredOSCCAL) {
#if defined(DEBUG)
        uint8_t tOSCCAL = OSCCAL;
#endif
        // retrieve the factory-stored oscillator calibration bytes to revert the Digispark OSCCAL tweak
        OSCCAL = tStoredOSCCAL;
#if defined(DEBUG)
        // write after resetting OSCCAL otherwise baud rate may be wrong
        writeString(F("Changed OSCCAL from "));
        writeUnsignedByteHex(tOSCCAL);
        writeString(F(" to "));
        writeUnsignedByteHex(tStoredOSCCAL);
        writeCRLF();
#endif
    }
}
#elif defined(ARDUINO_ARCH_APOLLO3)
    void AttinyUtilsDummyToAvoidBFDAssertions(){
        ;
    }
#endif //  defined(__AVR_ATtiny85__)
