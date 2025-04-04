#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>
#include <set>
#include <unordered_map>

using namespace std;

// Structure to hold the result: the shortlex normal form and its corresponding X- and Y-vectors.
struct ShortlexResult {
    string normalForm;
    vector<int> X; // X-coordinates for each letter in normalForm
    vector<int> Y; // Y-coordinates for each letter in normalForm
};

ShortlexResult computeShortlexFromVectors(const string& w, const vector<int>& X, const vector<int>& Y, int k) {
    int n = static_cast<int>(w.size());
    string v = w; // make a modifiable copy

    // --- Phase 2: Lexicographically reorder blocks with same (X, Y) and X+Y == k+1 ---
    int start = 0;
    while (start < n) {
        int end = start + 1;
        while (end < n &&
               X[end] == X[start] &&
               Y[end] == Y[start] &&
               (X[start] + Y[start] == k + 1)) {
            end++;
        }
        if (end - start > 1) {
            sort(v.begin() + start, v.begin() + end);
        }
        start = end;
    }

    ShortlexResult result;
    result.normalForm = v;
    result.X = X;
    result.Y = Y;
    return result;
}

// -------------------- Main --------------------
int main() {
    string w;
    int k;
    cout << "==== ShortLex Normalization (Σ-indexed Vector Mode) ====" << endl;
    cout << "Enter the value of input string w: ";
    cin >> w;
    cout << "Enter the value of k: ";
    cin >> k;

    // Build alphabet set Σ
    set<char> alphabetSet(w.begin(), w.end());
    vector<char> alphabet(alphabetSet.begin(), alphabetSet.end());
    int sigmaSize = alphabet.size();

    // Map each character to an index
    unordered_map<char, int> charToIndex;
    for (int i = 0; i < sigmaSize; ++i) {
        charToIndex[alphabet[i]] = i;
    }

    int n = w.size();
    vector<vector<int>> Xvec(n, vector<int>(sigmaSize));
    vector<vector<int>> Yvec(n, vector<int>(sigmaSize));

    cout << "Enter the Σ-indexed X-vectors (one line per position):\n";
    for (int i = 0; i < n; ++i) {
        cout << "X[" << i << "] (" << sigmaSize << " values): ";
        for (int j = 0; j < sigmaSize; ++j) {
            cin >> Xvec[i][j];
        }
    }

    cout << "Enter the Σ-indexed Y-vectors (one line per position):\n";
    for (int i = 0; i < n; ++i) {
        cout << "Y[" << i << "] (" << sigmaSize << " values): ";
        for (int j = 0; j < sigmaSize; ++j) {
            cin >> Yvec[i][j];
        }
    }

    // Convert Σ-indexed vectors into scalar X[i] and Y[i]
    vector<int> X(n), Y(n);
    for (int i = 0; i < n; ++i) {
        char c = w[i];
        int idx = charToIndex[c];
        X[i] = Xvec[i][idx];
        Y[i] = Yvec[i][idx];
    }

    // Call regular ShortLex sorter
    ShortlexResult result = computeShortlexFromVectors(w, X, Y, k);

    cout << "\nShortlex normal form: " << result.normalForm << endl;
    cout << "X-coordinates: ";
    for (int x : result.X) cout << x << " ";
    cout << "\nY-coordinates: ";
    for (int y : result.Y) cout << y << " ";
    cout << endl;

    return 0;
}