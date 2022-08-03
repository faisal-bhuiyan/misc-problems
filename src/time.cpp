// time.cpp: Faisal Bhuiyan
// Description: A simple time class implementation as proof of concept.

#include <iostream>

using namespace std;

/*
 * A simple class to keep track of time in HH:MM:SS format
 * Limitations: Meant to keep wallclock time - no concept of time beyond hours (e.g. days missing).
 *              Smallest resolution of time is measured in 1 second.
 */
class Time {
public:
    Time();
    Time(int h, int m, int s);

    /// Set the current time to the provided values
    inline void set(int h, int m, int s) {
        this->hours_ = h;
        this->minutes_ = m;
        this->seconds_ = s;
        return;
    }

    /// Increment time by 1 second
    void increment();

    /// Decrement time by 1 second
    void decrement();

    /// Display the current time in AM/PM format
    void display() const;

    /// Get the current time
    inline int Hours() const { return hours_; }
    inline int Minutes() const { return minutes_; }
    inline int Seconds() const { return seconds_; }

private:
    int hours_;
    int minutes_;
    int seconds_;
};

Time::Time() : hours_(0), minutes_(0), seconds_(0) {
}

Time::Time(int h, int m, int s) : hours_(h), minutes_(m), seconds_(s) {
}

void Time::increment() {
    seconds_++;
    minutes_ += seconds_ / 60;
    hours_ += minutes_ / 60;

    seconds_ %= 60;
    minutes_ %= 60;
    hours_ %= 24;

    return;
}

void Time::decrement() {
    seconds_--;
    if (seconds_ < 0) {
        seconds_ += 60;
        minutes_--;
    }
    if (minutes_ < 0) {
        minutes_ += 60;
        hours_--;
    }
    if (hours_ < 0) {
        hours_ += 24;
    }

    return;
}

void Time::display() const {
    cout << (hours_ % 12 ? hours_ % 12 : 12) << ':' << (minutes_ < 10 ? "0" : "") << minutes_ << ':'
        << (seconds_ < 10 ? "0" : "") << seconds_ << (hours_ < 12 ? " AM" : " PM") << endl;
}

Time ElapsedTime(const Time& start, const Time& finish) {
    int sec {0},
        min {0},
        hr {0};

    sec = finish.Seconds() - start.Seconds();
    if (sec < 0) {
        sec += 60;
        min--;
    }

    min += finish.Minutes() - start.Minutes();
    if (min < 0) {
        min += 60;
        hr--;
    }

    hr += finish.Hours() - start.Hours();
    if (hr < 0) {
        hr += 24;
    }

    return Time(hr, min, sec);
}

int main() {
    Time timer1;
    timer1.set(23,59,58);

    for (int i = 0; i < 5; i++) {
        timer1.increment();
        timer1.display();
    }

    Time timer2(0, 0, 3);

    for (int i = 0; i < 5; i++) {
        timer2.decrement();
        timer2.display();
    }

    auto elapsed = ElapsedTime(timer2, timer1);
    cout << "Elapsed time: " << elapsed.Hours() << " hours, " << elapsed.Minutes() << " minutes, " <<
        elapsed.Seconds() << " seconds" << "\n";
}

