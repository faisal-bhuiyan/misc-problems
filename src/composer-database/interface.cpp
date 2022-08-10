// interface.h: Faisal Bhuiyan
// Description: A function for interacting with a database of Composer records.

// Requirements are as follows:
// ---------------------------------------------
// 1) Add a new composer
// 2) Retrieve a composer's data
// 3) Promote/demote a composer's rank
// 4) List all composers
// 5) List all composers by rank
// 0) Quit

#include <iostream>
#include <stdexcept>
#include <string>

#include "database.h"

void DisplayOptions() {
    std::cout << "Please select an option from the following:" << std::endl <<
        "1-> Add a new composer" << std::endl <<
        "2-> Retrieve a composer's data" << std::endl <<
        "3-> Promote/demote a composer's rank" << std::endl <<
        "4-> List all composers" << std::endl <<
        "5-> List all composers by rank" << std::endl <<
        "0-> Quit the program" << std::endl;
}

void PrePopulate(Database& database) {
    // Pre-populate the database with 3 composers
    Composer& comp1 = database.AddComposer(
        "Ludwig van", "Beethoven", "Romantic", 1770,
        "Beethoven was completely deaf during the latter part of his life - he never "
        "heard a performance of his 9th symphony."
    );
    comp1.Promote(2);

    Composer& comp2 = database.AddComposer(
        "Johann Sebastian", "Bach", "Baroque", 1685,
        "Bach had 20 children, several of whom became famous musicians as well."
    );
    comp2.Promote(5);

    Composer& comp3 = database.AddComposer(
        "Wolfgang Amadeus", "Mozart", "Classical", 1756,
        "Mozart feared for his life during his last year - there is some evidence "
        "that he was poisoned."
    );
    comp3.Promote(8);
}

void AddComposer(Database& database) {
    std::cout << "Let's add a new composer to the database." << std::endl;

    std::cout << "Enter the first name of the composer:" << std::endl;
    std::string f_name;
    std::cin >> f_name;

    std::cout << "Enter the last name of the composer:" << std::endl;
    std::string l_name;
    std::cin >> l_name;
    if (l_name == database.GetComposer(l_name).LastName()) {
        std::cout << "Composer by name " << l_name << " already exists!" << std::endl;
        return;
    }

    std::cout << "Enter the genre of the composer:" << std::endl;
    std::string genre;
    std::cin >> genre;

    std::cout << "Enter the year of birth of the composer:" << std::endl;
    int year;
    std::cin >> year;

    std::cout << "Enter a fun fact about the composer:" << std::endl;
    std::string fact;
    std::cin >> fact;

    std::cout << "Enter a ranking for the composer (optional) between 1 and 10:" << std::endl;
    int rank = 0;
    std::cin >> rank;

    if (rank < 1 || rank > 10) {
        std::cout << "Provided ranking is invalid. Proceeding with the default ranking..." << std::endl;
        Composer& comp1 = database.AddComposer(f_name, l_name, genre, year, fact);
        return;
    }

    if (rank >= 1 and rank <= 10) {
        Composer& comp1 = database.AddComposer(f_name, l_name, genre, year, fact, rank);
    }
}

void RetrieveComposer(Database& database) {
    std::cout << "Let's retrieve information regarding an existing composer from the database." << std::endl;

    std::cout << "Enter the last name of the composer:" << std::endl;
    std::string l_name;
    std::cin >> l_name;

    if (l_name == database.GetComposer(l_name).LastName()) {
        database.GetComposer(l_name).Display();
    }
}

void PromoteOrDemote(Database& database) {
    std::cout << "Let's change the ranking of an existing composer from the database." << std::endl;

    std::cout << "Enter the last name of the composer:" << std::endl;
    std::string l_name;
    std::cin >> l_name;

    if (l_name == database.GetComposer(l_name).LastName()) {
        std::cout << "Choose if you want to promote (p) or demote (d):" << std::endl;
        char option;
        std::cin >> option;

        std::cout << "Enter the value for change in rank:" << std::endl;
        int value;
        std::cin >> value;

        if (option == 'p' || 'P') {
            database.GetComposer(l_name).Promote(value);
        } else if (option == 'd' || 'D') {
            database.GetComposer(l_name).Demote(value);
        } else {
            std::cout << "Please select a valid option between 'p' and 'd'!" << std::endl;
        }
    }
}

void TextInterface(Database& composer_db) {
    std::cout << "Welcome to the music composer database management program." << std::endl;

    std::cout << "Do you want to pre-populate the database with 3 composers? (y or n)" << std::endl;
    char ans;
    std::cin >> ans;
    if (ans == 'y' || 'Y') {
        PrePopulate(composer_db);
    }

    DisplayOptions();

    int input_var = -1;

    do {
        std::cout << "Enter an option (0 = quit): ";
        std::cin >> input_var;

        switch (input_var) {
        case 0:
            std::cout << "Quitting..." << std::endl;
            break;
        case 1:
            AddComposer(composer_db);
            break;
        case 2:
            RetrieveComposer(composer_db);
            break;
        case 3:
            PromoteOrDemote(composer_db);
            break;
        case 4:
            std::cout << "Displaying all composers: " << std::endl;
            composer_db.DisplayAll();
            break;
        case 5:
            std::cout << "Displaying all composers by rank: " << std::endl;
            composer_db.DisplayByRank();
            break;
        default:
            std::cout << "The provided input is not valid. Please enter a valid number to proceed." << std::endl;
            DisplayOptions();
            break;
        }
    } while (input_var != 0);
}

int main() {
    Database myDB;
    TextInterface(myDB);
}

