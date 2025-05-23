#ifndef CALCULATE_UNIVERSALITY_H
#define CALCULATE_UNIVERSALITY_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils/Alphabet.h"

inline int calculateUniversalityIndex(const std::string& text) {
    const int alphabetSize = Alphabet::getInstance().size();

    std::vector<int> required(alphabetSize, 0);     // target map: needed letters
    std::vector<int> windowCount(alphabetSize, 0);  // current window count

    int archCount = 0;
    int uniqueCount = 0;
    int left = 0;
    int n = text.size();

    for (int right = 0; right < n; ++right) {
        char c = text[right];
        int idx = Alphabet::getInstance().charToIndex(c);

        // expand window
        if (windowCount[idx] == 0) {
            ++uniqueCount;
        }
        ++windowCount[idx];

        // check if current window is a minimal arch
        if (uniqueCount == alphabetSize) {
            ++archCount;

            // move left to end of arch (minimize window)
            left = right;

            // reset counts
            windowCount.assign(alphabetSize, 0);
            uniqueCount = 0;
        }
    }

    return archCount;
}
#endif  // CALCULATE_UNIVERSALITY_H
