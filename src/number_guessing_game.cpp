// number_guessing_game.cpp: Faisal Bhuiyan
// Description: Generate a random number and help the player guess it.

// We will work on this game in three steps.
// - Figure out how to generate a random number within a given range of values.
// - Create a main function that processes one guess from the player, and provides hints.
// - Add what we need to allow for multiple guesses until the player guesses the number.

#include <iostream>
#include <random>

using namespace std;

int main() {
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(1, 100);

    int rvalue = uniform_dist(r);
    // cout << "rvalue: " << rvalue << endl;

    cout << "Guess a random integer between 1 and 100." << endl;

    int input_var = 0;

    do {
        cout << "Enter a number (-1 = quit): ";
        cin >> input_var;

        // TODO Throw error for non-integer inputs
        if (input_var == -1) {
            cout << "Quitting..." << endl;
            break;
        }

        if (input_var < rvalue) {
            cout << "You entered: " << input_var << ". Guess higher!" << endl;
        } else if (input_var > rvalue) {
            cout << "You entered: " << input_var << ". Guess lower!" << endl;
        } else {
            cout << "You entered: " << input_var << ". You win!" << endl;
        }
    } while (input_var != rvalue);

    return 0;
}

