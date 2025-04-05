#include <iostream>

#include "data/Shortlex.h"
#include "utils/Alphabet.h"
#include "utils/CalculateUniversality.h"

// ------------------
// A simple test driver for Shortlex.cpp
// ------------------
int main() {
    
    string w = "bacbaabada"; // assume alphabet = { a, b, c, d } in this example
    vector<int> X_vector(Alphabet::getInstance().size(), 1);
    vector<int> Y_vector(Alphabet::getInstance().size(), 1);
    int universality_index = calculateUniversalityIndex(w, 4);
    int k = 3;

    cout << "Input:" << endl;
    cout << "   w: " << w << endl;
    cout << "   X_vector: ";
    for (int x : X_vector) {
        cout << x << " ";
    }
    cout << endl;
    cout << "   Y_vector: ";
    for (int y : Y_vector) {
        cout << y << " ";
    }
    cout << endl;
    cout << "   universality_index: " << universality_index << endl;
    cout << "   k: " << k << endl;

    ShortlexResult result = computePartialShortlexNormalForm(
        w,
        X_vector,
        Y_vector,
        k + 2 - universality_index
    );

    cout << endl << "Output: " << endl;
    cout << "   Shortlex normal form: " << result.normalForm << endl;
    cout << "   new X-vector: ";
    for (int x : result.X) {
        cout << x << " ";
    }
    cout << endl;
    cout << "   new Y-vector: ";
    for (int y : result.Y) {
        cout << y << " ";
    }
    cout << endl;

    return 0;
}