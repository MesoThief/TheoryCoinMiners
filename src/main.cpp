#include <string>
#include <random>
#include <stdexcept>
#include <iostream>
#include <vector>

#include "data/Ranker.h"
#include "data/XYTree.h"

#include "utils/Alphabet.h"
#include "utils/RandomTextGenerator.h"
#include "utils/CalculateUniversality.h"
#include "utils/Common.h"

int main() {
    // Manually set alphabet size and text length
    Alphabet::getInstance().setAlphabet("abcde");
    int alphabetSize = Alphabet::getInstance().size();
    int text_length = 20;

    // Generate Random text
    std::string randText = generateRandomText(text_length);
    std::cout << "Random text: " << randText << std::endl;

    // Generate a random pattern
    int pattern_length = 6;
    int k = 2;
    std::string randPattern = generateRandomText(pattern_length);
    std::cout << "Random patter: " << randPattern << std::endl;

    // Get universality index of such pattern
    int universality = calculateUniversalityIndex(randPattern);

    // Make k-class shortlex form of a generated pattern
    ShortlexResult pattern_shortlex = computePartialShortlexNormalForm(
        randPattern,
        vector<int>(alphabetSize, 1),
        vector<int>(alphabetSize, 1),
        k + 1
    );

    // Make and build both X-ranker and Y-ranker table
    RankerTable ranker(randText);
    ranker.buildXRankerTable();
    ranker.buildYRankerTable();

    // This is to show how both ranker tables are properly set
    // You can comment-out these loops
    std::cout << "\nX-ranker (next position +1 for each char):\n";
    for (int i = 0; i <= text_length; ++i) {
        for (char c : Alphabet::getInstance().getAlphabet()) {
            int result = ranker.getX(i, c);
            std::cout << "X(" << i << ", " << c << ") = ";
            if (result == INF) std::cout << "INF";
            else std::cout << result;
            std::cout << "\t";
        }
        std::cout << "\n";
    }

    std::cout << "\nY-ranker (previous position for each char):\n";
    for (int i = 0; i <= text_length; ++i) {
        for (char c : Alphabet::getInstance().getAlphabet()) {
            int result = ranker.getY(i, c);
            std::cout << "Y(" << i << ", " << c << ") = ";
            std::cout << result << "\t";
        }
        std::cout << "\n";
    }

    // Build X-tree
    XYTree::Tree x_tree = XYTree::buildXTree(ranker, pattern_shortlex, randText);

    // Build Y-tree
    XYTree::Tree y_tree = XYTree::buildYTree(ranker, pattern_shortlex, randText);
    
    return 0;
}
