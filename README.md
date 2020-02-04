# [ArduinoUtils](https://github.com/ArminJo/Arduino-Utils)
### Version 1.0.0
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Build Status](https://travis-ci.org/ArminJo/Arduino-Utils.svg?branch=master)](https://travis-ci.org/ArminJo/Arduino-Utils)

## My utility collection for Arduino

### ADCUtils.cpp
- Fast and flexible ADC conversions.
### HCSR04
- Blocking and **non-blocking** reading of HCSR04 US Sensors with timeouts and exact conversions.
- Supports also **1 Pin mode** available with the HY-SRF05 or Parallax PING modules. 
#### You can modify the HCSR04 modules to 1 Pin mode by:
1 Old module with 3 16 pin chips: Connect Trigger and Echo direct or use a resistor < 4.7 kOhm. 
If you remove both 10 kOhm pullup resistor you can use a connecting resistor < 47 kOhm, but I suggest to use 10 kOhm which is more reliable.
2 Old module with 3 16 pin chips but with no pullup resistors near the connector row: Connect Trigger and Echo with a resistor > 200 Ohm. Use 10 kOhm.
3 New module with 1 16 pin and 2 8 pin chips: Connect Trigger and Echo by a resistor > 200 Ohm and < 22 kOhm.
4 All modules: Connect Trigger and Echo by a resistor of 4.7 kOhm.

### BlinkLed.cpp
- Class for blinking one ore more LED's in different fashions.
### ShowInfo
- Serial.print display of timer and other peripheral and system registers (to be extended :-)).
### AVRUtils.cpp
- Sleep and sleep with watchdog functions.
- Available Ram, Heap / Stack memory display.
### MillisUtils.cpp
- Tweaking `millis()` after long running tasks in `noIterrupt()` context like NeoPixel output, ADC buffer reading etc.
### DebugLevel.h
- Propagating debug levels for development. Supports level `TRACE, DEBUG, INFO, WARN and ERROR`. **Explains semantics of these levels**.
### ATtinyUtils.cpp
- `changeDigisparkClock()` to use Digispark boards with standard ATtiny code like ATTinyCore by [Spence Konde](https://github.com/SpenceKonde/ATTinyCore). 
It changes Digispark Bootloader clock settings to get the right CPU frequency and resets Digispark OCCAL tweak.
- `toneWithTimer1PWM()`.
- `isBODSFlagExistent()` -> checking for ATtiny85 revision C.
- fuse reading function.

### Trace.cpp.h
**Tracing an Arduino program** by printing each program counter value after executing one instruction.

### [ATtinySerialOut.cpp](https://github.com/ArminJo/ATtinySerialOut/src)
Minimal bit-bang send serial
- 115200 baud for 1/8/16 MHz ATtiny clock.
The utility for serial output for ATtinies is contained in the [ATtinySerialOut](https://github.com/ArminJo/ATtinySerialOut)
library available as an Arduino library.

### [EasyButtonAtInt01.cpp.h](https://github.com/ArminJo/EasyButtonAtInt01/src)
- Arduino library for handling push buttons just connected between ground and INT0 and / or INT1 pin.
- No external pullup, **no polling needed**.
The utility for easy button handling for ATmegas or ATtinies is contained in the [EasyButtonAtInt01](https://github.com/ArminJo/EasyButtonAtInt01)
library available as an Arduino library.

### The very useful *digitalWriteFast.h* file from  [Watterott electronic](https://github.com/watterott/Arduino-Libs).

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
- Digispark (using ATTiny85 @1MHz)
- Generic ATTiny85 @1MHz

## Requests for modifications / extensions
Please write me a PM including your motivation/problem if you need a modification or an extension.

#### If you find this library useful, please give it a star.