#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "data/MatchSimK.h"
#include "utils/Alphabet.h"
#include "data/Shortlex.h"
#include "utils/Common.h"
#include "utils/CalculateUniversality.h"

using namespace std;

int main(int argc, char* argv[]) {
    // processing the experiment data from exp_data
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

    // get title (first line)
    string title;
    if (!getline(inputFile, title)) {
        cerr << "Input file is empty!" << endl;
        return 1;
    }
    cout << "Title: " << title << "\n\n";

    Alphabet::getInstance().setAlphabet("ATGC");
    Alphabet::getInstance().enableExtension(true);

    // Now read lines of "<word> <integer>" until EOF
    string line;
    // order of a sequence, key, expected-k, most-matching-substring, most-number-of-matches
    vector<tuple<string, int, int, string, int>> rep_match_data;

    while (getline(inputFile, line)) {
        if (line.empty())
            continue; // skip blank lines

        istringstream iss(line);
        string sequence;
        int key;

        if (!(iss >> sequence >> key)) {
            cerr << "Warning: malformed line skipped: \""
                 << line << "\"" << endl;
            continue;
        }

        // expected arch length w.r.t. probability
        // length = 15 <- 95%
//        int exp_arch_length = 15;

        // length = 20 <- 99%
        int exp_arch_length = 20;

        // according to that value, calculate the expected number of archs, and set k
        int exp_k = (int) sequence.size() / exp_arch_length;
//
//        int real_arch_num = calculateUniversalityIndex(sequence);
//        cout << "real_arch_num: " << real_arch_num << endl;
//        cout << endl;

        pair<string, int> most_matching_substr("", 0);

        // pattern mining: given k, find the most frequent ~k matching pattern in a given text
        // testing every substring to the matchSimK algorithm
        for (int start = 0; start < sequence.size(); ++start) {
            for (int end = start + 1; end <= sequence.size(); ++end) {
                string substr = sequence.substr(start, end - start);
                string shortlexSubstr = computeShortlexNormalForm(substr, exp_k);

                // get matching positions and compute the number of total matches
                vector<MatchSimK::triple> positions = MatchSimK::matchSimK(sequence, shortlexSubstr, exp_k);
                int num_of_matches = 0;
                for (MatchSimK::triple position : positions) {
                    num_of_matches +=
                        (get<0>(position).end - get<0>(position).start + 1) *
                        (get<1>(position).end - get<1>(position).start + 1);
                }

                // update the most-matches
                if(num_of_matches > most_matching_substr.second) {
                    most_matching_substr.first = substr;
                    most_matching_substr.second = num_of_matches;
                }
            }
        }

        // determine the representative substring match and save
        // determine the representative substring match and save
        rep_match_data.emplace_back(sequence, key, exp_k, most_matching_substr.first, most_matching_substr.second);

        // print result immediately
        cout << "Sequence " << rep_match_data.size() << ":" << endl;
        cout << "[" << sequence << "]" << endl;
        cout << "expected k: " << exp_k << endl;
        cout << "Most frequently matching substring: " << most_matching_substr.first << endl;
        cout << "-> with " << most_matching_substr.second << " matches" << endl;
        cout << "-> aligning with key: " << key << endl;
        cout << endl;

    }

    // print out result
//    int counter = 1;
//    for(tuple<string, int, int, string, int> match_data : rep_match_data) {
//        cout << "Sequence " << counter << ":" << endl;
//        cout << "[" << get<0>(match_data) << "]" << endl;
//        cout << "expected k: " << get<2>(match_data) << endl;
//        cout << "Most frequently matching substring: " << get<3>(match_data) << endl;
//        cout << "-> with " << get<4>(match_data) << " matches" << endl;
//        cout << "-> aligning with key: " << get<1>(match_data) << endl;
//        cout << endl;
//        counter++;
//    }

    return 0;
}
