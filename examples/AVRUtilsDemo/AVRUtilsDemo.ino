/*
 *  AVRUtilsDemo.cpp
 *
 *  Demo of using 4 seconds sleep on an ATtiny
 *
 *  Copyright (C) 2020  Armin Joachimsmeyer
 *  armin.joachimsmeyer@gmail.com
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

 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

// ATMEL ATTINY85 / ARDUINO
//
//                       +-\/-+
// RESET/ADC0 (D5) PB5  1|    |8  VCC
//  USB- ADC3 (D3) PB3  2|    |7  PB2 (D2) INT0/ADC1 - default TX Debug output for ATtinySerialOut
//  USB+ ADC2 (D4) PB4  3|    |6  PB1 (D1) MISO/DO/AIN1/OC0B/OC1A/PCINT1 - (Digispark) LED
//                 GND  4|    |5  PB0 (D0) OC0A/AIN0
//                       +----+
//  USB+ and USB- are each connected to a 3.3 volt Zener to GND and with a 68 ohm series resistor to the ATtiny pin.
//  On boards with a micro USB connector, the series resistor is 22 ohm instead of 68 ohm.
//  USB- has a 1.5k pullup resistor to indicate a low-speed device.
//  USB+ and USB- are each terminated on the host side with 15k to 25k pull-down resistors.
#include <Arduino.h>

#if defined(__AVR__)

#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__) \
    || defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) \
    || defined(__AVR_ATtiny87__) || defined(__AVR_ATtiny167__) \
    || defined(__AVR_ATtiny88__)
#define CODE_FOR_ATTINY
#endif

#include "AVRUtils.h"

#if defined(CODE_FOR_ATTINY)
#include "ATtinySerialOut.hpp" // Available as Arduino library "ATtinySerialOut"
#else

/*
 * !!! THIS WORKS ONLY WITH VERSION 8.0 OF THE OPTIBOOT BOOTLOADER !!!
 First, we need a variable to hold the reset cause that can be written before
 early sketch initialization (that might change r2), and won't be reset by the
 various initialization code.
 avr-gcc provides for this via the ".noinit" section.
 */
uint8_t sMCUSR __attribute__ ((section(".noinit")));

/*
 Next, we need to put some code to save reset cause from the bootloader (in r2)
 to the variable.  Again, avr-gcc provides special code sections for this.
 If compiled with link time optimization (-flto), as done by the Arduino
 IDE version 1.6 and higher, we need the "used" attribute to prevent this
 from being omitted.
 */
void resetFlagsInit(void) __attribute__ ((naked))
__attribute__ ((used))
__attribute__ ((section (".init0")));
void resetFlagsInit(void) {
    /*
     save the reset flags passed from the bootloader
     This is a "simple" matter of storing (STS) r2 in the special variable
     that we have created. We use assembler to access the right variable.
     */
    __asm__ __volatile__ ("sts %0, r2\n" : "=m" (sMCUSR) :);
}

#endif

bool sBootReasonWasPowerUp = false;

//#include "ShowInfo.h"

#define VERSION_EXAMPLE "1.0"

#define LED_PIN  PB1
#define TONE_OUT_PIN 4
#define BEEP_FREQUENCY 2100
#define BEEP_START_DURATION_MILLIS 400
#define BEEP_DEFAULT_DURATION_MILLIS 80

#if defined(CODE_FOR_ATTINY)
// Pin 1 has an LED connected on my Digispark board.
#  if (LED_PIN == TX_PIN)
#error LED pin must not be equal TX pin (pin 2).
#  endif

uint8_t sMCUSRStored; // content of MCUSR register at startup
#endif

void setup() {

#if defined(CODE_FOR_ATTINY)
    /*
     * store MCUSR early for later use
     */
    if (MCUSR != 0) {
        sMCUSRStored = MCUSR; // content of MCUSR register at startup
        MCUSR = 0; // to prepare for next boot.
    } else {
        sMCUSRStored = GPIOR0; // Micronucleus puts a copy here if bootloader is in ENTRY_EXT_RESET mode
        GPIOR0 = 0; // Clear it to detect a jmp 0
    }

    /*
     * Initialize the serial pin as an output for Serial.print like debugging
     */
    initTXPin();
    writeString(F("START " __FILE__ "\nVersion " VERSION_EXAMPLE " from " __DATE__ "\nMCUSR="));
    writeUnsignedByteHexWithPrefix(sMCUSRStored);
    write1Start8Data1StopNoParity('\n');
#else
    MCUSR = 0;
    if (sMCUSR & (1 << PORF)) {
        sBootReasonWasPowerUp = true; // always true for old Optiboot bootloader
    }

    Serial.begin(115200);

    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ "\r\nVersion " VERSION_EXAMPLE " from  " __DATE__ "\nMCUSR="));
    Serial.print(F("sMCUSR=0x"));
    Serial.print(sMCUSR, HEX);
    Serial.print(F(" BootReasonWasPowerUp="));
    Serial.println(sBootReasonWasPowerUp);
#endif

    // This requires ShowInfo
//    printMCUSR(sMCUSRStored);
//    printBODLevel();
//    printFuses();

    pinMode(LED_PIN, OUTPUT);

    // activate LED
    delay(1000); // wait extra second after bootloader blink
    digitalWrite(LED_PIN, 1);
    delay(200);
    // deactivate LED
    digitalWrite(LED_PIN, 0);
    delay(200);

    tone(TONE_OUT_PIN, BEEP_FREQUENCY, BEEP_START_DURATION_MILLIS);
    delay(BEEP_START_DURATION_MILLIS);

    Serial.print(F("Stack+RAM="));
    Serial.println(getFreeRam());

    /*
     * init sleep mode and wakeup period
     */
    initSleep(SLEEP_MODE_PWR_DOWN);

    // Disable digital input buffer to save power has no effect on power down current if inputs are in a determined state or terminated.
    // Do not disable buffer for outputs whose values are read back
    // Disable it for pins used as ADC inputs!!!
    DIDR0 = _BV(ADC1D) | _BV(ADC2D) | _BV(ADC3D) | _BV(AIN1D) | _BV(AIN0D);

    /*
     * This disables USI and timer 0 and 1. this disables thus Arduino delay() and millis() and its interrupts which kills the deep sleep.
     * Do NOT set the PRADC bit WITHOUT disabling ADC (ADCSRA = 0) before!!!
     * Otherwise ADC can NOT disabled by (ADCSRA = 0) anymore and always consumes 200 uA!
     */
//    PRR = _BV(PRTIM1) | _BV(PRTIM0) | _BV(PRUSI); // Disable timer 0 and USI - has no effect on Power Down current
}

void loop() {

    // 4 second CPU idle
    for (int i = 0; i < 100; ++i) {
        delayMicroseconds(40000);
    }
    digitalWrite(LED_PIN, HIGH);
    delayMicroseconds(20000);
    digitalWrite(LED_PIN, LOW);
    ADCSRA = 0; // disable ADC just before sleep -> saves 200 uA
    sleepWithWatchdog(WDTO_4S, true); // Sleep 4 seconds
}
#else
#error This source is only for ATTinies
#endif
