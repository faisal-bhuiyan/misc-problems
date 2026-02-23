// number_guessing_game.cpp: Faisal Bhuiyan
// Description: Generate a random number and help the player guess it

/*
 * We will work on this game in three steps.
 * - Figure out how to generate a random number within a given range of values.
 * - Create a main function that processes one guess from the player, and provides hints.
 * - Add what we need to allow for multiple guesses until the player guesses the number.
 */

#include <iostream>
#include <random>

int main() {
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(1, 100);

    int rvalue = uniform_dist(r);
    // std::cout << "rvalue: " << rvalue << std::endl;

    std::cout << "Guess a random integer between 1 and 100." << std::endl;

    int input_var = 0;

    do {
        std::cout << "Enter a number (-1 = quit): ";
        std::cin >> input_var;

        // TODO Throw error for non-integer inputs
        if (input_var == -1) {
            std::cout << "Quitting..." << std::endl;
            break;
        }

        if (input_var < rvalue) {
            std::cout << "You entered: " << input_var << ". Guess higher!" << std::endl;
        } else if (input_var > rvalue) {
            std::cout << "You entered: " << input_var << ". Guess lower!" << std::endl;
        } else {
            std::cout << "You entered: " << input_var << ". You win!" << std::endl;
        }
    } while (input_var != rvalue);

    return 0;
}
