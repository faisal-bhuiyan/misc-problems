// database.h: Faisal Bhuiyan
// Description: Class for a database of Composer records.

#include <vector>

#include "composer.h"

class Database {
public:
    Database();

    /* Add a new composer using operations in the Composer class.
     * For convenience, we return a reference (pointer) to the new record.
     */
    Composer& AddComposer(std::string first_name, std::string last_name, std::string genre,
        int yob, std::string fact, int ranking=Composer::kDefaultRanking);

    /// Search for a composer based on last name. Return a reference to the found record.
    Composer& GetComposer(std::string last_name);

    /// Display all composers in the database.
    void DisplayAll() const;

    /// Sort database records by rank and then display all.
    void DisplayByRank();

private:
    std::vector<Composer> composers_;
};

