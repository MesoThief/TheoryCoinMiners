#include <fstream>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <chrono>              // for timing

#include "data/MatchSimK.h"
#include "utils/Alphabet.h"
#include "data/Shortlex.h"
#include "utils/Common.h"
#include "utils/CalculateUniversality.h"

using namespace std;

// A small struct to hold each result for printing later
struct Result {
    string sequence;
    int    key;
    int    exp_k;
    string best_orig, best_snf;
    int    best_count;
    double duration_ms;
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input-file>\n";
        return 1;
    }

    // 0) Read all inputs first
    ifstream in(argv[1]);
    string title;
    getline(in, title);
    cout << "Title: " << title << "\n\n";

    vector<pair<string,int>> inputs;
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        string seq;
        int k;
        if (iss >> seq >> k && seq.size() < 1000)
            inputs.emplace_back(move(seq), k);
    }

    // 1) Init alphabet once
    Alphabet &alpha = Alphabet::getInstance();
    alpha.setAlphabet("ATGC");

    // 2) Launch one async task per sequence
    int N = inputs.size();
    vector<future<Result>> futures;
    futures.reserve(N);

    for (auto &inp : inputs) {
        futures.emplace_back(async(launch::async, [&inp]() {
            const string& sequence = inp.first;
            int key = inp.second;
            int exp_k = calculateUniversalityIndex(sequence);

            // Cache to skip duplicate SNF builds (keyed by substring view)
            unordered_map<string_view, string> snf_cache;
            snf_cache.reserve(1024);

            Result R{sequence, key, exp_k, "", "", 0, 0.0};

            auto t0 = chrono::high_resolution_clock::now();

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
                        snf = it->second;
                    }

                    auto pos = MatchSimK::matchSimK(sequence, snf, exp_k);
                    int total = 0;
                    for (const auto &t : pos) {
                        const auto &iint = get<0>(t);
                        const auto &jint = get<1>(t);
                        total += (iint.end - iint.start + 1) *
                                 (jint.end - jint.start + 1);
                    }
                    if (total > R.best_count) {
                        R.best_count = total;
                        R.best_orig  = string(raw_view);
                        R.best_snf   = snf;
                    }
                }
            }

            auto t1 = chrono::high_resolution_clock::now();
            R.duration_ms = chrono::duration<double, milli>(t1 - t0).count();
            return R;
        }));
    }

    // 3) Collect & print in order
    for (int i = 0; i < N; ++i) {
        Result R = futures[i].get();
        cout << "Sequence " << (i+1) << ":\n"
             << "[" << R.sequence << "]\n"
             << "expected k: " << R.exp_k << "\n"
             << "Original substring:       " << R.best_orig  << "\n"
             << "Shortlex-normalized form: " << R.best_snf   << "\n"
             << "-> with " << R.best_count << " matches\n"
             << "Processing time: " << R.duration_ms << " ms\n"
             << "-> aligning with key: " << R.key << "\n\n";
    }

    return 0;
}
