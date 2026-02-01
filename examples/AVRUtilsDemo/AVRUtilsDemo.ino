/*
 *  AVRUtilsDemo.cpp
 *
 *  Demo of 2 seconds sleep with watchdog
 *  Demo of printRAMAndStackInfo() and printStackUnusedAndUsedBytes ()
 *
 *  Copyright (C) 2020-2026  Armin Joachimsmeyer
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

#include "AVRUtils.h"
#include "ShowInfo.h"
#include "HexDump.hpp"

#  if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__) \
    || defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) \
    || defined(__AVR_ATtiny87__) || defined(__AVR_ATtiny167__) \
    || defined(__AVR_ATtiny88__)
#  define CODE_FOR_ATTINY
#endif

#define VERSION_EXAMPLE "1.1"

#define TONE_OUT_PIN 4

#if defined(CODE_FOR_ATTINY)
#include "ATtinySerialOut.hpp" // Available as Arduino library "ATtinySerialOut"

#define SIZE_OF_DUMMY_ARRAY     0x100    // small heap is available

#define LED_PIN  PB1
// Pin 1 has an LED connected on my Digispark board.
#  if (LED_PIN == TX_PIN)
#error LED pin must not be equal TX pin (pin 2).
#  endif

#else

//#define SIZE_OF_DUMMY_ARRAY     1800    // Stack runs into data
//#define SIZE_OF_DUMMY_ARRAY     1700    // Stack is OK, but no heap is available
#define SIZE_OF_DUMMY_ARRAY     0x600    // 1536 Stack is OK, and small heap is available

#define LED_PIN  LED_BUILTIN
#endif
uint8_t sDummyArray[SIZE_OF_DUMMY_ARRAY] __attribute__((section(".noinit"))); // Place it at end of BSS to be first overwritten by stack.

uint8_t sMCUSRStored; // content of MCUSR register at startup

#define WATCHDOG_INFO_STRING_SIZE   16
/*
 * A variable to hold the reset cause written by the main program
 * It must not be overwritten by the initialization code after a reset.
 * avr-gcc provides this via the ".noinit" section.
 *
 * Independent of order in file, sDummyArray is allocated lower than sWatchdogResetInfoString :-(
 */
char sWatchdogResetInfoString[WATCHDOG_INFO_STRING_SIZE] __attribute__ ((section(".noinit")));
volatile uint16_t sNumberOfSleeps = 0;

// Helper macro for getting a macro definition as string
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

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

    initStackFreeMeasurement();

    /*
     * Initialize the serial pin as an output for Serial.print like debugging
     */
    initTXPin();
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ "\r\nVersion " VERSION_EXAMPLE " from  " __DATE__));
    Serial.println();
#else

    /*
     * This currently (3/2024) only works with optiboot 8.1 bootloader.
     * https://github.com/Optiboot/optiboot/blob/master/optiboot/bootloaders/optiboot/optiboot.c#L243
     * The standard Arduino bootloader overwrites MCUSR with 0 :-(.
     */
    sMCUSRStored = MCUSR; // content of MCUSR register at startup
    MCUSR = 0;
    wdt_disable();

    for (int i = 0; i < SIZE_OF_DUMMY_ARRAY; ++i) {
        sDummyArray[i] = 0; // Mark array with 0 to detect overwriting by StackFreeMeasurement
    }

    Serial.begin(115200);
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ "\r\nVersion " VERSION_EXAMPLE " from  " __DATE__));

    Serial.println();
    Serial.println(F("Print base RAM data"));
    printBaseRAMData(&Serial);
    Serial.flush();

    initStackFreeMeasurement();

