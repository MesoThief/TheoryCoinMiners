#include <deque>

#include "data/Shortlex.h"
#include "utils/Alphabet.h"

using namespace std;

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
    set<char> alph; // (minimal) \Sigma, yes.

    // --- Phase 1a: Compute X-coordinates from left-to-right ---
    // For each position, X[i] is set to the current counter for letter w[i] (then update all counters)
    for (int i = 0; i < n; i++) {
        unsigned char c = static_cast<unsigned char>(w[i]);
        X[i] = counter[c]; alph.insert(c);
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
    int start = 0; int size_alph = alph.size();
    vector<set<unsigned char>> s_p; set<unsigned char> s; set<unsigned char> alph_track;
    vector<int> arch;
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

        // Update s_p
        s.clear();
        for(int a = start; a < end; a++) {
            s.insert(static_cast<unsigned char>(v[a]));
            alph_track.insert(static_cast<unsigned char>(v[a]));
        }
        s_p.insert(s_p.begin(), s);
        if(size_alph == alph_track.size()){ // Detect arch
            alph_track.clear();
            arch.push_back(end);
        }
        start = end;
    }

    // Prepare the result.
    ShortlexResult result;
    result.normalForm = v;
    result.X = newX;
    result.Y = newY;
    result.universality = universality;
    result.alphabet = alph;
    result.arch_ends = arch;
    return result;
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

    vector<int> X(n, 0);
    vector<int> Y(n, 0);

    string normalForm;  
    deque<int> normalX; // X-coordinates of normal form
    deque<int> normalY; // Y-coordinates of normal form

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
                Y_vector[j] = min(Y_vector[j], X_vector[alphabet_index]);
            }

            normalForm.insert(normalForm.begin(), c);
            normalX.push_front(X[i]);
            normalY.push_front(Y[i]);
        }
    }

    // 3. Lexicographically reorder blocks
    // block = elements whose coordinate (X,Y) are the same
    // also, compute the stack form of SNF.
    int m = normalForm.size();
    int start = 0;
    deque<set<char>> stackForm;
    set<char> alphabet_track;
    vector<int> arch_ends;
    while (start < m) {
        // sort block
        int end = start + 1;
        while (end < m &&
            normalX[end] == normalX[start] &&
            normalY[end] == normalY[start] &&
            (normalX[start] + normalY[start] == threshold)
        ) {
            end++;
        }
        if (end - start > 1) {
            sort(normalForm.begin() + start, normalForm.begin() + end);
        }

        // push block to stack form
        // Note that we iterate from the beginning of normal form, so we push to bottom of stack form
        set<char> block_charset;
        for (int i = start; i < end; i++) {
            char c = normalForm[i];
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

    // 4. Compute new X-vector for the end of normal form
    for (int i = 0; i < m; i++) {
        char c = normalForm[i];
        int alphabet_index = Alphabet::getInstance().charToIndex(c);

        new_X_vector[alphabet_index]++;

        for (int j = 0; j < ALPHABET_SIZE; j++) {
            new_X_vector[j] = min(new_X_vector[j], new_X_vector[alphabet_index]);
        }
    }

    // 5. Compute new Y-vector for the front of normal form
    for (int i = m - 1; i >= 0; i--) {
        char c = normalForm[i];

        int alphabet_index = Alphabet::getInstance().charToIndex(c);

        new_Y_vector[alphabet_index]++;

        for (int j = 0; j < ALPHABET_SIZE; j++) {
            new_Y_vector[j] = min(new_Y_vector[j], new_Y_vector[alphabet_index]);
        }
    }

    ShortlexResult result;
    result.normalForm = normalForm;
    result.X = new_X_vector;
    result.Y = new_Y_vector;
    result.stackForm = stackForm;
    result.arch_ends = arch_ends;
    result.alphabet = w_alphabet;
    result.universality = arch_ends.size();
    return result;
}
