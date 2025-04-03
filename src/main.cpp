#include <string>
#include <random>
#include <stdexcept>
#include <iostream>

#include "data/Node.h"
#include "data/Ranker.h"
#include "data/Trees.h"

#include "utils/RandomTextGenerator.h"
#include "utils/CalculateUniversality.h"



int main() {
    // Manually set alphabet size and text length
    int alphabetSize = 5;  // for example: a, b, c, d, e
    int text_length = 20;

    // Generate Random text
    std::string randText = generateRandomText(alphabetSize, text_length);
    std::cout << "Random text: " << randText << "\n";

    // Make and build both X-ranker and Y-ranker table
    RankerTable ranker(randText, alphabetSize);
    ranker.buildXRankerTable();
    ranker.buildYRankerTable();

    // This is to show how both ranker tables are properly set
    // You can comment-out these loops
    std::cout << "\nX-ranker (next position +1 for each char):\n";
    for (int i = 0; i <= text_length; ++i) {
        for (char c = 'a'; c < 'a' + alphabetSize; ++c) {
            int result = ranker.getX(i, c);
            std::cout << "X(" << i << ", " << c << ") = ";
            if (result == RankerTable::INF) std::cout << "INF";
            else std::cout << result;
            std::cout << "\t";
        }
        std::cout << "\n";
    }

    std::cout << "\nY-ranker (previous position for each char):\n";
    for (int i = 0; i <= text_length; ++i) {
        for (char c = 'a'; c < 'a' + alphabetSize; ++c) {
            int result = ranker.getY(i, c);
            std::cout << "Y(" << i << ", " << c << ") = ";
            std::cout << result << "\t";
        }
        std::cout << "\n";
    }

    // Generate a random pattern
    int pattern_length = 6;
    int congruence_class = 2;
    std::string randPattern = generateRandomText(alphabetSize, pattern_length);

    // Get universality index of such pattern
    int universality = calculateUniversalityIndex(randPattern, alphabetSize);

    // Make k-class shortlex form of a generated pattern
    ShortlexResult shortlex_pattern = computeShortlexNormalForm(randPattern, universality, congruence_class);

    // Build X-tree
    Node xRoot = Trees::buildXTree(ranker, shortlex_pattern, randText);

    // Build Y-tree
    Node yRoot = Trees::buildYTree(ranker, shortlex_pattern, randText);
    
    return 0;
}
