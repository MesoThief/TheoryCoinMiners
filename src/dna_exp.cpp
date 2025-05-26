#include <fstream>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <chrono>              // for timing
#include <nlohmann/json.hpp>

#include "data/MatchSimK.h"
#include "utils/Alphabet.h"
#include "data/Shortlex.h"
#include "utils/Common.h"
#include "utils/CalculateUniversality.h"

using namespace std;
using json = nlohmann::json;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input-file-path> <output-file-path>\n";
        return 1;
    }

    // 0) Read all inputs first
    ifstream input_file(argv[1]);
    json input_json = json::parse(input_file);
    input_file.close();
    
    string animal = input_json["animal"];

    json sequences = input_json["sequences"];

    // 1) Init alphabet once
    Alphabet &alpha = Alphabet::getInstance();
    alpha.setAlphabet("ATGC");

    int exp_k = 15;

    json results = json::array();
    std::mutex results_mutex;

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < sequences.size(); ++i) {
        const auto &data = sequences[i];

        json result = json::object();
        string sequence = data["sequence"];

        set<string> pattern_set;

        result["class"] = data["class"];
        result["id"] = data["id"];
        result["sequence"] = sequence;
        result["sequence_length"] = data["sequence_length"];
        result["patterns"] = json::array();

        int n = (int)sequence.size();
        for (int start = 0; start < n; ++start) {
            for (int subsequence_length = 1; start + subsequence_length <= n; ++subsequence_length) {
                string subsequence = sequence.substr(start, subsequence_length);
                string pattern = computeShortlexNormalForm(subsequence, exp_k);

                if (pattern_set.find(pattern) == pattern_set.end()) {
                    pattern_set.emplace(pattern);
                } else {
                    continue;
                }

                auto t0 = chrono::high_resolution_clock::now();
                auto positions = MatchSimK::matchSimK(sequence, subsequence, exp_k);
                auto t1 = chrono::high_resolution_clock::now();
                double duration = chrono::duration<double, milli>(t1 - t0).count();

                long num_matches = 0;
                for (const auto &position : positions) {
                    const auto &interval_1 = get<0>(position);
                    const auto &interval_2 = get<1>(position);
                    num_matches += (interval_1.end - interval_1.start + 1) * (interval_2.end - interval_2.start + 1);
                }

                if (num_matches == 0) continue;

                json pattern_info = json::object();
                pattern_info["pattern"] = pattern;
                pattern_info["pattern_universality"] = calculateUniversalityIndex(pattern);
                pattern_info["num_matches"] = num_matches;
                pattern_info["duration_ms"] = duration;

                result["patterns"].push_back(pattern_info);
            }
        }

        // 병렬 결과 병합
        {
            std::lock_guard<std::mutex> lock(results_mutex);
            results.push_back(result);
        }
    }

    json out_json = json::object();
    out_json["animal"] = animal;
    out_json["results"] = results;

    ofstream output(argv[2]);
    output << out_json.dump(4);

    output.close();

    return 0;
}
