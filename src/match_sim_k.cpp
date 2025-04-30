#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "data/MatchSimK.h"
#include "utils/Alphabet.h"
#include "utils/Common.h"

using namespace std;

// ------------------
// A simple test driver for MatchSimK.cpp
// ------------------
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "You must enter a test input file" << endl;
        cerr << "Usage: " << argv[0] << " <test-input-file-name>" << endl;
        return 1;
    }

    string inputFileName = argv[1];
    ifstream inputFile(inputFileName);
    if (!inputFile) {
        cerr << "Error opening " << inputFileName << endl;
        return 1;
    }

    string alphabet;
    string text;
    string pattern;
    int k;

    getline(inputFile, alphabet);
    Alphabet::getInstance().setAlphabet(alphabet);

    getline(inputFile, text);
    getline(inputFile, pattern);
    string k_line;
    getline(inputFile, k_line);
    istringstream k_stream(k_line);
    k_stream >> k;

    cout << "text: " << text << endl;
    cout << "pattern: " << pattern << endl;
    cout << "k: " << k << endl;

    vector<MatchSimK::triple> positions = MatchSimK::matchSimK(text, pattern, k);

    cout << endl << "returned positions:" << endl;
    for (MatchSimK::triple position : positions) {
        cout << get<0>(position) << ", " << get<1>(position) << ", offset=" << get<2>(position) << endl;
    }

    return 0;
}