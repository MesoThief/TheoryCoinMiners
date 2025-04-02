#include <iostream>
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

// Computes the shortlex normal form of w (under Simon's congruence for parameter k)
// using a two-phase procedure:
//   Phase 1: Compute X-coordinates (left-to-right) and Y-coordinates (right-to-left) and mark letters for deletion 
//            when X[i] + Y[i] > k+1. Then form the reduced word.
//   Phase 2: For each maximal block of letters having the same attribute (X, Y) with X+Y==k+1, sort that block lexicographically.
ShortlexResult computeShortlexNormalForm(const string& w, int universality, int k) {
    // For simplicity, we ignore universality (which in a full implementation may be used
    // to immediately return w if k is "large" relative to |w|).
    int n = static_cast<int>(w.size());
    vector<int> X(n, 0), Y(n, 0);
    const int ALPHABET_SIZE = 256;
    vector<int> counter(ALPHABET_SIZE, 1); // counters for X computation

    // --- Phase 1a: Compute X-coordinates from left-to-right ---
    // For each position, X[i] is set to the current counter for letter w[i] (then update all counters)
    for (int i = 0; i < n; i++) {
        unsigned char c = static_cast<unsigned char>(w[i]);
        X[i] = counter[c];
        counter[c]++;
        int current = counter[c];
        // Update all counters: each counter becomes the minimum of itself and counter[c]
        for (int a = 0; a < ALPHABET_SIZE; a++) {
            if (counter[a] > current) {
                counter[a] = current;
            }
        }
    }

    // --- Phase 1b: Compute Y-coordinates from right-to-left and mark positions for deletion ---
    fill(counter.begin(), counter.end(), 1); // reset counters for Y
    vector<bool> keep(n, true);
    for (int i = n - 1; i >= 0; i--) {
        unsigned char c = static_cast<unsigned char>(w[i]);
        // If the sum X[i] + current counter for c is at most k+1, then we keep the letter.
        if (X[i] + counter[c] <= k + 1) {
            Y[i] = counter[c];
            counter[c]++;
            int current = counter[c];
            for (int a = 0; a < ALPHABET_SIZE; a++) {
                if (counter[a] > current) {
                    counter[a] = current;
                }
            }
        } else {
            // Otherwise, mark this position for deletion.
            keep[i] = false;
        }
    }

    // Build the reduced word v from kept positions.
    string v;
    for (int i = 0; i < n; i++) {
        if (keep[i])
            v.push_back(w[i]);
    }

    // For simplicity, recompute the X- and Y-coordinates on the new word v.
    int m = static_cast<int>(v.size());
    vector<int> newX(m, 0), newY(m, 0);
    fill(counter.begin(), counter.end(), 1);
    for (int i = 0; i < m; i++) {
        unsigned char c = static_cast<unsigned char>(v[i]);
        newX[i] = counter[c];
        counter[c]++;
        int current = counter[c];
        for (int a = 0; a < ALPHABET_SIZE; a++) {
            if (counter[a] > current) {
                counter[a] = current;
            }
        }
    }
    fill(counter.begin(), counter.end(), 1);
    for (int i = m - 1; i >= 0; i--) {
        unsigned char c = static_cast<unsigned char>(v[i]);
        // Here, since v is already minimal (no deletion is possible),
        // we expect newX[i] + counter[c] <= k+1.
        newY[i] = counter[c];
        counter[c]++;
        int current = counter[c];
        for (int a = 0; a < ALPHABET_SIZE; a++) {
            if (counter[a] > current) {
                counter[a] = current;
            }
        }
    }

    // --- Phase 2: Lexicographically reorder blocks ---
    // For each maximal contiguous block in v where the (newX, newY) pair is the same and the sum equals k+1,
    // sort that block in increasing order.
    int start = 0;
    while (start < m) {
        int end = start + 1;
        // Continue while the attributes are identical and their sum equals k+1.
        while (end < m && newX[end] == newX[start] && newY[end] == newY[start] && (newX[start] + newY[start] == k + 1)) {
            end++;
        }
        if (end - start > 1) {
            // Sort the substring [start, end) of v in lexicographical order.
            sort(v.begin() + start, v.begin() + end);
        }
        start = end;
    }

    // Prepare the result.
    ShortlexResult result;
    result.normalForm = v;
    result.X = newX;
    result.Y = newY;
    return result;
}

// ------------------
// A simple test driver
// ------------------
int main() {
    string w;
    int universality;
    int k;
    cout << "Enter the value of input string w: ";
    cin >> w;
    cout << "Enter the value of universality index: ";
    cin >> universality;
    cout << "Enter the value of k: ";
    cin >> k;
    
    ShortlexResult result = computeShortlexNormalForm(w, universality, k);

    cout << "Shortlex normal form: " << result.normalForm << endl;
    cout << "X-coordinates: ";
    for (int x : result.X) {
        cout << x << " ";
    }
    cout << endl;
    cout << "Y-coordinates: ";
    for (int y : result.Y) {
        cout << y << " ";
    }
    cout << endl;

    return 0;
}
