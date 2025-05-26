#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "data/MatchSimK.h"
#include "data/Shortlex.h"
#include "utils/Alphabet.h"
#include "utils/Common.h"
#include "utils/RandomTextGenerator.h"
#include "utils/CalculateUniversality.h"

using namespace std;
using json = nlohmann::json;

// ------------------
// MatchSimK Running Time Analysis
// ------------------
int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input-file-path> <output-file-path>\n";
        return 1;
    }

    string inputFileName = argv[1];
    ifstream inputFile(inputFileName);
    if (!inputFile) {
        cerr << "Error opening " << inputFileName << endl;
        return 1;
    }

    // HAIL JSON
    json input_json = json::parse(inputFile);
    inputFile.close();

    string alphabet = input_json["alphabet"];
    Alphabet::getInstance().setAlphabet(alphabet);

    int k = input_json["k"];
    cout << "k: " << k << endl;

    string pattern = input_json["pattern"];
    cout << "pattern: " << pattern << endl;
    
    string pattern_shortlex = input_json["pattern_shortlex"];
    cout << "pattern_shortlex: " << pattern_shortlex << endl; 

    vector<string> texts = input_json["texts"];

    json output_res;
    output_res["alphabet"] = alphabet;
    output_res["k"] = k;
    output_res["pattern"] = pattern;
    output_res["pattern_shortlex"] = pattern_shortlex;
#ifdef NOCP
    output_res["use_checkpoint"] = false;
#else
    output_res["use_checkpoint"] = true;
#endif

    output_res["result"] = json::array();

    for (string text : texts) {
        int text_length = text.size();
        cout << "Text length: " << text_length << endl;
        
        auto start_time = chrono::high_resolution_clock::now();
        vector<MatchSimK::triple> positions = MatchSimK::matchSimK(text, pattern_shortlex, k);
        auto end_time = chrono::high_resolution_clock::now();
        
        auto duration = chrono::duration<double, milli>(end_time - start_time).count();

        long num_matches = 0;
        for (const auto &pos : positions) {
            const auto &interval_1 = get<0>(pos);
            const auto &interval_2 = get<1>(pos);
            num_matches += (interval_1.end - interval_1.start + 1) * (interval_2.end - interval_2.start + 1);
        }

        json res_json;
        // res_json["text"] = text;
        res_json["text_length"] = text_length;
        res_json["duration_ms"] = duration;
        res_json["num_matches"] = num_matches;

        output_res["result"].push_back(res_json);
    }

    ofstream output(argv[2]);
    output << output_res.dump(4);
    output.close();

    return 0;
}