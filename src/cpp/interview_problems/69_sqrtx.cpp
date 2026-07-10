/*
 * Given a non-negative integer x, return the square root of x rounded down to the nearest integer.
 * The returned integer should be non-negative as well.
 *
 * You must not use any built-in exponent function or operator.
 *
 * For example, do not use pow(x, 0.5) in c++ or x ** 0.5 in python.
 */

#include <cmath>

class Solution {
public:
    int mySqrt(int x) {
        // base case
        if (x < 2) {
            return x;
        }

        // general case: use Newton's method to solve for sqrt(x) -> quadratic convergence
        // t_i+1 = t_i - f(t) / f'(t)
        // Let, t = sqrt(x) -> here t is the variable
        // f(t) = t^2 - x = 0 -> f'(t) = 2t
        // t_i+1 = (t_i + (x / t_i)) / 2

        // initial guess
        unsigned long t = x;
        while (t * t > x) {
            t = std::floor((t + (x / t)) / 2);
        }
        return t;
    }
};
