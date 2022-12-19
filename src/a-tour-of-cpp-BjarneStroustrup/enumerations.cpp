// enumerations.cpp: Faisal Bhuiyan
// Description: a short tutorial on enums

#include <iostream>
#include <string>

/*
 * In addition to classes, C++ supports a simple form of user-defined type for which
 * we can enumerate the values - enum.
 * Enumerations are used to represent small sets of integer values. They are used to
 * make code more readable and less error-prone than it would have been had the symbolic
 * and mnemonic enumerator names not been used.
*/

enum class Color { red, green, blue };
enum class TrafficLight { green, yellow, red };

// By default, an enum class has only assignment, initialization, and comparisons (e.g. == and <)
// defined. However, an enumeration is a user-defined type, so we can define operators for it.
TrafficLight& operator++(TrafficLight& t) {
    switch(t) {
        case TrafficLight::green: {
            return t = TrafficLight::yellow;
        }
        case TrafficLight::yellow: {
            return t = TrafficLight::red;
        }
        case TrafficLight::red: {
            return t = TrafficLight::green;
        }
    }
}

int main() {
    // The class after the enum specifies that an enumeration is strongly typed and
    // that its enumerators are scoped
    // Color x = red; // error: which red?
    // Color y = TrafficLight::red; // error: red is not a Color
    Color z = Color::red; // ok

    // Similarly, we cannot implicitly mix Color and integer values
    // int i = Color::red; // error: Color::Red is not an int
    // Color c = 2; // error: 2 is not a Color

    auto current_signal = TrafficLight::red;
    auto next_signal = ++current_signal;
}

