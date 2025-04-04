#include <string>
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

// Structure to hold the result: the shortlex normal form and its corresponding X- and Y-vectors.
struct ShortlexResult {
    string normalForm;
    vector<int> X; // X-coordinates for each letter in normalForm
    vector<int> Y; // Y-coordinates for each letter in normalForm
};

ShortlexResult computeShortlexNormalForm(const string& w, int universality, int k);