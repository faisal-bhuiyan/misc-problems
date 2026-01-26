// Implement the following methods:

// add on class TextInput - adds the given character to the current value
// getValue on class TextInput - returns the current value
// add on class NumericInput - overrides the base class method so that each non-numeric character is
// ignored

#include <iostream>
#include <string>
#include <vector>

/*
 * A simple class to handle user input
 */
class TextInput {
public:
    virtual void add(char c) { input_.push_back(c); }

    std::string getValue() { return input_; }

protected:
    std::string input_;
};

/*
 * A simple class to handle numeric input
 */
class NumericInput : public TextInput {
public:
    virtual void add(char c) override {
        if (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' ||
            c == '7' || c == '8' || c == '9') {
            input_.push_back(c);
        }
    }
};

#ifndef RunTests

int main() {
    TextInput* input{new NumericInput()};

    input->add('1');
    input->add('a');
    input->add('0');

    std::cout << input->getValue();

    delete input;
}

#endif
