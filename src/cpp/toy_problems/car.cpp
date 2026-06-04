// car.cpp: Faisal Bhuiyan
// Description: A simple car struct to illustrate PODs

#include <iostream>
#include <string>

struct Car {
    std::string brand;
    std::string model;
    int year;

    Car() : brand(std::string("X")), model(std::string("Y")), year(0) {};

    Car(std::string brand, std::string model, int year) : brand(brand), model(model), year(year) {};
};

// Overload the == operator to compare two Car objects
bool operator==(const Car& car1, const Car& car2) {
    if (car1.brand == car2.brand && car1.model == car2.model && car1.year == car2.year) {
        std::cout << "Provided cars are the same!" << "\n";
        return true;
    }
    std::cout << "Provided cars are different!" << "\n";
    return false;
}

int main() {
    Car car1("hidy", "ho", 1776);  // constructor with arguments
    std::cout << "car1: " << "brand: " << car1.brand << ", model: " << car1.model
              << ", year: " << car1.year << "\n";

    Car car2;  // default constructor
    std::cout << "car2: " << "brand: " << car2.brand << ", model: " << car2.model
              << ", year: " << car2.year << "\n";

    std::cout << "car1 == car2: " << (car1 == car2) << "\n";  // We expect this to be false

    Car car3;                                                 // default constructor
    std::cout << "car2 == car3: " << (car2 == car3) << "\n";  // We expect this to be true
}
