/*
 *  VoltAndOhmmeter.cpp
 *
 *  Realizes an volt and ohmmeter with 6mV and 2 Ohm resolution at the lower end.
 *
 *  Copyright (C) 2021  Armin Joachimsmeyer
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

/*
 *  SCHEMATIC
 *            +----o A3 open/VCC(R) | open(U)
 *            |
 *            -
 *           | |
 *           | | R3 (10 kOhm)
 *           | |
 *            -
 *            |     _____
 * Input <----+----|_____|---o input(R) | open(U)
 *            |     100 k - just to protect the pin
 *    ^       -
 *    |      | |
 *    |      | | R1 (100 kOhm)
 *    |      | |
 *    -       -
 *   | |      |
 *   | | Rx   +---o A0 VCC(R) | input(U)
 *   | |      |
 *    -       -
 *    |      | |
 *    o GND  | | R2 (22k)
 *           | |
 *            -
 *            |
 *            +---o A2 open(R) | GND(U)
 *
 *  The ratio of R1 to Rx is equal the Ratio of (1023 - x) to x
 *
 *  => The formula is: Rx/R1 = x / (1023-x)
 *      Rx = R1 * x / (1023-x)
 *
 */
#include <Arduino.h>

#include "ADCUtils.h"

#define VERSION_EXAMPLE "1.0"

/*
 * Activate the type of LCD you use
 */
//#define USE_PARALELL_LCD
//#define USE_SERIAL_LCD
/*
 * Define the size of your LCD
 */
#define USE_1602_LCD
//#define USE_2004_LCD

/*
 * Imports and definitions for LCD
 */
#if defined(USE_SERIAL_LCD)
#include <LiquidCrystal_I2C.h> // Use an up to date library version which has the init method
#endif
#if defined(USE_PARALELL_LCD)
#include <LiquidCrystal.h>
#endif

#if defined(USE_1602_LCD)
// definitions for a 1602 LCD
#define LCD_COLUMNS 16
#define LCD_ROWS 2
#endif
#if defined(USE_2004_LCD)
// definitions for a 2004 LCD
#define LCD_COLUMNS 20
#define LCD_ROWS 4
#endif

#if defined(USE_SERIAL_LCD) && defined(USE_PARALELL_LCD)
#error Cannot use parallel and serial LCD simultaneously
#endif
#if defined(USE_SERIAL_LCD) || defined(USE_PARALELL_LCD)
#define USE_LCD
#endif

#if defined(USE_SERIAL_LCD)
LiquidCrystal_I2C myLCD(0x27, LCD_COLUMNS, LCD_ROWS);  // set the LCD address to 0x27 for a 20 chars and 2 line display
#endif
#if defined(USE_PARALELL_LCD)
LiquidCrystal myLCD(4, 5, 6, 7, 8, 9);
#endif

// Include it after LCD settings, it requires the macros USE_LCD and USE_2004_LCD to be set
#include "MeasureVoltageAndResistance.cpp.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

//#define DEBUG

void setup() {
    // initialize the digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || defined(SERIAL_PORT_USBVIRTUAL)  || defined(ARDUINO_attiny3217)
    delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
#endif
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ "\r\nVersion " VERSION_EXAMPLE " from " __DATE__));

    printVoltageAndResistanceUsage();

    /*
     * LCD initialization
     */
#if defined(USE_SERIAL_LCD)
    myLCD.init();
    myLCD.clear();
    myLCD.backlight();
#endif
#if defined(USE_PARALELL_LCD)
    myLCD.begin(LCD_COLUMNS, LCD_ROWS);
#endif

#if defined(USE_SERIAL_LCD) || defined(USE_PARALELL_LCD)
    myLCD.print(F("Volt+Ohm meter"));
    myLCD.setCursor(0, 1);
    myLCD.print(F(VERSION_EXAMPLE " " __DATE__));
    delay(2000);
#endif

}

void loop() {

    MeasureVoltageAndResistance();

    delay(500);
}
