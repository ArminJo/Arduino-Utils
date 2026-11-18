/*
 *  50Hz.ino
 *  Example only for ATtinies: Output 50 Hz at Pin 2/3
 *
 *  Copyright (C) 2015-2026  Armin Joachimsmeyer
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/gpl-3.0.html>.
 *
 */

#include <Arduino.h>

#include "ATtinySerialOut.hpp" // Available as Arduino library "ATtinySerialOut"
#include "ATtinyUtils.h" // for toneWithTimer1PWM()
#include <avr/sleep.h>

#define VERSION_EXAMPLE "1.0"
#define INFO

void setup() {
#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny87__) || defined(__AVR_ATtiny167__)
    initTXPin();

#  if defined(INFO)
    writeString(F("START " __FILE__ "\nVersion " VERSION_EXAMPLE " from " __DATE__ "\n"));
#  endif

    toneWithTimer1PWM(50, true);
#endif

    // let the cpu and flash sleep
    sleep_enable()
    ;
    set_sleep_mode(SLEEP_MODE_IDLE);
}

void loop() {
    interrupts(); // Enable interrupts
    sleep_cpu()
    ;
}
