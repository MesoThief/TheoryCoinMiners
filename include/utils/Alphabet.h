#ifndef ALPHABET_H
#define ALPHABET_H

#include <string>
#include <stdexcept>
#include <vector>

#define MAP_SIZE 256
#define UNDEF -1

class Alphabet {
   public:
    static Alphabet& getInstance() {
        static Alphabet instance;
        return instance;
    }

    std::string getAlphabet() const { return alphabet; }

    void setAlphabet(const std::string& value) {
        alphabet = value;
        indexMap.assign(MAP_SIZE, UNDEF); // mark all invalid first
        for (int i = 0; i < (int)alphabet.size(); i++) {
            unsigned char uc = static_cast<unsigned char>(alphabet[i]);
            indexMap[uc] = i;
        }
    }

    int size() const { return (int)alphabet.size(); }

    char indexToChar(int idx) const {
        if (idx < 0 || idx >= (int)alphabet.size())
            throw std::out_of_range("Alphabet index out of range");
        return alphabet[idx];
    }

    inline int charToIndex(char c) const {
        int idx = indexMap[static_cast<unsigned char>(c)];
        if (idx < 0)
            throw std::out_of_range("Character not in alphabet");
        return idx;
    }

   private:
    // 기본 Alphabet (∑) 정의: 필요에 따라 수정
    std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::vector<int> indexMap;

    Alphabet() {
        setAlphabet(alphabet);      
    }
    ~Alphabet() {}

    Alphabet(const Alphabet&) = delete;
    Alphabet& operator=(const Alphabet&) = delete;
};
#endif