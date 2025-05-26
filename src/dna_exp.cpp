#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <algorithm>
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

    // 0) Read input JSON
    ifstream input_file(argv[1]);
    json input_json = json::parse(input_file);
    input_file.close();

    string animal = input_json["animal"];
    json sequences = input_json["sequences"];
    int M = static_cast<int>(sequences.size());

    // Prepare results vector to preserve sequence order
    vector<json> results(M);

    // Initialize alphabet once
    Alphabet &alpha = Alphabet::getInstance();
    alpha.setAlphabet("ATGC");

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < M; ++i) {
        const auto &data = sequences[i];
        const string sequence = data["sequence"];
        int exp_k = data["sequence_length"];

        // Map to accumulate pattern info per unique SNF pattern
        unordered_map<string, json> pattern_map;
        pattern_map.reserve(1024);

        int n = static_cast<int>(sequence.size());

        for (int start = 0; start < n; ++start) {
            for (int len = 1; start + len <= n; ++len) {
                // Get substring and its SNF pattern
                string_view raw_view(sequence.data() + start, len);
                string subseq(raw_view);
                string pattern = computeShortlexNormalForm(subseq, exp_k);

                // Skip trivial patterns
                int universality = calculateUniversalityIndex(pattern);
                if (static_cast<int>(pattern.length()) == universality * alpha.size())
                    continue;

                // Measure matching
                auto t0 = chrono::high_resolution_clock::now();
                auto positions = MatchSimK::matchSimK(sequence, pattern, exp_k);
                auto t1 = chrono::high_resolution_clock::now();
                double duration = chrono::duration<double, milli>(t1 - t0).count();

                // Compute matches count for this occurrence
                long long this_matches = 0;
                for (const auto &pos : positions) {
                    const auto &int1 = get<0>(pos);
                    const auto &int2 = get<1>(pos);
                    this_matches += static_cast<long long>(int1.end - int1.start + 1) *
                                    (int2.end - int2.start + 1);
                }
                if (this_matches == 0) continue;

                auto it = pattern_map.find(pattern);
                if (it == pattern_map.end()) {
                    // First occurrence: create JSON entry
                    json info;
                    info["pattern"] = pattern;
                    info["pattern_universality"] = universality;
                    info["num_matches"] = this_matches;
                    info["duration_ms"] = duration;
                    info["occurrences"] = 1;
                    pattern_map.emplace(pattern, move(info));
                } else {
                    // Subsequent occurrence: accumulate
                    json &info = it->second;
                    info["num_matches"] = info["num_matches"].get<long long>() + this_matches;
                    info["duration_ms"] = info["duration_ms"].get<double>() + duration;
                    info["occurrences"] = info["occurrences"].get<long long>() + 1;
                }
            }
        }

        // Collect and sort pattern infos by num_matches descending
        vector<json> patterns;
        patterns.reserve(pattern_map.size());
        for (auto &p : pattern_map) patterns.push_back(move(p.second));
        sort(patterns.begin(), patterns.end(), [](const json &a, const json &b) {
            return a["num_matches"].get<long long>() > b["num_matches"].get<long long>();
        });

        // Assemble result
        json result;
        result["class"] = data["class"];
        result["id"] = data["id"];
        result["sequence"] = sequence;
        result["sequence_length"] = n;
        result["patterns"] = patterns;

        results[i] = move(result);
    }

    // Output JSON
    json out_json;
    out_json["animal"] = animal;
    out_json["results"] = results;

    ofstream output_file(argv[2]);
    output_file << out_json.dump(4);
    output_file.close();

    return 0;
}
