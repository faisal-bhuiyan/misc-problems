// test_composer.cpp: Faisal Bhuiyan
//
// This program tests the Composer class.

#include "composer.h"

#include <iostream>

using namespace std;

int main() {
    cout << endl << "Testing the Composer class." << endl << endl;

    Composer composer;

    composer.SetFirstName("Ludwig van");
    composer.SetLastName("Beethoven");
    composer.SetComposerYob(1770);
    composer.SetComposerGenre("Romantic");
    composer.SetFact("Beethoven was completely deaf during the latter part of "
        "his life - he never heard a performance of his 9th symphony.");

    composer.Promote(2);
    composer.Demote(1);

    composer.Display();
}

