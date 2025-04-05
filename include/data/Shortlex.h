#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <set>

using namespace std;

// Structure to hold the result: the shortlex normal form and its corresponding X- and Y-vectors.
struct ShortlexResult {
    string normalForm;
    vector<int> X;
    vector<int> Y;

    vector<set<unsigned char>> s_p;
    int universality;
    set<unsigned char> alph;
    vector<int> arch;
};

// Testing Simon's congruence 논문 버전
ShortlexResult computeShortlexNormalForm(
    const string& w,
    int universality,
    int k
);

// Simon's congruence pattern matching에서 필요한 버전
ShortlexResult computePartialShortlexNormalForm(
    const string& w,
    vector<int> X_vector,
    vector<int> Y_vector,
    int threshold
);
