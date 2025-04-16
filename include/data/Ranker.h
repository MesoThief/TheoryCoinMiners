#ifndef RANKER_H
#define RANKER_H

#include <string>
#include <unordered_map>
#include <vector>

class RankerTable {
   public:
    RankerTable(const std::string &text);

    void buildXRankerTable();
    void buildYRankerTable();

    int getX(int index, char c) const;
    int getY(int index, char c) const;

   private:
    std::string text;
    std::vector<std::vector<int>> xTable;  // [index][char]
    std::vector<std::vector<int>> yTable;  // [index][char]
};

#endif  // RANKER_H
