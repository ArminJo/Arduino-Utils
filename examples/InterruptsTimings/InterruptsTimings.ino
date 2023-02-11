/*
 *  InterruptsTimings.cpp
 *
 *  A endless loop toggling a pin, which is only interrupted by the millis() interrupt and by the servo signal generation.
 *  The millis interrupt lasts 6.0 us.
 *  The Servo interrupts are around 14 us for first and last and 20 us for the ones between (if we have more than 1 servo)
 *  1 Servo: 13,2 us (10.8 + 2,4) for rising, 14.7 us (8.2 + 6.5) for falling signal.
 *  2 Servos: 13,2 us (10.8 + 2,4) for rising Servo1, 20.2 us (8.2 + 12 -Servo1) (17.7 + 2.5 -Servo2) for falling 1 rising 2
 *    and 14.2 (9.2 + 5) for falling Servo2 signal.
 *
 *  Program size: 1620 / 47
 *  With pinModeFast and digitalWriteFast 1554 (-64) / 47
 *  With Serial.print: 2768 bytes / 226 bytes
 *  with 1 Servo: 3472 / 229 (+3)
 *  with 2 Servos: 3492 / 232 (+3)
 *
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#include <Arduino.h>
#include <Servo.h>

#include "digitalWriteFast.h"

#define VERSION_EXAMPLE "1.0"

const uint8_t TEST_OUT_PIN = LED_BUILTIN;
//Servo Servo1;
//Servo Servo2;

void setup() {
    // initialize the digital pin as an output.
    pinMode(TEST_OUT_PIN, OUTPUT);

    Serial.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_PORT_USBVIRTUAL) || defined(SERIAL_USB) /*stm32duino*/|| defined(USBCON) /*STM32_stm32*/|| defined(SERIALUSB_PID) || defined(ARDUINO_attiny3217)
    delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
#endif

    Serial.println(F("START " __FILE__ "\r\nVersion " VERSION_EXAMPLE " from " __DATE__));

//    Servo1.attach(4);
//    Servo1.write(90);
//    Servo2.attach(5);
//    Servo2.write(90);

}

void loop() { // 11 clock cycles // 687,5 ns
    while (true) { // 2 clock cycles / 125 ns
        digitalWriteFast(TEST_OUT_PIN, HIGH); // 2 clock cycles / 125 ns
        digitalWriteFast(TEST_OUT_PIN, LOW); // 2 clock cycles / 125 ns
        digitalWriteFast(TEST_OUT_PIN, HIGH);
        digitalWriteFast(TEST_OUT_PIN, LOW);
        digitalWriteFast(TEST_OUT_PIN, HIGH);
        digitalWriteFast(TEST_OUT_PIN, LOW);
        digitalWriteFast(TEST_OUT_PIN, HIGH);
        digitalWriteFast(TEST_OUT_PIN, LOW);

        digitalWrite(TEST_OUT_PIN, HIGH); // 3.8 us
        digitalWrite(TEST_OUT_PIN, LOW);  // 3.5 us
        digitalWrite(TEST_OUT_PIN, HIGH);
        digitalWrite(TEST_OUT_PIN, LOW);
        digitalWrite(TEST_OUT_PIN, HIGH);
        digitalWrite(TEST_OUT_PIN, LOW);
        digitalWrite(TEST_OUT_PIN, HIGH);
        digitalWrite(TEST_OUT_PIN, LOW);

    }
}
