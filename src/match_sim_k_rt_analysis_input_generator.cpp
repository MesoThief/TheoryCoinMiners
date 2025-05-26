#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "data/Shortlex.h"
#include "utils/Alphabet.h"
#include "utils/Common.h"
#include "utils/RandomTextGenerator.h"

using namespace std;
using json = nlohmann::json;

// ------------------
// MatchSimK Running Time Analysis
// ------------------
int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << "<input-file-path> <output-file-path>" << endl;
        return 1;
    }

    ifstream inputFile(argv[1]);
    json input_json = json::parse(inputFile);
    inputFile.close();

    string alphabet = input_json["alphabet"];
    Alphabet::getInstance().setAlphabet(alphabet);

    int k = input_json["k"];
    cout << "k: " << k << endl;

    int pattern_length = input_json["pattern_length"];
    cout << "pattern_length: " << pattern_length << endl;
    
    string pattern = generateRandomText(pattern_length);
    string pattern_shortlex = computeShortlexNormalForm(pattern, k);

    cout << "pattern: " << pattern << endl; 
    cout << "pattern_shortlex: " << pattern_shortlex << endl; 

    vector<int> text_lengths = input_json["text_lengths"];

    json output_res;
    output_res["alphabet"] = alphabet;
    output_res["k"] = k;
    output_res["pattern"] = pattern;
    output_res["pattern_shortlex"] = pattern_shortlex;
    output_res["text_lengths"] = text_lengths;

    output_res["texts"] = json::array();
    for (int text_length : text_lengths) {
        string text = generateRandomText(text_length);
        output_res["texts"].push_back(text);
    }

    ofstream output(argv[2]);
    output << output_res.dump(4);
    output.close();

    return 0;
}