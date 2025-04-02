#include "Ranker.h"
#include <limits>
#include <stdexcept>

const int RankerTable::INF = std::numeric_limits<int>::max();

RankerTable::RankerTable(const std::string& text, int alphabetSize)
        : text(text), alphabetSize(alphabetSize) {
    int n = text.size();
    xTable.assign(n + 1, std::vector<int>(alphabetSize, INF));
    yTable.assign(n + 1, std::vector<int>(alphabetSize, -1));

    for (int i = 0; i < alphabetSize; ++i) {
        charToIndexMap['a' + i] = i;
    }
}

int RankerTable::charToIndex(char c) const {
    auto it = charToIndexMap.find(c);
    if (it == charToIndexMap.end())
        throw std::invalid_argument("Character not in alphabet");
    return it->second;
}

void RankerTable::buildXRankerTable() {
    int n = text.size();
    std::vector<int> next(alphabetSize, INF);

    for (int i = n - 1; i >= 0; --i) {
        next[charToIndex(text[i])] = i + 1;
        for (int c = 0; c < alphabetSize; ++c) {
            xTable[i][c] = next[c];
        }
    }
}

void RankerTable::buildYRankerTable() {
    int n = text.size();
    std::vector<int> prev(alphabetSize, -1);

    for (int i = 0; i < n; ++i) {
        prev[charToIndex(text[i])] = i + 1;
        for (int c = 0; c < alphabetSize; ++c) {
            yTable[i + 1][c] = prev[c];
        }
    }
}

int RankerTable::getX(int index, char c) const {
    return xTable[index][charToIndex(c)];
}

int RankerTable::getY(int index, char c) const {
    return yTable[index][charToIndex(c)];
}
