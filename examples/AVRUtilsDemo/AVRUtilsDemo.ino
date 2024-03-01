/*
 *  AVRUtilsDemo.cpp
 *
 *  Demo of 2 seconds sleep with watchdog
 *  Demo of printRAMInfo() and printStackUnusedAndUsedBytes ()
 *
 *  Copyright (C) 2020-2024  Armin Joachimsmeyer
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
#if defined(CODE_FOR_ATTINY)
#include "ATtinySerialOut.hpp" // Available as Arduino library "ATtinySerialOut"
#else

//#define SIZE_OF_DUMMY_ARRAY     1800    // Stack runs into data
//#define SIZE_OF_DUMMY_ARRAY     1700    // Stack is OK, but no heap is available
#define SIZE_OF_DUMMY_ARRAY     1600    // Stack is OK, and small heap is available
uint8_t sDummyArray[SIZE_OF_DUMMY_ARRAY] __attribute__((section(".noinit"))); // Place it at end of BSS to be first overwritten by stack.
#endif

#define VERSION_EXAMPLE "1.0"

#define TONE_OUT_PIN 4

#if defined(CODE_FOR_ATTINY)
#define LED_PIN  PB1
// Pin 1 has an LED connected on my Digispark board.
#  if (LED_PIN == TX_PIN)
#error LED pin must not be equal TX pin (pin 2).
#  endif

#else
#define LED_PIN  LED_BUILTIN
#endif

uint8_t sMCUSRStored; // content of MCUSR register at startup

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
#else

    sMCUSRStored = MCUSR; // content of MCUSR register at startup
    MCUSR = 0;
    wdt_disable();

    for (int i = 0; i < SIZE_OF_DUMMY_ARRAY; ++i) {
        sDummyArray[i] = 0; // Mark array with 0 to detect overwriting by StackFreeMeasurement
    }
    initStackFreeMeasurement();

    Serial.begin(115200);

#endif // defined(CODE_FOR_ATTINY)
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ "\r\nVersion " VERSION_EXAMPLE " from  " __DATE__));
    Serial.println();

    Serial.print(F("sMCUSR=0x"));
    Serial.print(sMCUSRStored, HEX);
    Serial.print(F(" => Boot reason is"));
    printMCUSR(sMCUSRStored);

    printBODLevel();

    /*
     * Prints to avoid optimizing away the sDummyArray, which is first overwritten by stack.
     */
    Serial.print(F("sDummyArray[1000]=0x"));
    Serial.println(sDummyArray[1000], HEX);
    Serial.println(F("DummyArray size=" STR(SIZE_OF_DUMMY_ARRAY)));

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

    Serial.println();
    printRAMInfo(&Serial);
    printStackUnusedAndUsedBytes(&Serial);

    Serial.println();

    Serial.println(F("Dump stack / end of RAM"));
    printMemoryHexDump((uint8_t*) (RAMEND - 256) + 1, 256);

    Serial.println();
    printStackUsedBytes(&Serial);
    Serial.println();

//}

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

    digitalWrite(LED_PIN, HIGH);
    delay(400);
    if (sNumberOfSleeps == 4) {
        Serial.println(F("Wait 5 seconds to force watchdog reset"));
        wdt_enable(WDTO_2S); // Resets after 2.5 seconds and then every 100 ms if not wdt_disable();
        delay(5000);
    }
    digitalWrite(LED_PIN, LOW);
    Serial.print(F("Sleep 2 seconds with watchdog reset sNumberOfSleeps="));
    Serial.println(sNumberOfSleeps);
    Serial.flush(); // Otherwise the USART interrupt will wake us up
    sleepWithWatchdog(WDTO_2S, true); // Sleep 2 seconds
}
#else
#error This source is only for AVR
#endif //defined(__AVR__)
