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

#include <Arduino.h>
#include "digitalWriteFast.h"

void sendUSART(char aChar);
void sendUnsignedIntegerHex(uint16_t aInteger);
void sendLineFeed();
void sendPCHex(uint16_t aPC);
void sendHex(uint16_t aInteger, char aName);

/*
 * INT0 is at pin2
 */
void enableINT0InterruptOnFallingEdge() {
    pinMode(2, INPUT_PULLUP);
    // interrupt on falling
#if defined(EICRA)
    EICRA |= _BV(ISC01);
    // clear interrupt bit
    EIFR |= _BV(INTF0);
    // enable interrupt on next change
    EIMSK |= _BV(INT0);
#elif defined(GIFR)
    MCUCR |= _BV(ISC01);
    // clear interrupt bit
    GIFR |= _BV(INTF0);
    // enable interrupt on next change
    GIMSK |= _BV(INT0);
#endif

}

extern char __init;
extern char _etext;
void printTextSectionAddresses() {
    Serial.print(F("Start of text section=0x"));
    Serial.print((uint16_t) &__init, HEX);
    Serial.print(F(" end=0x"));
    Serial.println((uint16_t) &_etext, HEX);
}

/*
 * Prints PC from stack
 * the amount of pushes for this ISR is compiler and compiler flag dependent
 * I saw 15, 17, 19 and 20 pushes. They can be found by looking for <__vector_1> in the lss file.

 * The assembler file can be generated with avr-objdump --section-headers --source --line-numbers <myfilename>.elf  > <myfilename>.lss.
 * The ATTinyCore board package generates this assembler as a *.lst file.
 * If you use the -g flag for the linker, the c source code content is also available in the lss file.
 *
 * Currently commented out:
 * The ISR starts now with the defined constant, but looks for an zero byte of the first R1 push on the stack
 * This register is always held zero, except if we interrupt an MUL operation.
 */
#define NUMBER_OF_PUSH 15
//#define NUMBER_OF_PUSH 17
//#define NUMBER_OF_PUSH 19

ISR(INT0_vect) {
    uint8_t * tStackPtr = (uint8_t *) SP;
    // If we have 17 pushs and pops for this function, add 17+1
    // Search the stack for zero and adjust NUMBER_OF_PUSH
    uint8_t tPushAdjust = 0;
    tStackPtr = tStackPtr + NUMBER_OF_PUSH;
//    while (*(tStackPtr + tPushAdjust) != 0) {
//        tPushAdjust++;
//    }
    tStackPtr += tPushAdjust + 1;
    // cannot load 16 bit directly, since bytes are swapped then
    uint16_t tPC = *tStackPtr;
    tPC <<= 8;
    tPC |= *(tStackPtr + 1);
    tPC <<= 1;   // Generate LSB. The program counter points only to even addresses and needs no LSB.
    /*
     * 0000 __vectors (interrupt vector table)
     * Startup from __ctors_start to __bad_interrupt
     * PC has value from __bad_interrupt + 2 where e.g. micros() start
     * to __stop_program
     */
    if (tPC >= (uint16_t) &__init && tPC <= (uint16_t) &_etext) {
        sendPCHex(tPC); // PC=0x03DC (11 character@115200 baud) => 954,86 us or 1047,2727 Hz rate. But I had to trigger it externally with 1070 Hz to get no misses.
//        sendHex(tPC, tPushAdjust + 0x41);
    } else {
        sendHex(tPC, tPushAdjust + 0x61); // no valid address
    }
}

/*
 * Generate 2 kHz square wave at pin 11 to connect it with INT0 pin 2
 */
void startTracingSignal() {
    TCCR2A = 0; // set entire TCCR2A register to 0 - Normal mode
    TCCR2B = _BV(CS21) | _BV(CS20); // Clock/32 => 2 us / 512 us period
    OCR2A = 125 - 1; // set compare match register for 50% duty cycle
    TCNT2 = 0; // initialize counter value. 1. pulse generated after first period, so generate 1. pulse manually below.

    // generate first pulse manually
    pinModeFast(11, OUTPUT);
    digitalWriteFast(11, HIGH);
    digitalWriteFast(11, LOW);

    // connect OC2A / pin 11
    TCCR2A = _BV(COM2A1) | _BV(COM2A0) | _BV(WGM21) | _BV(WGM20); // Set OC2A/PB3/D11 on compare match, clear at 00 / Fast PWM mode with 0xFF as TOP
}

void stopTracingSignal() {
    TCCR2A = 0; // disconnect OC2A / pin 11
    TCCR2B = 0; // No clock, stop counter
//    1/2020 This disables the timer signal output at startTracingSignal, not the manual pulse generation, but I don't know why.
//    pinModeFast(11, INPUT); // default behavior of pin 11 is input if we additionally connect the INT0 pin to a button connecting to ground.
}

void sendPCHex(uint16_t aPC) {
    sendUSART('P');
    sendUSART('C');
    sendUSART('=');
    sendUnsignedIntegerHex(aPC);
    sendLineFeed();
}

void sendHex(uint16_t aInteger, char aName) {
    sendUSART(aName);
    sendUSART('=');
    sendUnsignedIntegerHex(aInteger);
    sendLineFeed();
}

/*
 * for own debugging
 */
void sendHexNoInterrupts(uint16_t aInteger, char aName) {
    noInterrupts();
    sendUSART(aName);
    sendUSART('=');
    sendUnsignedIntegerHex(aInteger);
    sendLineFeed();
    interrupts();
}
/**
 * ultra simple blocking USART send routine - works 100%!
 */
void sendUSART(char aChar) {
// wait for buffer to become empty
#  if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__) || defined(ARDUINO_AVR_LEONARDO) || defined(__AVR_ATmega16U4__) || defined(__AVR_ATmega32U4__)
    // Use TX1 on MEGA and on Leonardo, which has no TX0
    while (!((UCSR1A) & (1 << UDRE1))) {
        ;
    }
    UDR1 = aChar;
#  else
    while (!((UCSR0A) & (1 << UDRE0))) {
        ;
    }
    UDR0 = aChar;
#  endif // Atmega...
}

void sendLineFeed() {
    sendUSART('\r');
    sendUSART('\n');
}

char nibbleToHex(uint8_t aByte) {
    aByte = aByte & 0x0F;
    if (aByte < 10) {
        return aByte + '0';
    }
    return aByte + 'A' - 10;
}

void sendUnsignedByteHex(uint8_t aByte) {
    sendUSART(nibbleToHex(aByte >> 4));
    sendUSART(nibbleToHex(aByte));
}

void sendUnsignedIntegerHex(uint16_t aInteger) {
    sendUSART('0');
    sendUSART('x');
    sendUnsignedByteHex(aInteger >> 8);
    sendUnsignedByteHex(aInteger);
}

