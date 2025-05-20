#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>

#include "data/MatchSimK.h"
#include "data/Shortlex.h"
#include "utils/Alphabet.h"
#include "utils/Common.h"
#include "utils/RandomTextGenerator.h"

using namespace std;

// ------------------
// Running Time Analysis
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
    int k;
    vector<int> text_len_vec;

    getline(inputFile, alphabet);
    Alphabet::getInstance().setAlphabet(alphabet);

    string k_line;
    getline(inputFile, k_line);
    istringstream k_stream(k_line);
    k_stream >> k;

    cout << "k: " << k << endl;

    string text_len_line;
    int text_len;
    getline(inputFile, text_len_line);
    istringstream t_stream(text_len_line);

    cout << "|T|: ";
    while (t_stream >> text_len) {
       text_len_vec.push_back(text_len);
       cout << text_len << " ";
    }
    cout << endl;

    if (t_stream.eof()) {
        debug(cout << "Read total of " << text_len_vec.size() << " experiment requests" << endl);
    } else if (t_stream.fail()) {
        cerr << "Error occurred while reading experiment requests" << endl;
        return 1;
    }

    if (text_len_vec.size() == 0) {
        cerr << "No experiment request was read. Strange!" << endl;
        return 1;
    }

    // Testing Part
    string text, subseq, shortlex_subseq, best_sl;
    unordered_map<string, int> sl_cnt_map;
    unordered_map<string, int>::const_iterator sl_find;
    int p_count, best_count;
    for(int tl : text_len_vec) {
        cout << endl << "======== Experiment for Text Length [ " << tl << " ] ========" << endl;
        text = generateRandomText(tl);
        cout << "Generated Text: " << text << endl;
        sl_cnt_map.clear();
        cout << "Timing Started." << endl;

        // Test for all eligible subseq. Timing start.
        auto s_t = chrono::high_resolution_clock::now();
        for (int s = 0; s < tl; s++) {
            for (int n = 1; n <= tl - s; n++) {
                subseq = text.substr(s, n);
                Alphabet::getInstance().setAlphabet(alphabet);
                shortlex_subseq = computeShortlexNormalForm(subseq, k);

                // This operation has const. time complexity unless being sooooooooooooooooo unlucky
                if (sl_cnt_map.count(shortlex_subseq) != 0) continue;

                p_count = 0;
                vector<MatchSimK::triple> positions = MatchSimK::matchSimK(text, shortlex_subseq, k);
                for (auto pos : positions) {
                    p_count += (get<0>(pos).end - get<0>(pos).start + 1) * (get<1>(pos).end - get<1>(pos).start + 1);
                }

                // Store result
                sl_cnt_map.insert({{shortlex_subseq, p_count}});
                cout << "Calculate (SL, count): (" << shortlex_subseq << ", " << p_count << ")" << endl;
            }
        }

        // Find Max
        best_count = 0;
        for (auto it = sl_cnt_map.begin(); it != sl_cnt_map.end(); it++){
            if (it->second > best_count) {
                best_count = it->second;
                best_sl = it->first;
            }
        }

        auto f_t = chrono::high_resolution_clock::now();
        cout << "Timing Ended." << endl;
        cout << "Found Best Matching Pattern P=" << best_sl << " with Matching Count=" << best_count << endl;
        auto duration = chrono::duration_cast<chrono::microseconds>(f_t - s_t);
        cout << "The Experiment Took: " << duration.count() << " microseconds." << endl;
    }

    return 0;
}