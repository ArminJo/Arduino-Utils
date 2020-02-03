# [ArduinoUtils](https://github.com/ArminJo/Arduino-Utils)
### Version 1.0.0
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Build Status](https://travis-ci.org/ArminJo/Arduino-Utils.svg?branch=master)](https://travis-ci.org/ArminJo/Arduino-Utils)

## My utility collection for Arduino
Contains utility files:
### ADCUtils
- Fast and flexible ADC conversions.
### HCSR04
- Blocking and **non-blocking** reading of HCSR04 US Sensors with timeouts and exact conversions.
### BlinkLed
- Class for blinking one ore more LED's in different fashions.
### ShowInfo
- Serial.print display of timer and other peripheral and system registers (to be extended :-)).
### AVRUtils
- Sleep and sleep with watchdog functions.
- Available Ram, Heap / Stack memory display.
### MillisUtils
- Tweaking `millis()` after long running tasks in `noIterrupt()` context like NeoPixel output, ADC buffer reading etc.
### DebugLevel
- Propagating debug levels for development. Supports level `TRACE, DEBUG, INFO, WARN and ERROR`. **Explains semantics of these levels**.
### ATtinyUtils
- `changeDigisparkClock()` to use Digispark boards with standard ATtiny code like ATTinyCore by [Spence Konde](https://github.com/SpenceKonde/ATTinyCore). 
It changes Digispark Bootloader clock settings to get the right CPU frequency and resets Digispark OCCAL tweak.
- `toneWithTimer1PWM()`.
- fuse reading function.
- `isBODSFlagExistent()` -> checking for ATtiny85 revision C.
### The very useful *digitalWriteFast.h* file from  [Watterott electronic](https://github.com/watterott/Arduino-Libs).


### Trace. 
**Tracing an Arduino program** by printing each program counter value after executing one instruction.

## Utilities for serial output for ATtinies are contained in the [ATtinySerialOut](https://github.com/ArminJo/ATtinySerialOut) library available in Arduino.

# Modifying library properties
To access the Arduino library files from a sketch, you have to first use *Sketch/Show Sketch Folder (Ctrl+K)* in the Arduino IDE.<br/>
Then navigate to the parallel `libraries` folder and select the library you want to access.<br/>
The library files itself are located in the `src` sub-directory.<br/>
If you did not yet store the example as your own sketch, then with *Ctrl+K* you are instantly in the right library folder.
## Consider to use [Sloeber](http://eclipse.baeyens.it/stable.php?OS=Windows) as IDE
If you are using Sloeber as your IDE, you can easily define global symbols at *Properties/Arduino/CompileOptions*.<br/>
![Sloeber settings](https://github.com/ArminJo/ServoEasing/blob/master/pictures/SloeberDefineSymbols.png)

# Revision History
### Version 1.0.0
- Initial Arduino library version

# Travis CI
The ArduinoUtils library examples are built on Travis CI for the following boards:

- Arduino Uno
- Arduino Leonardo
- Arduino Mega 2560
- Arduino cplayClassic

## Requests for modifications / extensions
Please write me a PM including your motivation/problem if you need a modification or an extension.

#### If you find this library useful, please give it a star.
