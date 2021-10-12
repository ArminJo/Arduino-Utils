/*
 *  ADCSwitchingTest.cpp
 *
 *  Tests the switching delays between reference and channels.
 *
 *  Copyright (C) 2020-2021  Armin Joachimsmeyer
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

#include "ADCUtils.h"

#define VERSION_EXAMPLE "1.0"

#define CHANNEL_VCC_1_M_OHM     0
#define CHANNEL_VCC_100_K_OHM   1
#define CHANNEL_GND_1_M_OHM     2
#define CHANNEL_GND_100_K_OHM   3
#define CHANNEL_VOLTAGE_DIVIDER 5

#define PRECHARGE_WITH_VCC      true
#define PRECHARGE_WITH_GND      false

#define ADC_ARRAY_SIZE         32
uint16_t ADCValueArray[ADC_ARRAY_SIZE];

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

void printADCValueArray(uint16_t tAdditionalDelay = 0);
void printADCVoltageValueArray(uint16_t tAdditionalDelay = 0);
void doMeasurement(uint8_t aChannel, const __FlashStringHelper *aChannelString, bool aPrechargeWithVCC, uint16_t aDelay);

void setup() {
    // initialize the digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || defined(SERIAL_PORT_USBVIRTUAL)  || defined(ARDUINO_attiny3217)
    delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
#endif

    Serial.println(F("START " __FILE__ "\r\nVersion " VERSION_EXAMPLE " from " __DATE__));
    /*
     * Print usage
     */
    Serial.println();
    Serial.println(F("Connect ADC0 with 1 MOhm to VCC"));
    Serial.println(F("Connect ADC1 with 100 kOhm to VCC"));
    Serial.println(F("Connect ADC2 with 1 MOhm to VCC"));
    Serial.println(F("Connect ADC3 with 100 kOhm to VCC"));
    Serial.println(F("Connect ADC" STR(CHANNEL_VOLTAGE_DIVIDER) " with 1 MOhm to VCC and 100 kOhm to GND"));
    Serial.println();
}

