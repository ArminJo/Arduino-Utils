# [ArduinoUtils](https://github.com/ArminJo/Arduino-Utils)

### [Version 1.0.0](https://github.com/ArminJo/Arduino-Utils/archive/master.zip)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Build Status](https://github.com/ArminJo/Arduino-Utils/workflows/LibraryBuild/badge.svg)](https://github.com/ArminJo/Arduino-Utils/actions)

## My utility collection for Arduino

### SimpleEMAFilters.cpp
- A fixed set of 10 **ultrafast EMA (Exponential Moving Average) filters** and display routines for Arduino Plotter.
 8 filters requires only 1 to 2 microseconds, in contrast to the floating point implemenation used for comparison, which takes 24 to 34 µs.<br/>
 An alternative fixed point implementation is included which requires 4 µs.
 
All filters can be applied to your signal and the results can then easily be displayed in the Arduino Plotter.

### ADCUtils.cpp
Fast and flexible ADC conversions. Intelligent **handling of reference switching**.
- Functions for easy **oversampling**.
- Function for easy getting the maximum value of measurements.
- Functions for getting **temperature and VCC voltage**.  For VCC, resolution is only 20 millivolt!


### HCSR04
- Blocking and **non-blocking** reading of HCSR04 US Sensors with timeouts and exact conversions.
- Supports also **1 Pin mode** available with the HY-SRF05 or Parallax PING modules.

#### You can modify the HCSR04 modules to 1 Pin mode by:
1 Old module with 3 16 pin chips: Connect Trigger and Echo direct or use a resistor < 4.7 kOhm. 
If you remove both 10 kOhm pullup resistor you can use a connecting resistor < 47 kOhm, but I suggest to use 10 kOhm which is more reliable.
2 Old module with 3 16 pin chips but with no pullup resistors near the connector row: Connect Trigger and Echo with a resistor > 200 Ohm. Use 10 kOhm.
3 New module with 1 16 pin and 2 8 pin chips: Connect Trigger and Echo by a resistor > 200 Ohm and < 22 kOhm.
4 All modules: Connect Trigger and Echo by a resistor of 4.7 kOhm.

### MeasureVoltageAndResistance.hpp
Measures voltage and resistance with 1 mV and 2 Ohm resolution at the lower end.<br/>
First voltage is measured. If voltage is zero, then resistance to ground is measured using 5 volt (VCC) and 10 kOhm or 100 kOhm supply.

### Fritzing board
![Fritzing board](extras/VoltAndOhmmeter_Steckplatine.png)
### Fritzing schematics
![Fritzing schematics](extras/VoltAndOhmmeter_Schaltplan.png)

### BlinkLed.cpp
- Class for blinking one ore more LED's in different fashions.

### ShowInfo
- Serial.print display of timer and other peripheral and system registers (to be extended :-)).

### AVRUtils.cpp
- Sleep and sleep with watchdog functions.
- Available Ram, Heap / Stack memory display.

### MillisUtils.cpp
Unifies millis() timer handling for Digispark, AttinyCore and Arduino cores.
- Start, stop and modify milliseconds timer and value.
- Functions to compensat `millis()` after long running tasks in `noIterrupt()` context like NeoPixel output, ADC buffer reading etc.
- Blocking delayMilliseconds() function for use in noInterrupts context like ISR.

### DebugLevel.h
- Propagating debug levels for development. Supports level `TRACE, DEBUG, INFO, WARN and ERROR`. 
- **Includes an explanation of semantics of these levels**.

### ATtinyUtils.cpp
- `toneWithTimer1PWM()`.
- `isBODSFlagExistent()` -> checking for ATtiny85 revision C.
- fuse reading function.
- `changeDigisparkClock()` to use Digispark boards with no Digispark core like e.g. ATTinyCore by [Spence Konde](https://github.com/SpenceKonde/ATTinyCore). 
It changes Digispark Bootloader clock settings to get the right CPU frequency and resets Digispark OCCAL tweak. Consider to use new [optimized Digispark core](https://github.com/ArminJo/DigistumpArduino) instead. 

### AvrTracing.hpp
**Tracing an Arduino program** by printing each program counter value after executing one instruction.

## Utilities available as separate Arduino library

### [ATtinySerialOut.cpp](https://github.com/ArminJo/ATtinySerialOut/src)
Minimal bit-bang send serial
- 115200 baud for 1/8/16 MHz ATtiny clock.
The utility for serial output for ATtinies is contained in the [ATtinySerialOut](https://github.com/ArminJo/ATtinySerialOut)
library available as an Arduino library.

### [EasyButtonAtInt01.hpp](https://github.com/ArminJo/EasyButtonAtInt01/src)
- Arduino library for handling push buttons just connected between ground and INT0 and / or INT1 pin.
- No external pullup, **no polling needed**.
The utility for easy button handling for ATmegas or ATtinies is contained in the [EasyButtonAtInt01](https://github.com/ArminJo/EasyButtonAtInt01)
library available as an Arduino library.

### The very useful *digitalWriteFast.h* file from  [Watterott electronic](https://github.com/watterott/Arduino-Libs).

### Modifying compile options with Arduino IDE
First, use *Sketch > Show Sketch Folder (Ctrl+K)*.<br/>
If you did not yet stored the example as your own sketch, then you are instantly in the right library folder.<br/>
Otherwise you have to navigate to the parallel `libraries` folder and select the library you want to access.<br/>
In both cases the library files itself are located in the `src` directory.<br/>

### Modifying compile options with Sloeber IDE
If you are using Sloeber as your IDE, you can easily define global symbols with *Properties > Arduino > CompileOptions*.<br/>
![Sloeber settings](https://github.com/ArminJo/ServoEasing/blob/master/pictures/SloeberDefineSymbols.png)

# Revision History

### Version 1.0.0 - work in progress
- Added unions.
- Changed distance return value for overflow to 0.

# Travis CI
The ArduinoUtils library examples are built on Travis CI for the following boards:

- Arduino Uno
- Arduino Leonardo
- Digispark (using ATTiny85 @1MHz)
- Generic ATTiny85 @1MHz

## Requests for modifications / extensions
Please write me a PM including your motivation/problem if you need a modification or an extension.

#### If you find this library useful, please give it a star.
