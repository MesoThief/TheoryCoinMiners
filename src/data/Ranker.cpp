#include "data/Ranker.h"

#include <limits>
#include <stdexcept>

#include "utils/Alphabet.h"
#include "utils/Common.h"

RankerTable::RankerTable(const std::string &text) : text(text) {
    int n = text.size();
    int alphabetSize = Alphabet::getInstance().size();

    xTable.assign(n + 1, std::vector<int>(alphabetSize, INF));
    yTable.assign(n + 1, std::vector<int>(alphabetSize, -1));
}

void RankerTable::buildXRankerTable() {
    int n = text.size();
    int alphabetSize = Alphabet::getInstance().size();

    std::vector<int> next(alphabetSize, INF);

    for (int i = n - 1; i >= 0; --i) {
        int charIndex = Alphabet::getInstance().charToIndex(text[i]);
        next[charIndex] = i + 1;
        for (int c = 0; c < alphabetSize; ++c) {
            xTable[i][c] = next[c];
        }
    }
}

void RankerTable::buildYRankerTable() {
    int n = text.size();
    int alphabetSize = Alphabet::getInstance().size();

    std::vector<int> prev(alphabetSize, -1);

    for (int i = 0; i < n; ++i) {
        int charIndex = Alphabet::getInstance().charToIndex(text[i]);
        prev[charIndex] = i + 1;
        for (int c = 0; c < alphabetSize; ++c) {
            yTable[i + 1][c] = prev[c];
        }
    }
}

int RankerTable::getX(int index, char c) const {
    int charIndex = Alphabet::getInstance().charToIndex(c);
    return xTable[index][charIndex];
}

int RankerTable::getY(int index, char c) const {
    int charIndex = Alphabet::getInstance().charToIndex(c);
    return yTable[index][charIndex];
}
