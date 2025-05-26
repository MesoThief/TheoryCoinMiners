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
    json data = sequences[0];

    // 1) Init alphabet once
    Alphabet &alpha = Alphabet::getInstance();
    alpha.setAlphabet("ATGC");

    const string& sequence = data["sequence"];
    int exp_k = 15;

    // Cache to skip duplicate SNF builds (keyed by substring view)
    unordered_map<string_view, string> snf_cache;
    snf_cache.reserve(1024);

    json results = json::array();
    int n = (int)sequence.size();
    for (int i = 0; i < n; ++i) {
        for (int len = 1; i + len <= n; ++len) {
            // zero-copy substring view
            string_view raw_view(sequence.data() + i, size_t(len));

            // compute SNF once per unique raw substring
            auto it = snf_cache.find(raw_view);
            string snf;
            if (it == snf_cache.end()) {
                // materialize substring for SNF computation
                string raw_str(raw_view);
                snf = computeShortlexNormalForm(raw_str, exp_k);
                snf_cache.emplace(raw_view, snf);
            } else {
                continue;
                // snf = it->second;
            }

            json result = json::object();

            auto t0 = chrono::high_resolution_clock::now();
            auto pos = MatchSimK::matchSimK(sequence, snf, exp_k);
            auto t1 = chrono::high_resolution_clock::now();
            double duration = chrono::duration<double, milli>(t1 - t0).count();
            result["duration_ms"] = duration;
            int num_matches = 0;
            for (const auto &t : pos) {
                const auto &iint = get<0>(t);
                const auto &jint = get<1>(t);
                num_matches += (iint.end - iint.start + 1) *
                            (jint.end - jint.start + 1);
            }

            if (num_matches == 0) continue;

            result["pattern"] = snf;
            result["pattern_universality"] = calculateUniversalityIndex(snf);
            result["num_matches"] = num_matches;

            results.push_back(result);
        }
    }

    json out_json = json::object();
    out_json["animal"] = animal;
    out_json["sequence"] = sequence;
    out_json["class"] = data["class"];
    out_json["results"] = results;

    ofstream output(argv[2]);
    output << out_json.dump(4);

    output.close();

    return 0;
}