void loop() {

    Serial.println(F("--------------------------------"));
    Serial.println(F("Each ADC conversion takes 104 us."));
    Serial.println(F("--------------------------------"));

    Serial.println();
    Serial.println(F("-------------------------"));
    Serial.println(F("Test VCC reading 2 times. Resolution is only 20 mV!"));
    Serial.print(F("VCC="));
    Serial.print(getVCCVoltageMillivolt());
    Serial.println(F("mv"));
    Serial.print(F("VCC="));
    Serial.print(getVCCVoltageMillivolt());
    Serial.println(F("mv"));

    uint8_t i;

    /*
     * Test channel switching from GND to VCC
     */
    Serial.println();
    Serial.println(F("-------------------------"));
    Serial.println(F("Test reference switching."));
    /*
     * Test reference switching from DEFAULT to INTERNAL
     * Result: 20 samples / 7600 microseconds until value is stable
     */
// set sample capacitor to VCC
    while (readADCChannelWithReference(CHANNEL_VCC_100_K_OHM, DEFAULT) != 1023) {
        ;
    }
    delay(10);
    for (uint8_t i = 0; i < ADC_ARRAY_SIZE; ++i) {
        ADCValueArray[i] = readADCChannelWithReference(CHANNEL_VOLTAGE_DIVIDER, INTERNAL);
        delayMicroseconds(296);
    }
    Serial.println();
    Serial.println(
            F(
                    "Read VCC at 1M / 100k voltage divider (by 11) at pin A" STR(CHANNEL_VOLTAGE_DIVIDER) " after switching reference from DEFAULT/VCC to INTERNAL/1.1V in a 400 us raster."));
    printADCVoltageValueArray(296);

    /*
     * Test reference switching from INTERNAL to DEFAULT
     * Result: unbelievable? 0 samples until value is stable
     */
    while (readADCChannelWithReference(ADC_GND_CHANNEL_MUX, INTERNAL) != 0) {
        // wait until sample capacitor is at 0 volt
    }
    delay(10);
    for (i = 0; i < ADC_ARRAY_SIZE; ++i) {
        ADCValueArray[i] = readADCChannelWithReference(CHANNEL_VCC_100_K_OHM, DEFAULT);
    }
    Serial.println();
    Serial.println(
            F(
                    "Read 100 kOhm VCC input at pin A" STR(CHANNEL_VCC_100_K_OHM) " after switching reference from INTERNAL/1.1V to DEFAULT/VCC."));
    printADCValueArray();

    /*
     * Test reference switching from INTERNAL to DEFAULT
     * Result: unbelievable? 0 samples until value is stable
     */
    for (uint8_t i = 0; i < ADC_ARRAY_SIZE; ++i) {
        ADCValueArray[i] = readADCChannelWithReference(CHANNEL_VOLTAGE_DIVIDER, INTERNAL);
        delayMicroseconds(296);
    }
    delay(10);
    for (i = 0; i < ADC_ARRAY_SIZE; ++i) {
        ADCValueArray[i] = readADCChannelWithReference(CHANNEL_GND_100_K_OHM, DEFAULT);
    }
    Serial.println();
    Serial.println(
            F(
                    "Read 100 kOhm GND input at pin A" STR(CHANNEL_GND_100_K_OHM) " after switching reference from INTERNAL/1.1V to DEFAULT/VCC."));
    printADCValueArray();

    /*
     * Test channel switching from GND to VCC
     */
    Serial.println();
    Serial.println(F("--------------------------------------------------------------------------"));
    Serial.println(F("Test channel switching from GND to VCC with different resistors and delays."));

    /*
     * 100kOhm VCC input -> 0 to 1 sample until value is stable since the first sample reads always 1022
     */
    doMeasurement(CHANNEL_VCC_100_K_OHM, F("100 kOhm VCC input at pin A" STR(CHANNEL_VCC_100_K_OHM)), PRECHARGE_WITH_GND, 0);

    /*
     * 1MOhm VCC input -> 3 samples /312 microseconds until value is stable
     */
    doMeasurement(CHANNEL_VCC_1_M_OHM, F("1 MOhm VCC input at pin A" STR(CHANNEL_VCC_1_M_OHM)), PRECHARGE_WITH_GND, 120);

    /*
     * 1MOhm VCC input -> 3 samples /312 microseconds until value is stable
     */
    doMeasurement(CHANNEL_VCC_1_M_OHM, F("1 MOhm VCC input at pin A" STR(CHANNEL_VCC_1_M_OHM)), PRECHARGE_WITH_GND, 100);

    /*
     * 1MOhm VCC input -> 2 samples / 208 microseconds until value is stable
     */
    doMeasurement(CHANNEL_VCC_1_M_OHM, F("1 MOhm VCC input at pin A" STR(CHANNEL_VCC_1_M_OHM)), PRECHARGE_WITH_GND, 50);

    /*
     * 1MOhm VCC input-> 3 - 4 samples / 312 microseconds until value is stable
     */
    doMeasurement(CHANNEL_VCC_1_M_OHM, F("1 MOhm VCC input at pin A" STR(CHANNEL_VCC_1_M_OHM)), PRECHARGE_WITH_GND, 0);

    /*
     * Read 1.1 volt reference input -> 0 samples until value 237 is stable
     */
    doMeasurement(ADC_1_1_VOLT_CHANNEL_MUX, F("1.1 volt reference"), PRECHARGE_WITH_GND, 50);

    /*
     * Read 1.1 volt reference input -> 2 samples until value 237 is stable
     */
    doMeasurement(ADC_1_1_VOLT_CHANNEL_MUX, F("1.1 volt reference"), PRECHARGE_WITH_GND, 0);

    /*
     * Read 1.1 volt reference input and reset ADC first which introduces 13.5 S&H periods for the first measurement
     * This is not as good as simply introducing a delay!
     * -> 3 samples until value 237 is stable
     */
    while (readADCChannelWithReference(ADC_GND_CHANNEL_MUX, DEFAULT) != 0) {
        // wait until sample capacitor is at 0 volt
    }
    delay(4);
    ADMUX = ADC_1_1_VOLT_CHANNEL_MUX | (DEFAULT << SHIFT_VALUE_FOR_REFERENCE);
// Turn ADC off to enable 13.5 clock periods S&H time for first conversion
    ADCSRA = ADC_PRESCALE;
    for (i = 0; i < ADC_ARRAY_SIZE; ++i) {
        ADCValueArray[i] = readADCChannelWithReference(ADC_1_1_VOLT_CHANNEL_MUX, DEFAULT);
    }
    Serial.println();
    Serial.println(F("Read 1.1 volt reference after sample capacitor was loaded with GND + ADC disable + 13.5 S&H periods."));
    printADCValueArray(); // raw = 2803

    /*
     * Read internal temperature ->  samples
     */
    doMeasurement(ADC_TEMPERATURE_CHANNEL_MUX, F("internal temperature channel"), PRECHARGE_WITH_GND, 0);

    /*
     * Test channel switching from VCC to GND
     */
    Serial.println();
    Serial.println(F("--------------------------------------------------------------------------"));
    Serial.println(F("Test channel switching from VCC to GND with different resistors and delays."));

    /*
     * Read 100kOhm GND input -> unbelievable 0 microseconds until value is stable
     */
    doMeasurement(CHANNEL_GND_100_K_OHM, F("100 kOhm GND input at pin A" STR(CHANNEL_GND_100_K_OHM)), PRECHARGE_WITH_VCC, 0);

    /*
     * Read 1MOhm GND input -> 3 samples / 312 microseconds until value is stable
     */
    doMeasurement(CHANNEL_GND_1_M_OHM, F("1 MOhm GND input at pin A" STR(CHANNEL_GND_1_M_OHM)), PRECHARGE_WITH_VCC, 100);

    /*
     * Read 1MOhm GND input -> 3 samples / 312 microseconds until value is stable
     */
    doMeasurement(CHANNEL_GND_1_M_OHM, F("1 MOhm GND input at pin A" STR(CHANNEL_GND_1_M_OHM)), PRECHARGE_WITH_VCC, 0);

    /*
     * Read 1.1 volt reference input -> 7 samples /  microseconds until value 237 is stable
     */
    doMeasurement(ADC_1_1_VOLT_CHANNEL_MUX, F("1.1 volt reference"), PRECHARGE_WITH_VCC, 200);

    /*
     * Read 1.1 volt reference input -> 3 samples until value 237 is stable
     */
    doMeasurement(ADC_1_1_VOLT_CHANNEL_MUX, F("1.1 volt reference"), PRECHARGE_WITH_VCC, 150);

    /*
     * Read 1.1 volt reference input -> 7 samples /  microseconds until value 237 is stable
     */
    doMeasurement(ADC_1_1_VOLT_CHANNEL_MUX, F("1.1 volt reference"), PRECHARGE_WITH_VCC, 0);

    /*
     * Read internal temperature ->  samples
     */
    doMeasurement(ADC_TEMPERATURE_CHANNEL_MUX, F("internal temperature channel"), PRECHARGE_WITH_VCC, 0);

    /*
     * 1MOhm VCC input -> 2 to 3 samples until value is stable
     */
//    // set sample capacitor to VCC
//    while (readADCChannelWithReference(CHANNEL_VCC_100_K_OHM, DEFAULT) != 1023) {
//        ;
//    }
//    delay(4);
//    ADMUX = CHANNEL_VCC_1_M_OHM | (DEFAULT << SHIFT_VALUE_FOR_REFERENCE);
//    // Turn ADC off to enable 13.5 clock periods S&H time for first conversion
//    ADCSRA = ADC_PRESCALE;
//    for (i = 0; i < ADC_ARRAY_SIZE; ++i) {
//        ADCValueArray[i] = readADCChannelWithReference(CHANNEL_GND_1_M_OHM, DEFAULT);
//    }
//    Serial.println();
//    Serial.println(
//            F(
//                    "Read 1MOhm GND input at pin A" STR(CHANNEL_GND_1_M_OHM) " after sample capacitor was loaded with VCC + ADC disable (1 * 13.5 S&H periods)."));
//    printADCValueArray();
    /*
     * Read GND input -> the expected 0 microseconds.
     */
    doMeasurement(ADC_GND_CHANNEL_MUX, F("internal GND channel"), true, 0);

    Serial.println();
    Serial.println();
    delay(5000);
}

