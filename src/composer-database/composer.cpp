#include "composer.h"

#include <iostream>

Composer::Composer() :
    first_name_ {""},
    last_name_ {""},
    composer_yob_ {0},
    composer_genre_ {""},
    fact_ {""},
    ranking_ {kDefaultRanking} {
}

Composer::Composer(std::string first_name, std::string last_name, int yob, std::string genre, std::string fact,
    int ranking) :
    first_name_(first_name),
    last_name_(last_name),
    composer_yob_(yob),
    composer_genre_(genre),
    fact_(fact),
    ranking_(ranking) {
}

void Composer::Promote(int increment) {
    this->ranking_ -= increment;

    // Make sure the highest possible ranking is not lower than 1
    if (this->ranking_ < 1) {
        this->ranking_ = 1;
    }
}

void Composer::Demote(int decrement) {
    this->ranking_ += decrement;

    // Make sure the lowest possible ranking is not lower than the default
    if (this->ranking_ > kDefaultRanking) {
        this->ranking_ = kDefaultRanking;
    }
}

void Composer::Display() const {
    std::cout << '\n' << "Composer: " << this->first_name_ << " " << this->last_name_ << '\n' <<
        "Year of birth: " << this->composer_yob_ << '\n' <<
        "Genre of music: " << this->composer_genre_ << '\n' <<
        "A fun fact: " << this->fact_ << '\n' <<
        "Ranking: " << this->ranking_ << '\n';
}

