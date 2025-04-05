#include <deque>

#include "data/Shortlex.h"
#include "utils/Alphabet.h"

using namespace std;

/**
   * @brief Computes the shortlex normal form of w (under Simon's congruence for parameter k)
   * 
   * Testing Simon's congruence 논문 버전 SNF 계산 알고리즘
  */
string computeShortlexNormalForm(const string& w, int k) {
    int n = w.size();

    vector<int> X(n, 0);    // X-coordinates
    vector<int> Y(n, 0);    // Y-coordinates

    int ALPHABET_SIZE = Alphabet::getInstance().size();
    
    string shortlexNormalForm;
    deque<int> shortlexNormalX;    // X-coordinates of SNF
    deque<int> shortlexNormalY;    // Y-coordinates of SNF
    
    // 1. Compute X-coordinates from left-to-right
    vector<int> counter(ALPHABET_SIZE, 1);
    for (int i = 0; i < n; i++) {
        char c = w[i];
        int alphabetIndex = Alphabet::getInstance().charToIndex(c);

        X[i] = counter[alphabetIndex];
        counter[alphabetIndex]++;
        
        for (int a = 0; a < ALPHABET_SIZE; a++) {
            counter[a] = min(counter[a], counter[alphabetIndex]);
        }
    }

    // 2. Compute Y-coordinates from right-to-left, while computing SNF and its coordinates
    fill(counter.begin(), counter.end(), 1); // reset counters for Y
    for (int i = n - 1; i >= 0; i--) {
        char c = w[i];
        int alphabetIndex = Alphabet::getInstance().charToIndex(c);

        // If X[i] + Y[i] is at most k+1, then we keep the letter.
        if (X[i] + counter[alphabetIndex] <= k + 1) {
            Y[i] = counter[alphabetIndex];
            counter[alphabetIndex]++;

            for (int a = 0; a < ALPHABET_SIZE; a++) {
                counter[a] = min(counter[a], counter[alphabetIndex]);
            }

            shortlexNormalForm.insert(shortlexNormalForm.begin(), c);
            shortlexNormalX.push_front(X[i]);
            shortlexNormalY.push_front(Y[i]);
        } // otherwise, do not include the letter in SNF.
    }

    // 3. Re-compute X-coordinates based on SNF
    int m = shortlexNormalForm.size();
    fill(counter.begin(), counter.end(), 1);
    for (int i = 0; i < m; i++) {
        char c = shortlexNormalForm[i];
        int alphabetIndex = Alphabet::getInstance().charToIndex(c);

        shortlexNormalX.push_back(counter[alphabetIndex]);
        counter[alphabetIndex]++;
        
        for (int a = 0; a < ALPHABET_SIZE; a++) {
            counter[a] = min(counter[a], counter[alphabetIndex]);
        }
    }

    // 4. Lexicographically reorder blocks
    // block = elements whose coordinate (X,Y) are the same
    int start = 0;
    while (start < m) {
        int end = start + 1;
        while (end < m &&
            shortlexNormalX[end] == shortlexNormalX[start] &&
            shortlexNormalY[end] == shortlexNormalY[start] &&
            (shortlexNormalX[start] + shortlexNormalY[start] == k + 1)
        ) {
            end++;
        }
        if (end - start > 1) {
            // TODO: replace to custom sorting algorithm --- ex) counting sort
            sort(shortlexNormalForm.begin() + start, shortlexNormalForm.begin() + end);
        }

        start = end;
    }

    return shortlexNormalForm;
}


/**
   * @brief Computes partial shortlex normal form of a substring w.
   * @param w          a substring of text T
   * @param X_vector   X-vector of T at start of w
   * @param Y_vector   Y-vector of T at end of w
   * @param threshold  if x + y > threashold, delete character.
   * 
   * The value of threshold depends on whether w is a XY-link or YX-link.
   * XY-link: threshold = (k + 1 -(iota(p) - 1)) = k + 2 - iota(p)
   * YX-link: threshold = k + 1 - iota(p)
   * 
   * Simon's congruence pattern matching 논문에서 필요한 SNF 계산 알고리즘
  */
