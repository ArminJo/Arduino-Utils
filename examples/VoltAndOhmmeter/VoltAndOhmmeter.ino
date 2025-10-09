/*
 *  VoltAndOhmmeter.cpp
 *
 *  Realizes an volt and ohmmeter with 6mV and 2 ohm resolution at the lower end.
 *  First voltage is measured.
 *  If voltage is zero, then resistance to ground is measured using 5 volt (VCC) and 10 kOhm or 100 kOhm supply.
 *
 *  Copyright (C) 2021-2025  Armin Joachimsmeyer
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#include <Arduino.h>

#define VERSION_EXAMPLE "2.0"

//#define NO_PRINT_OF_RESISTOR_MEASURMENT_VOLTAGE

/*
 * Activate the type of LCD you use
 */
//#define USE_PARALLEL_LCD
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
#include "LiquidCrystal_I2C.hpp" // Here we use an enhanced version, which supports SoftI2CMaster
#endif
#if defined(USE_PARALLEL_LCD)
#include "LiquidCrystal.h"
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

#if defined(USE_SERIAL_LCD) && defined(USE_PARALLEL_LCD)
#error Cannot use parallel and serial LCD simultaneously
#endif
#if defined(USE_SERIAL_LCD) || defined(USE_PARALLEL_LCD)
#define USE_LCD
#endif

#if defined(USE_SERIAL_LCD)
LiquidCrystal_I2C myLCD(0x27, LCD_COLUMNS, LCD_ROWS);  // set the LCD address to 0x27 for a 20 chars and 2 line display
#endif
#if defined(USE_PARALLEL_LCD)
LiquidCrystal myLCD(4, 5, 6, 7, 8, 9);
#endif

// Include it after LCD settings, it requires the macros below and the macros USE_LCD and USE_2004_LCD to be set
#define PRINT_OF_RESISTOR_MEASURMENT_VOLTAGE
#define PRINT_OF_VCC
#include "MeasureVoltageAndResistance.hpp"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

//#define DEBUG

void setup() {
    // initialize the digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_PORT_USBVIRTUAL) || defined(SERIAL_USB) /*stm32duino*/|| defined(USBCON) /*STM32_stm32*/ \
    || defined(SERIALUSB_PID)  || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_attiny3217)
    delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
#endif
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ "\r\nVersion " VERSION_EXAMPLE " from " __DATE__));

    /*
     * LCD initialization, which may reduce VCC
     */
#if defined(USE_SERIAL_LCD)
    myLCD.init();
    myLCD.clear();
    myLCD.backlight();
#endif
#if defined(USE_PARALLEL_LCD)
    myLCD.begin(LCD_COLUMNS, LCD_ROWS);
#endif

    printVoltageAndResistanceUsage();

#if defined(USE_SERIAL_LCD) || defined(USE_PARALLEL_LCD)
    myLCD.print(F("Volt+Ohm meter"));
    myLCD.setCursor(0, 1);
    myLCD.print(F(VERSION_EXAMPLE " " __DATE__));
    delay(2000);
    myLCD.clear();
#endif

}

void loop() {

    MeasureVoltageAndResistance();

    delay(500);
}