#endif // defined(CODE_FOR_ATTINY)
    Serial.println();
    Serial.println(F("Print MCUSR, Brown-out level and dummy array info"));

    Serial.flush();

    Serial.print(F("sMCUSR=0x"));
    Serial.print(sMCUSRStored, HEX);
    if (sMCUSRStored != 0) {
        Serial.print(F(" => Boot reason is"));
        printMCUSR(sMCUSRStored);
    }
    Serial.println();

    if (sMCUSRStored & (1 << WDRF)) {
        Serial.println(sWatchdogResetInfoString);
    } else {
        strncpy(sWatchdogResetInfoString, "during setup", WATCHDOG_INFO_STRING_SIZE - 1);
        sWatchdogResetInfoString[WATCHDOG_INFO_STRING_SIZE - 1] = '\0'; // Terminate any string later copied with strncpy(..., ..., WATCHDOG_INFO_STRING_SIZE - 1);
    }
    Serial.flush();

    printBODLevel();
    Serial.flush();

    /*
     * Prints to avoid optimizing away the sDummyArray, which is first overwritten by stack.
     */
    sDummyArray[SIZE_OF_DUMMY_ARRAY - 1] = 0x42;
    Serial.print(F("DummyArray size=" STR(SIZE_OF_DUMMY_ARRAY)));
    Serial.print(F(", &DummyArray["));
    Serial.print(SIZE_OF_DUMMY_ARRAY - 1);
    Serial.print(F("]=0x"));
    Serial.println((uint16_t) &sDummyArray[SIZE_OF_DUMMY_ARRAY - 1], HEX);
    Serial.flush();

    pinMode(LED_PIN, OUTPUT);

    // activate LED
    delay(1000); // wait extra second after bootloader blink
    digitalWrite(LED_PIN, 1);
    delay(200);
    // deactivate LED
    digitalWrite(LED_PIN, 0);
    delay(200);

    tone(TONE_OUT_PIN, 2200, 400);
    delay(400);

#if !defined(CODE_FOR_ATTINY)
    Serial.println();
    Serial.println(F("Print RAM and Stack info"));
    printRAMAndStackInfo(&Serial);

    Serial.println();
    Serial.println(F("Dump 288 bytes of stack / end of RAM"));
    printStackMemory(288);

    Serial.println();
    Serial.println(F("Print RAM and Stack info, stack usage has increased"));
    printRAMAndStackInfo(&Serial);

    /*
     * Test calloc sizes
     */
    Serial.println();
    Serial.println(F("Test calloc sizes"));
    testCallocSizesAndPrint(&Serial);

    Serial.println();
    Serial.println(F("Print base RAM data again"));
    printBaseRAMData(&Serial);
    Serial.println();
    Serial.println(F("Print RAM and Stack info again"));
    printRAMAndStackInfo(&Serial);
#endif

    Serial.println();
    Serial.print(F("Dump current stack: "));
    printStackDump();
    Serial.print(F("\"__builtin_return_address(0)\" yields the return address of the current function \"setup()\" =0x"));
    Serial.println((uint16_t) __builtin_return_address(0), HEX);
    Serial.println();

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
    Serial.println(F("Start of loop:"));

}

void loop() {

    digitalWrite(LED_PIN, HIGH);
    delay(400);
    if (sNumberOfSleeps >= 4) {
        Serial.println(F("Wait 3 seconds to see timeout message after 2 seconds"));
        initTimeoutWithWatchdog(WDTO_2S);
        delay(3000);

        Serial.println(F("Now wait 3 seconds to wait for watchdog reset after 2 seconds"));
        Serial.println();
        strncpy(sWatchdogResetInfoString, "4. loop", WATCHDOG_INFO_STRING_SIZE - 1);
        wdt_enable(WDTO_2S); // Resets the CPU after 2 seconds. To avoid this you must call wdt_disable() or wdt_reset() before.
        delay(3000);

        Serial.println(F("We will never see this, because watchdog does a reset before!"));
    }
    strncpy(sWatchdogResetInfoString, "unknown reason", WATCHDOG_INFO_STRING_SIZE - 1);
    digitalWrite(LED_PIN, LOW);
    Serial.print(F("Sleep 2 seconds with watchdog timer sNumberOfSleeps="));
    Serial.println(sNumberOfSleeps);
    Serial.flush(); // Otherwise the USART interrupt will wake us up
    sleepWithWatchdog(WDTO_2S, true); // Sleep 2 seconds, then continue with loop
}

/*
 * This interrupt wakes up the cpu from sleep
 */
ISR(WDT_vect) {
    if (sNumberOfSleeps >= 4) {
        wdt_reset(); // restart watchdog time
        Serial.println(F("Timeout detected!"));
    }
    sNumberOfSleeps++;
}

#else
#error This source is only for AVR
#endif //defined(__AVR__)
