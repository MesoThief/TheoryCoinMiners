#ifndef CALCULATE_UNIVERSALITY_H
#define CALCULATE_UNIVERSALITY_H

#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

int calculateUniversalityIndex(const std::string& text, int alphabetSize) {
    if (alphabetSize < 1 || alphabetSize > 26) {
        throw std::invalid_argument("Alphabet size must be between 1 and 26.");
    }
  
    const std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
    std::vector<int> required(alphabetSize, 0);  // target map: needed letters
    std::vector<int> windowCount(alphabetSize, 0);  // current window count
  
    int archCount = 0;
    int uniqueCount = 0;
    int left = 0;
    int n = text.size();
  
    for (int right = 0; right < n; ++right) {
        char c = text[right];
        int idx = c - 'a';
  
        // expand window
        if (windowCount[idx] == 0) {
            ++uniqueCount;
        }
        ++windowCount[idx];
  
        // check if current window is a minimal arch
        if (uniqueCount == alphabetSize) {
            ++archCount;
  
            // move left to end of arch (minimize window)
            ++right;  // move past the arch
            left = right;
  
            // reset counts
            windowCount.assign(alphabetSize, 0);
            uniqueCount = 0;
        }
    }
  
    return archCount;
  }
  #endif // CALCULATE_UNIVERSALITY_H
