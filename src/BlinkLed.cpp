/*
 * BlinkLed.cpp
 *
 *  This library let multiple LEDs, connected to any Arduino output, blink non blocking and independently.
 *  Therefore you must call update continuously in loop()
 *  Parameter blink count: 0 / BLINK_LED_COUNT_FOREVER means forever
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
 */
#include <Arduino.h>

#include "BlinkLed.h"

/*
 * The simple blocking variant with 50% duty cycle as static function with parameter aLedPin
 */
void blinkLEDBlocking(uint8_t aLedPin, uint8_t aBlinkCount, uint16_t aDelayMillis) {
    for (uint8_t i = 0; i < aBlinkCount; ++i) {
        digitalWrite(aLedPin, HIGH);
        delay(aDelayMillis);
        digitalWrite(aLedPin, LOW);
        delay(aDelayMillis);
    }
}

/*
 * set to 50% duty cycle
 */
void BlinkLed::blinkBlocking(unsigned int aBlinkCount, unsigned int aPeriodMillis) {
    if (aBlinkCount == 0) {
        return;
    }
    start(aBlinkCount, aPeriodMillis);
    while (enabled) {
        delay(1);
        update();
    }
}

BlinkLed::BlinkLed(uint8_t aLedPin) {
    init(aLedPin, false);
    setOnOffTime(1000, 1000);
}

BlinkLed::BlinkLed(uint8_t aLedPin, bool aInitState, unsigned int aOnTimeMillis, unsigned int aOffTimeMillis) {
    init(aLedPin, aInitState);
    setOnOffTime(aOnTimeMillis, aOffTimeMillis);
}

/*
 * Is called in constructor, but pinMode() etc. works on AVR here :-)
 */
void BlinkLed::init(uint8_t aLedPin, bool aInitState) {
    pin = aLedPin;
    pinMode(pin, OUTPUT);
    state = aInitState;
    digitalWrite(pin, state);
}

void BlinkLed::setOnOffTime(unsigned int aOnTimeMillis, unsigned int aOffTimeMillis) {
    onDelayMillis = aOnTimeMillis;
    offDelayMillis = aOffTimeMillis;
}

// must be called continuously in loop()
void BlinkLed::update() {

    if (!enabled) {
        return;
    }

    if (state) {
        // check for on time gone to change state
        if ((millis() - lastUpdateMillis >= onDelayMillis)) {
            toggle();
            // check for forever blinking
            if (numberOfBlinks > 0) {
                numberOfBlinks--;
                if (numberOfBlinks == 0) {
                    // stop blinking
                    enabled = false;
                }
            }
        }
    } else if ((millis() - lastUpdateMillis >= offDelayMillis)) {
        toggle();
    }
}

/*
 * @param aBlinkCount - 0 means forever
 */
void BlinkLed::start(unsigned int aBlinkCount, unsigned int aOnTimeMillis, unsigned int aOffTimeMillis) {
    onDelayMillis = aOnTimeMillis;
    offDelayMillis = aOffTimeMillis;
    start(aBlinkCount);
}

/*
 * set to around 50% duty cycle
 */
void BlinkLed::start(unsigned int aBlinkCount, unsigned int aPeriodMillis) {
    onDelayMillis = offDelayMillis = aPeriodMillis / 2;
    start(aBlinkCount);
}

void BlinkLed::start(unsigned int aBlinkCount) {
    numberOfBlinks = aBlinkCount;
    start();
}

// Force ON and enable blink
void BlinkLed::start() {
    digitalWrite(pin, HIGH);
    state = true;
    enabled = true;
    lastUpdateMillis = millis();
}

/*
 * No count specified here, so set to BLINK_LED_COUNT_FOREVER
 */
void BlinkLed::startForeverWithOnTime(unsigned int aOnTimeMillis) {
    onDelayMillis = aOnTimeMillis;
    start(BLINK_LED_COUNT_FOREVER);
}

void BlinkLed::startForeverWithOffTime(unsigned int aOffTimeMillis) {
    offDelayMillis = aOffTimeMillis;
    start(BLINK_LED_COUNT_FOREVER);
}

void BlinkLed::startForeverWithOnOffTime(unsigned int aOnTimeMillis, unsigned int aOffTimeMillis) {
    onDelayMillis = aOnTimeMillis;
    offDelayMillis = aOffTimeMillis;
    start(BLINK_LED_COUNT_FOREVER);
}

/*
 * set to 50% duty cycle
 */
void BlinkLed::startForeverWithPeriod(unsigned int aPeriodMillis) {
    start(BLINK_LED_COUNT_FOREVER, aPeriodMillis);
}

/*
 * set to 50% duty cycle
 * !!!Parameter aFrequency is not checked for 0.0!!!
 */
void BlinkLed::startForeverWithFrequency(float aFrequency) {
    offDelayMillis = onDelayMillis = 500.0 / aFrequency;
    start(BLINK_LED_COUNT_FOREVER);
}

// Toggle state and set new timestamp
void BlinkLed::toggle() {
    state = !state;
    digitalWrite(pin, state);
    lastUpdateMillis = millis();
}

// Force ON and disable blink
void BlinkLed::on() {
    digitalWrite(pin, HIGH);
    state = true;
    enabled = false;
}

// Force off and disable blink
void BlinkLed::stop() {
    digitalWrite(pin, LOW);
    state = false;
    enabled = false;
}

// Force off and disable blink - the same as stop
void BlinkLed::off() {
    stop();
}

/*
 * Might be useful if using negative logic i.e. LED is connected to VCC.
 * You can also access "enabled" directly, but this is not the recommended way to use a class ;-)
 */
void BlinkLed::setEnabled(bool aIsEnabled) {
    enabled = aIsEnabled;
}
