#include "database.h"

#include <algorithm>
#include <iostream>

Database::Database() : composers_ {} {}

Composer& Database::AddComposer(std::string first_name, std::string last_name, std::string genre, int yob,
    std::string fact, int ranking) {
    composers_.push_back(Composer(first_name, last_name, yob, genre, fact, ranking));
    return composers_.back();
}

Composer&  Database::GetComposer(std::string last_name) {
    // TODO Throw if the requested name could not be found
    for (auto& c : composers_) {
        if (c.LastName() == last_name) {
            return c;
        }
    }
}

void Database::DisplayAll() const {
    for (const auto& c : composers_) {
        c.Display();
    }
}

void Database::DisplayByRank() {
    std::sort(
        composers_.begin(),
        composers_.end(),
        [](const Composer& a, const Composer& b) -> bool{ return a.Ranking() < b.Ranking(); }
    );

    this->DisplayAll();
}

