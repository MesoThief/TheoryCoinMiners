#ifndef ALPHABET_H
#define ALPHABET_H

#include <string>
#include <unordered_map>

class Alphabet {
  public:
    static Alphabet& getInstance() {
      static Alphabet instance;
      return instance;
    }

    int size() {
      return alphabet.size();
    }

    char indexToChar(int index) {
      return alphabet.at(index);
    }

    int charToIndex(char c) {
      return indexMap.at(c);
    }

  private:
    // Alphabet (∑) 정의: 필요에 따라 수정
    std::string alphabet = "abcdefghijklmnopqrstuvwxyz";

    std::unordered_map<char, int> indexMap;
    
    Alphabet() {
      for (int i = 0; i < alphabet.size(); i++) {
        indexMap[alphabet[i]] = i;
      }
    }
    ~Alphabet() {}

    Alphabet(const Alphabet&) = delete;
    Alphabet& operator=(const Alphabet&) = delete;
};
#endif