#ifndef ALPHABET_H
#define ALPHABET_H

#include <string>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

class Alphabet {
   public:
    static Alphabet& getInstance() {
        static Alphabet instance;
        return instance;
    }

    std::string getAlphabet() const { return alphabet; }

    void setAlphabet(const std::string& value) {
        alphabet = value;
        indexMap.clear();
        for (int i = 0; i < alphabet.size(); i++) {
            indexMap[alphabet[i]] = i;
        }
    }

    int size() const { return alphabet.size(); }

    char indexToChar(int index) const {
        if (index < 0 || index >= alphabet.size())
            throw std::out_of_range("indexToChar: index out of range");
        return alphabet.at(index);
    }

    int charToIndex(char c) {
        if (indexMap.count(c)) {
            return indexMap.at(c);
        } else if (allowExtension) {
            int newIndex = alphabet.size();
            alphabet += c;
            indexMap[c] = newIndex;
//            std::cerr << "[Alphabet] Extended alphabet with '" << c << "' at index " << newIndex << '\n';
            return newIndex;
        } else {
//            std::cerr << "[Alphabet Error] Character '" << c << "' not in alphabet.\n";
            throw std::out_of_range("charToIndex: character not in alphabet");
        }
    }

    void enableExtension(bool value) {
        allowExtension = value;
    }

   private:
    std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::unordered_map<char, int> indexMap;
    bool allowExtension = false; // default: fixed alphabet

    Alphabet() {
        for (int i = 0; i < alphabet.size(); ++i) {
            indexMap[alphabet[i]] = i;
        }
    }
    ~Alphabet() {}

    Alphabet(const Alphabet&) = delete;
    Alphabet& operator=(const Alphabet&) = delete;
};
#endif