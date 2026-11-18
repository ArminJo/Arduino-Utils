/*
 * BlinkLed.h
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

#include <inttypes.h>

#ifndef _BLINK_LED_H
#define _BLINK_LED_H

// The simple blocking variant
void blinkLEDBlocking(uint8_t aLedPin, uint8_t aBlinkCount, uint16_t aDelayMillis);

#define BLINK_LED_COUNT_FOREVER (0)
class BlinkLed {
public:
    // constructors and initialization
    BlinkLed(uint8_t aLedPin);
    BlinkLed(uint8_t aLedPin, bool aInitState, unsigned int aOnTimeMillis, unsigned int aOffTime);
    void init(uint8_t aLedPin, bool aInitState);
    void setOnOffTime(unsigned int aOnTimeMillis, unsigned int aOffTime);

    void blinkBlocking(unsigned int aBlinkCount, unsigned int aPeriodMillis);

    void update(); // must be called continuously in loop()

    // Blinking ends after aBlinkCount
    void start(unsigned int aBlinkCount, unsigned int aOnTimeMillis, unsigned int aOffTime); // start
    void start(unsigned int aBlinkCount, unsigned int aPeriodMillis); // start
    void start(unsigned int aBlinkCount); // use old values for on and off time
    void start();

    // Blinking goes forever until off()
    void startForeverWithOnOffTime(unsigned int aOnTimeMillis, unsigned int aOffTime);
    void startForeverWithPeriod(unsigned int aPeriodMillis);
    void startForeverWithOnTime(unsigned int aOnTimeMillis);
    void startForeverWithOffTime(unsigned int aOffTime);
    void startForeverWithFrequency(float aFrequency); // set to 50% duty cycle

    void on(); // force on but do not blink
    void stop();
    void off(); // the same as stop()

    void toggle();
    void setEnabled(bool aIsEnabled);

    uint8_t pin; // Pin number connected to LED anode
    // uint8_t to enable more states like PinStatus of TinyCore
    uint8_t state;
    unsigned int numberOfBlinks = BLINK_LED_COUNT_FOREVER; // 0 means forever
    unsigned int onDelayMillis = 1000; // On time in milliseconds
    unsigned int offDelayMillis = 1000; // Off time in milliseconds
    unsigned long lastUpdateMillis = 0; // The last time (millis) LED was updated
    bool enabled = false; // LED enabled/disabled state
};

#endif // _BLINK_LED_H
