#include <algorithm>
#include <deque>
#include <limits>
#include <set>
#include <string>
#include <vector>

using namespace std;

// Structure to hold the result: the shortlex normal form and its corresponding
// X- and Y-vectors.
struct ShortlexResult {
    string shortlexNormalForm;
    vector<int> X_vector;
    vector<int> Y_vector;

    deque<set<char>> stackForm;
    vector<int> arch_ends;
    set<char> alphabet;
    int universality;
};

// Testing Simon's congruence 논문 버전
string computeShortlexNormalForm(const string &w, int k);

// Simon's congruence pattern matching에서 필요한 버전
ShortlexResult
computePartialShortlexNormalForm(const string &w, vector<int> X_vector, vector<int> Y_vector, int threshold);
