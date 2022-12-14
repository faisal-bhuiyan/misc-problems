/* composer.h: Faisal Bhuiyan
 * Description: Class for managing a Composer record.
 */

#include <string>

/* Class for managing a Composer record.
 * The default ranking is 10 which is the lowest possible.
 */
class Composer {
public:
    static constexpr int kDefaultRanking = 10;

    Composer();
    Composer(std::string first_name, std::string last_name, int yob, std::string genre, std::string fact,
        int ranking=kDefaultRanking);

    /// Accessors and Mutators
    inline void SetFirstName(std::string name) { first_name_ = name; }
    inline std::string FirstName() const { return first_name_; }

    inline void SetLastName(std::string name) { last_name_ = name; }
    inline std::string LastName() const { return last_name_; }

    inline void SetComposerYob(int year) { composer_yob_ = year; }
    inline int ComposerYob() const { return composer_yob_; }

    inline void SetComposerGenre(std::string genre) { composer_genre_ = genre; }
    inline std::string ComposerGenre() const { return composer_genre_; }

    inline void SetRanking(int rank) { ranking_ = rank; }
    inline int Ranking() const { return ranking_; }

    inline void SetFact(std::string fact) { fact_ = fact; }
    inline std::string Fact() const { return fact_; }

    /// Increase a composer's rank by increment
    void Promote(int increment);

    /// Decrease a composer's rank by decrement
    void Demote(int decrement);

    /// Display all the attributes of a composer
    void Display() const;

private:
    std::string first_name_;
    std::string last_name_;
    int composer_yob_; //< year of birth
    std::string composer_genre_; //< baroque, classical, romantic, etc.
    std::string fact_;
    int ranking_; //< an integer between 1 - 10, lower number indicates better rank
};

