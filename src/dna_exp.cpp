#include <fstream>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <set>
#include <mutex>
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

    // 1) Init alphabet once
    Alphabet &alpha = Alphabet::getInstance();
    alpha.setAlphabet("ATGC");

    int exp_k = 15;

    json results = json::array();
    mutex results_mutex;

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < static_cast<int>(sequences.size()); ++i) {
        const auto &data = sequences[i];
        json result = json::object();

        string sequence = data["sequence"];
        result["class"] = data["class"];
        result["id"] = data["id"];
        result["sequence"] = sequence;
        result["sequence_length"] = data["sequence_length"];
        result["patterns"] = json::array();

        set<string> pattern_set;
        int n = static_cast<int>(sequence.size());

        for (int start = 0; start < n; ++start) {
            for (int subseq_len = 1; start + subseq_len <= n; ++subseq_len) {
                string_view raw_view(sequence.data() + start, subseq_len);
                string subseq(raw_view);
                string pattern = computeShortlexNormalForm(subseq, exp_k);

                if (!pattern_set.insert(pattern).second) {
                    continue;
                }

                auto t0 = chrono::high_resolution_clock::now();
                auto positions = MatchSimK::matchSimK(sequence, pattern, exp_k);
                auto t1 = chrono::high_resolution_clock::now();
                double duration = chrono::duration<double, milli>(t1 - t0).count();

                long long num_matches = 0;
                for (const auto &pos : positions) {
                    const auto &interval1 = get<0>(pos);
                    const auto &interval2 = get<1>(pos);
                    num_matches += static_cast<long long>((interval1.end - interval1.start + 1)) *
                                   (interval2.end - interval2.start + 1);
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

        // Sort patterns by num_matches descending
        auto &patterns = result["patterns"];
        sort(patterns.begin(), patterns.end(), [](const json &a, const json &b) {
            return a["num_matches"].get<long long>() > b["num_matches"].get<long long>();
        });

        // Merge into final results
        {
            lock_guard<mutex> lock(results_mutex);
            results.push_back(result);
        }
    }

    // Output JSON
    json out_json = json::object();
    out_json["animal"] = animal;
    out_json["results"] = results;

    ofstream output(argv[2]);
    output << out_json.dump(4);
    output.close();

    return 0;
}
