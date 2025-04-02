#ifndef RANKER_H
#define RANKER_H

#include <vector>
#include <string>
#include <unordered_map>

class RankerTable {
public:
    static const int INF;

    RankerTable(const std::string& text, int alphabetSize);

    void buildXRankerTable();
    void buildYRankerTable();

    int getX(int index, char c) const;
    int getY(int index, char c) const;

private:
    std::string text;
    int alphabetSize;
    std::unordered_map<char, int> charToIndexMap;
    std::vector<std::vector<int> > xTable; // [index][char]
    std::vector<std::vector<int> > yTable; // [index][char]
    int charToIndex(char c) const;
};

#endif // RANKER_H
