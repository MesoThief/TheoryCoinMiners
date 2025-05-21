#include <fstream>
#include <future>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "data/MatchSimK.h"
#include "utils/Alphabet.h"
#include "data/Shortlex.h"
#include "utils/Common.h"

using namespace std;

// A small struct to hold each result for printing later
struct Result {
    string sequence;
    int    key;
    int    exp_k;
    string best_orig, best_snf;
    int    best_count;
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
        string seq; int k;
        if (iss >> seq >> k)
            inputs.emplace_back(move(seq), k);
    }

    // 1) Init alphabet once
    Alphabet &alpha = Alphabet::getInstance();
    alpha.setAlphabet("ATGC");
    alpha.enableExtension(false);  // strict mode

    // 2) Launch one async task per sequence
    int N = inputs.size();
    vector<future<Result>> futures;
    futures.reserve(N);

    for (auto &inp : inputs) {
        futures.push_back(async(launch::async, [&inp]() {
            const auto &sequence = inp.first;
            int key = inp.second;
            int exp_k = sequence.size() / 20;

            // Cache to skip duplicate SNF builds
            unordered_map<string, string> snf_cache;
            snf_cache.reserve(1024);

            Result R{ sequence, key, exp_k, "", "", 0 };

            // for each substring
            for (int i = 0; i < (int)sequence.size(); ++i) {
                for (int j = i+1; j <= (int)sequence.size(); ++j) {
                    string raw = sequence.substr(i, j-i);

                    // compute SNF once per unique raw
                    auto it = snf_cache.find(raw);
                    string snf;
                    if (it == snf_cache.end()) {
                        snf = computeShortlexNormalForm(raw, exp_k);
                        snf_cache.emplace(raw, snf);
                    } else {
                        snf = it->second;
                    }

                    auto pos = MatchSimK::matchSimK(sequence, snf, exp_k);
                    int total = 0;
                    for (auto &t : pos) {
                        total +=
                            (get<0>(t).end - get<0>(t).start + 1)
                            * (get<1>(t).end - get<1>(t).start + 1);
                    }
                    if (total > R.best_count) {
                        R.best_count  = total;
                        R.best_orig   = move(raw);
                        R.best_snf    = move(snf);
                    }
                }
            }
            return R;
        }));
    }

    // 3) Collect & print in order
    for (int i = 0; i < N; ++i) {
        Result R = futures[i].get();
        cout << "Sequence " << (i+1) << ":\n";
        cout << "[" << R.sequence << "]\n";
        cout << "expected k: " << R.exp_k << "\n";
        cout << "Original substring:        " << R.best_orig << "\n";
        cout << "Shortlex-normalized form:  " << R.best_snf  << "\n";
        cout << "-> with " << R.best_count << " matches\n";
        cout << "-> aligning with key: " << R.key << "\n\n";
    }
}
