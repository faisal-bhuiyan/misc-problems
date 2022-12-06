// function_overload.cpp: Faisal Bhuiyan
// Description: An example of function overloading in C++

#include <iostream>
#include <string>

void print(int);            // takes an integer argument
void print(double);         // takes a floating point argument
void print(std::string);    // takes a string argument

void print(int i) {
    std::cout << "Provided integer: " << i << "\n";
}

void print(double d) {
    std::cout << "Provided real number: " << d << "\n";
}

void print(std::string s) {
    std::cout << "Provided string: " << s << "\n";
}

void user() {
    print(42);          // calls print(int)
    print(9.65);        // calls print(double)
    print("Barcelona"); // calls print(string)
}

int main(){
    user();
}