void printADCValueArray(uint16_t tAdditionalDelay) {
    uint8_t i;
    int8_t tIndexOfFirstStable = -1;
    // First search for first stable measurement
    for (i = 0; i < ADC_ARRAY_SIZE; ++i) {
        // Print all or until the last 3 are equal
        if (i >= 2 && ADCValueArray[i] == ADCValueArray[i - 1] && ADCValueArray[i - 1] == ADCValueArray[i - 2]) {
            tIndexOfFirstStable = i - 2;
            break;
        }
    }
    // Print summary
    if (tIndexOfFirstStable >= 0) {
        Serial.print(F(" Samples="));
        Serial.print(tIndexOfFirstStable);
        Serial.print(F(" Time="));
        Serial.print(104 * tIndexOfFirstStable);
        Serial.print(F(" us "));
    }
    // Print values
    Serial.print(F(" ADC="));
    for (i = 0; i < ADC_ARRAY_SIZE; ++i) {
        Serial.print(ADCValueArray[i]);
        // Print all or until the last 3 are equal
        if (i >= 2 && ADCValueArray[i] == ADCValueArray[i - 1] && ADCValueArray[i - 1] == ADCValueArray[i - 2]) {
            break;
        }
        Serial.print(' ');
    }
    Serial.println();

    Serial.flush();
}

