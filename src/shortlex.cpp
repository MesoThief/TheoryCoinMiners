#include <iostream>
#include <fstream>
#include <sstream>

#include "data/Shortlex.h"
#include "utils/Alphabet.h"
#include "utils/CalculateUniversality.h"

// ------------------
// A simple test driver for Shortlex.cpp
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

    // read inputs
    string alphabet;
    string XYorYX;  // is input string w XY-link or YX-link?
    string w;
    vector<int> X_vector;
    vector<int> Y_vector;
    int k;
    
    string line;

    getline(inputFile, alphabet);
    Alphabet::getInstance().setAlphabet(alphabet);

    getline(inputFile, XYorYX);

    getline(inputFile, w);

    getline(inputFile, line);
    istringstream x_vec_stream(line);
    int num;
    while (x_vec_stream >> num) {
        X_vector.push_back(num);
    }

    getline(inputFile, line);
    istringstream y_vec_stream(line);
    while (y_vec_stream >> num) {
        Y_vector.push_back(num);
    }

    getline(inputFile, line);
    istringstream k_stream(line);
    k_stream >> k;

    int universality_index = calculateUniversalityIndex(w);
    int threshold;
    if (XYorYX == "XY") {
        threshold = k + 2 - universality_index;
    } else {
        threshold = k + 1 - universality_index;
    }

    cout << "Input:" << endl;
    cout << "   w: " << w << endl;
    cout << "   X_vector size: " << X_vector.capacity() << endl;
    cout << "   X_vector: ";
    for (int x : X_vector) {
        cout << x << " ";
    }
    cout << endl;
    cout << "   Y_vector size: " << Y_vector.capacity() << endl;
    cout << "   Y_vector: ";
    for (int y : Y_vector) {
        cout << y << " ";
    }
    cout << endl;
    cout << "   k: " << k << endl;
    cout << "   universality_index: " << universality_index << endl;

    // run test
    ShortlexResult result = computePartialShortlexNormalForm(
        w,
        X_vector,
        Y_vector,
        threshold
    );

    cout << "Output: " << endl;
    cout << "   Shortlex normal form: " << result.shortlexNormalForm << endl;
    cout << "   X_vector size: " << result.X_vector.capacity() << endl;
    cout << "   new X-vector: ";
    for (int x : result.X_vector) {
        cout << x << " ";
    }
    cout << endl;
    cout << "   Y_vector size: " << result.Y_vector.capacity() << endl;
    cout << "   new Y-vector: ";
    for (int y : result.Y_vector) {
        cout << y << " ";
    }
    cout << endl;

    cout << "   shortlex universality: " << result.universality << endl;
    cout << "Testing Simon's congruence 논문 버전 SNF:" << computeShortlexNormalForm(w, k) << endl;

    return 0;
}