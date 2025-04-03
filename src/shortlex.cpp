#include <iostream>

#include "data/Shortlex.h"

// ------------------
// A simple test driver for Shortlex.cpp
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