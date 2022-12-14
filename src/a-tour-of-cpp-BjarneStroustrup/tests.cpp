// tests.cpp: Faisal Bhuiyan
// Description: Demonstration of conditional and loop statements

#include <iostream>
#include <string>
#include <vector>

// a simple function that prompts the user and returns a Boolean indicating the response
bool accept() {
    std::cout << "Do you want to proceed? (y or n) \n";    // write question
    char answer {0};   // initialize to a value that will not appear on input
    std::cin >> answer;

    if (answer == 'y') {
        return true;
    }
    return false;
}

// a second iteration for the accept function using switch
bool accept2() {
    std::cout << "Do you want to proceed? (y or n) \n";    // write question
    char answer {0};    // initialize to a value that will not appear on input
    std::cin >> answer;

    switch (answer) {
        case 'y':
            return true;
        case 'n':
            return false;
        default:
            std::cout << "I'll take that as a no.\n";
            return false;
    }
}

// A class to hold two-dimensional points (to illustrate the use of action() function)
class Point {
public:
    Point(double x=0., double y=0.) : x_(x), y_(y) {
    }

    inline double GetX() const { return this->x_; }
    inline double GetY() const { return this->y_; }

    Point operator+=(const Point& pt) const {
        return Point(this->x_ + pt.GetX(), this->y_ + pt.GetY());
    }

private:
    double x_;
    double y_;
};

// a parser for a trivial command video game based on switch
void action() {
    while (true) {
        std::cout << "enter action: \n";    // request action
        std::string act;
        std::cin >> act;    // read characters into a string
        Point delta {0, 0};    // Point holds an {x,y} pair

        for (char ch : act) {
            switch (ch) {
            case 'u':
                // go up
            case 'n':
                // go north
                delta += Point(0., 1.);
                break;
            case 'r':
                // go right
            case 'e':
                // go east
                delta += Point(1., 0.);
                break;
            // ... more actions ...
            default:
                std::cout << "I freeze!\n";
            }
        }

        // move(current + delta * scale);
        // update_display;
    }
}

// Introduce a variable in if-statement condition declaration:
// The purpose of this is to keep the scope of the variable limited to improve readability
// and minimize errors
void do_something(std::vector<int>& v) {
    if (auto n = v.size(); n!= 0) {
        // ... we get here if n != 0
    }
    // ...
}

// The most common case of the above is testing a variable against 0 or the nullptr:
// To do that, simply leave out the explicit mention of the condition
void do_something2(std::vector<int>& v) {
    // A terser and simpler form of the do_something function above
    if (auto n = v.size()) {
        // ... we get here if n != 0
    }
    // ...
}

int main() {
    accept();
    accept2();

    std::vector<int> v {1, 2, 3};

    do_something(v);
    do_something2(v);
}