ShortlexResult computePartialShortlexNormalForm(
    const string& w,
    vector<int> X_vector,
    vector<int> Y_vector,
    int threshold
) {
    int n = w.size();
    int ALPHABET_SIZE = Alphabet::getInstance().size();
    set<char> w_alphabet; // for detecting archs

    vector<int> X(n, 0);    // X-coordinates
    vector<int> Y(n, 0);    // Y-coordinates

    string shortlexNormalForm;  
    deque<int> shortlexNormalX; // X-coordinates of normal form
    deque<int> shortlexNormalY; // Y-coordinates of normal form

    vector<int> new_X_vector = X_vector;   // X-vector which will be updated by iterating normal form
    vector<int> new_Y_vector = Y_vector;   // Y-vector which will be updated by iterating normal form

    // 1. Compute X-coordinates
    for (int i = 0; i < n; i++) {
        char c = w[i];
        int alphabet_index = Alphabet::getInstance().charToIndex(c);

        X[i] = X_vector[alphabet_index];
        X_vector[alphabet_index]++;

        for (int j = 0; j < ALPHABET_SIZE; j++) {
            X_vector[j] = min(X_vector[j], X_vector[alphabet_index]);
        }

        // for detecting archs, compute alph(w)
        w_alphabet.insert(c);
    }

    // 2. Compute Y-coordinates and normal form
    for (int i = n - 1; i >= 0; i--) {
        char c = w[i];
        int alphabet_index = Alphabet::getInstance().charToIndex(c);

        Y[i] = Y_vector[alphabet_index];

        if (X[i] + Y[i] <= threshold) {
            Y_vector[alphabet_index]++;

            for (int j = 0; j < ALPHABET_SIZE; j++) {
                Y_vector[j] = min(Y_vector[j], Y_vector[alphabet_index]);
            }

            shortlexNormalForm.insert(shortlexNormalForm.begin(), c);
            shortlexNormalY.push_front(Y[i]);
        }
    }

    // 3. Compute new X-vector based on normal form (and also recompute SNF's X-coordinates)
    int m = shortlexNormalForm.size();
    for (int i = 0; i < m; i++) {
        char c = shortlexNormalForm[i];
        int alphabet_index = Alphabet::getInstance().charToIndex(c);

        shortlexNormalX.push_back(new_X_vector[alphabet_index]);
        new_X_vector[alphabet_index]++;

        for (int j = 0; j < ALPHABET_SIZE; j++) {
            new_X_vector[j] = min(new_X_vector[j], new_X_vector[alphabet_index]);
        }
    }
    // Y-vector is already updated based on normal form, so don't re-compute.
    new_Y_vector = Y_vector;

    // 4. Lexicographically reorder blocks
    // block = elements whose coordinate (X,Y) are the same
    // also, compute the stack form of SNF.
    int start = 0;
    deque<set<char>> stackForm;
    set<char> alphabet_track;
    vector<int> arch_ends;
    while (start < m) {
        // sort block
        int end = start + 1;
        while (end < m &&
            shortlexNormalX[end] == shortlexNormalX[start] &&
            shortlexNormalY[end] == shortlexNormalY[start] &&
            (shortlexNormalX[start] + shortlexNormalY[start] == threshold)
        ) {
            end++;
        }
        if (end - start > 1) {
            // TODO: replace to custom sorting algorithm --- ex) counting sort
            sort(shortlexNormalForm.begin() + start, shortlexNormalForm.begin() + end);
        }

        // push block to stack form
        // Note that we iterate from the beginning of normal form, so we push to bottom of stack form
        set<char> block_charset;
        for (int i = start; i < end; i++) {
            char c = shortlexNormalForm[i];
            block_charset.insert(c);
            alphabet_track.insert(c);
        }
        stackForm.push_front(block_charset);

        // detect arch ends
        if (block_charset.size() == w_alphabet.size()) {
            arch_ends.push_back(end);
            alphabet_track.clear();
        }

        start = end;
    }

    ShortlexResult result;
    result.shortlexNormalForm = shortlexNormalForm;
    result.X_vector = new_X_vector;
    result.Y_vector = new_Y_vector;
    result.stackForm = stackForm;
    result.arch_ends = arch_ends;
    result.alphabet = w_alphabet;
    result.universality = arch_ends.size();
    return result;
}
