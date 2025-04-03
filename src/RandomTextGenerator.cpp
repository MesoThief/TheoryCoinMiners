#include "utils/RandomTextGenerator.h"

std::string generateRandomText(int alphabetSize, int length) {
  const std::string baseAlphabet = "abcdefghijklmnopqrstuvwxyz";

  if (alphabetSize < 1 || alphabetSize > 26) {
      throw std::invalid_argument("Alphabet size must be between 1 and 26.");
  }

  std::string alphabet = baseAlphabet.substr(0, alphabetSize);
  std::string result;
  result.reserve(length);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(0, alphabetSize - 1);

  for (int i = 0; i < length; ++i) {
      result += alphabet[dist(gen)];
  }

  return result;
}