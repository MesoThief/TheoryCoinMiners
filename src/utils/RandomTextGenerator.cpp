#include "utils/RandomTextGenerator.h"

#include "utils/Alphabet.h"

std::string generateRandomText(int length) {
    int alphabetSize = Alphabet::getInstance().size();

    std::string result;
    result.reserve(length);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, alphabetSize - 1);

    for (int i = 0; i < length; ++i) {
        result += Alphabet::getInstance().indexToChar(dist(gen));
    }

    return result;
}