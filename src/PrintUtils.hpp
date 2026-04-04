/*
 *  PrintUtils.hpp
 *
 *  Copyright (C) 2026  Armin Joachimsmeyer
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
 */

#ifndef _PRINT_UTILS_HPP
#define _PRINT_UTILS_HPP

//#define LOCAL_DEBUG // This enables debug output only for this file - only for development

// Helper macro for getting a macro definition as string
#if !defined(STR_HELPER) && !defined(STR)
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#endif

template <typename TypeToPrint>
class PrintIfChanged {
public:
    const char *PGMTextPtr;
    TypeToPrint LastValuePrinted;

    // Constructor with PGM text as parameter. const __FlashStringHelper* does not work for constructors
    PrintIfChanged(const char *aPGMText) { // @suppress("Class members should be properly initialized")
        PGMTextPtr = aPGMText;
    }

    /*
     * @return true if value has changed and printing was done
     */
    bool printWithLeadingText(TypeToPrint aValueToPrint) {
        if (LastValuePrinted != aValueToPrint) {
            LastValuePrinted = aValueToPrint;
            Serial.print(reinterpret_cast<const __FlashStringHelper*>(PGMTextPtr));
            Serial.println(aValueToPrint);
            return true;
        }
        return false;
    }

    bool printWithTrailingText(TypeToPrint aValueToPrint) {
        if (LastValuePrinted != aValueToPrint) {
            LastValuePrinted = aValueToPrint;
            Serial.print(aValueToPrint);
            Serial.println(reinterpret_cast<const __FlashStringHelper*>(PGMTextPtr));
            return true;
        }
        return false;
    }
};

/*
 * Usage
 * const char BrightnessPGM[] PROGMEM = "Brightness=";
 * PrintIfChanged sBrightnessPrint(BrightnessPGM);
 */

#if defined(LOCAL_DEBUG)
#undef LOCAL_DEBUG
#endif
#endif // _PRINT_UTILS_HPP
