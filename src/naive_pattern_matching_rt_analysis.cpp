#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <nlohmann/json.hpp>
#include <omp.h>

#include "data/MatchSimK.h"
#include "data/Shortlex.h"
#include "utils/Alphabet.h"
#include "utils/Common.h"
#include "utils/RandomTextGenerator.h"
#include "utils/CalculateUniversality.h"

using namespace std;
using json = nlohmann::json;

// ------------------
// Naive MatchSimK Running Time Analysis
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
        
        // sub_texts 추출 병렬적 전처리
        vector<string> sub_texts;
        #pragma omp parallel
        {
            vector<string> local_sub_texts;

            #pragma omp for nowait
            for (int start = 0; start < text_length; start++) {
                for (int sub_text_length = 1; sub_text_length <= text_length - start; sub_text_length++) {
                    local_sub_texts.push_back(text.substr(start, sub_text_length));
                }
            }

            #pragma omp critical
            {
                sub_texts.insert(sub_texts.end(), local_sub_texts.begin(), local_sub_texts.end());
            }
        }
        
        long num_matches = 0;
        auto start_time = chrono::high_resolution_clock::now();
        for (string sub_text : sub_texts) {
            string sub_text_shortlex = computeShortlexNormalForm(sub_text, k);
            if (sub_text_shortlex == pattern_shortlex) {
                ++num_matches;
            }
        }
        auto end_time = chrono::high_resolution_clock::now();
        
        auto duration = chrono::duration<double, milli>(end_time - start_time).count();

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