#ifndef RANKER_H
#define RANKER_H

#include <vector>
#include <string>
#include <unordered_map>

class RankerTable {
public:
    static const int INF;

    RankerTable(const std::string& text);

    void buildXRankerTable();
    void buildYRankerTable();

    int getX(int index, char c) const;
    int getY(int index, char c) const;

private:
    std::string text;
    std::vector<std::vector<int> > xTable; // [index][char]
    std::vector<std::vector<int> > yTable; // [index][char]
};

#endif // RANKER_H