void printADCVoltageValueArray(uint16_t tAdditionalDelay) {
    uint8_t i;
    int8_t tIndexOfFirstStable = -1;
    for (i = 0; i < ADC_ARRAY_SIZE; ++i) {
        // Print all or until the last 3 are equal
        if (i >= 2 && ADCValueArray[i] == ADCValueArray[i - 1] && ADCValueArray[i - 1] == ADCValueArray[i - 2]) {
            tIndexOfFirstStable = i - 2;
            break;
        }
    }

    if (tIndexOfFirstStable >= 0) {
        Serial.print(F(" Samples="));
        Serial.print(tIndexOfFirstStable);
        Serial.print(F(" Time="));
        Serial.print((104 + tAdditionalDelay) * tIndexOfFirstStable);
        Serial.print(F(" us "));
    }

    Serial.print(F(" VCC[mV]="));
    for (i = 0; i < ADC_ARRAY_SIZE; ++i) {
        // Print all or until the last 3 are equal
        Serial.print((ADCValueArray[i] * (12100L)) / 1023);
        if (i >= 2 && ADCValueArray[i] == ADCValueArray[i - 1] && ADCValueArray[i - 1] == ADCValueArray[i - 2]) {
            tIndexOfFirstStable = i - 2;
            break;
        }
        Serial.print(' ');
    }
    Serial.print("=");
    Serial.print(ADCValueArray[i - 1]);
    Serial.println("(raw)");

    Serial.flush();
}

void doMeasurement(uint8_t aChannel, const __FlashStringHelper *aChannelString, bool aPrechargeWithVCC, uint16_t aDelay) {
    if (aPrechargeWithVCC) {
        while (readADCChannelWithReference(CHANNEL_VCC_100_K_OHM, DEFAULT) != 1023) {
            ;
        }
    } else {
        while (readADCChannelWithReference(ADC_GND_CHANNEL_MUX, DEFAULT) != 0) {
            // wait until sample capacitor is at 0 volt
        }
    }

    delay(4);
    ADMUX = aChannel | (DEFAULT << SHIFT_VALUE_FOR_REFERENCE);
    if (aDelay > 0) {
        delayMicroseconds(aDelay);
    }
    for (uint8_t i = 0; i < ADC_ARRAY_SIZE; ++i) {
        ADCValueArray[i] = readADCChannelWithReference(aChannel, DEFAULT);
    }

    Serial.println();
    Serial.print(F("Read "));
    Serial.print(aChannelString);
    Serial.print(F(" after sample capacitor was loaded with "));
    if (aPrechargeWithVCC) {
        Serial.print(F("VCC"));
    } else {
        Serial.print(F("GND"));
    }
    Serial.print(F(" + "));
    if (aDelay > 0) {
        Serial.print(aDelay);
        Serial.print(F(" us "));
    } else {
        Serial.print(F(" no "));
    }
    Serial.println(F(" delay."));
    printADCValueArray(); // raw = 2803
}

