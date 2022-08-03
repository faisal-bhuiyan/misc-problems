// car.cpp: Faisal Bhuiyan
// Description: A simple car struct.

#include <iostream>
#include <string>

using namespace std;

struct Car {
public:
    string brand;
    string model;
    int year;

    Car() : brand(string("X")), model(string("Y")), year(0) {};

    Car(string brand, string model, int year) :
        brand(brand), model(model), year(year) {
    };
};

bool operator==(const Car& car1, const Car& car2) {
    if (car1.brand == car2.brand && car1.model == car2.model && car1.year == car2.year) {
        cout << "Provided cars are the same!" << "\n";
        return true;
    }
    cout << "Provided cars are different!" << "\n";
    return false;
}

int main() {
    Car car1("hidy", "ho", 1776);
    cout << "car1: " << "brand: " << car1.brand << ", model: " << car1.model << ", year: " << car1.year <<"\n";

    Car car2;
    cout << "car2: " << "brand: " << car2.brand << ", model: " << car2.model << ", year: " << car2.year <<"\n";

    car1 == car2;

    Car car3;
    car2 == car3;
}

