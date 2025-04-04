#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <set>

using namespace std;

// Structure to hold the result: the shortlex normal form and its corresponding X- and Y-vectors.
struct ShortlexResult {
    string normalForm;
    vector<int> X; // X-coordinates for each letter in normalForm
    vector<int> Y; // Y-coordinates for each letter in normalForm

    vector<set<unsigned char>> s_p;
    int universality;
    set<unsigned char> alph;
    vector<int> arch;
};

ShortlexResult computeShortlexNormalForm(const string& w, int universality, int k);