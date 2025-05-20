#include "data/Shortlex.h"

#include <deque>

#include "utils/Alphabet.h"

using namespace std;

/**
   * @brief Computes the shortlex normal form of w (under Simon's congruence for parameter k)
   * 
   * Testing Simon's congruence 논문 버전 SNF 계산 알고리즘
  */
string computeShortlexNormalForm(const string& w, int k) {
    int n = w.size();
    vector<int> X(n, 0), Y(n, 0);

    // Prepare for dynamic alphabet size
    int currAlphaSize = Alphabet::getInstance().size();
    vector<int> counter(currAlphaSize, 1);

    string shortlexNormalForm;
    deque<int> shortlexNormalX;
    deque<int> shortlexNormalY;

    // 1. Compute X-coordinates (left-to-right)
    for (int i = 0; i < n; ++i) {
        char c = w[i];
        int alphabetIndex = Alphabet::getInstance().charToIndex(c);

        // Resize counter if alphabet grew
        currAlphaSize = Alphabet::getInstance().size();
        if ((int)counter.size() < currAlphaSize)
            counter.resize(currAlphaSize, 1);

        X[i] = counter[alphabetIndex];
        counter[alphabetIndex]++;

        for (int a = 0; a < currAlphaSize; ++a) {
            counter[a] = min(counter[a], counter[alphabetIndex]);
        }
    }

    // 2. Compute Y-coordinates (right-to-left) and build SNF
    // reset counter to 1
    fill(counter.begin(), counter.end(), 1);
    for (int i = n - 1; i >= 0; --i) {
        char c = w[i];
        int alphabetIndex = Alphabet::getInstance().charToIndex(c);

        // Resize if needed
        currAlphaSize = Alphabet::getInstance().size();
        if ((int)counter.size() < currAlphaSize)
            counter.resize(currAlphaSize, 1);

        int yVal = counter[alphabetIndex];
        if (X[i] + yVal <= k + 1) {
            Y[i] = yVal;
            counter[alphabetIndex]++;

            for (int a = 0; a < currAlphaSize; ++a) {
                counter[a] = min(counter[a], counter[alphabetIndex]);
            }

            // prepend to SNF and its coordinate deques
            shortlexNormalForm.insert(shortlexNormalForm.begin(), c);
            shortlexNormalX.push_front(X[i]);
            shortlexNormalY.push_front(Y[i]);
        }
    }

    // 3. Re-compute X-coordinates on the SNF string
    int m = shortlexNormalForm.size();
    fill(counter.begin(), counter.end(), 1);
    for (int i = 0; i < m; ++i) {
        char c = shortlexNormalForm[i];
        int alphabetIndex = Alphabet::getInstance().charToIndex(c);

        // Resize if needed
        currAlphaSize = Alphabet::getInstance().size();
        if ((int)counter.size() < currAlphaSize)
            counter.resize(currAlphaSize, 1);

        shortlexNormalX.push_back(counter[alphabetIndex]);
        counter[alphabetIndex]++;

        for (int a = 0; a < currAlphaSize; ++a) {
            counter[a] = min(counter[a], counter[alphabetIndex]);
        }
    }

    // 4. Lexicographically reorder same-(X,Y) blocks
    int start = 0;
    while (start < m) {
        int end = start + 1;
        while (end < m
            && shortlexNormalX[end] == shortlexNormalX[start]
            && shortlexNormalY[end] == shortlexNormalY[start]
            && (shortlexNormalX[start] + shortlexNormalY[start] == k + 1)) {
            ++end;
        }
        if (end - start > 1) {
            sort(shortlexNormalForm.begin() + start,
                shortlexNormalForm.begin() + end);
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
   * @param threshold  if x + y > threshold, delete character.
   * 
   * The value of threshold depends on whether w is a XY-link or YX-link.
   * XY-link: threshold = (k + 1 -(iota(p) - 1)) = k + 2 - iota(p)
   * YX-link: threshold = k + 1 - iota(p)
   * 
   * Simon's congruence pattern matching 논문에서 필요한 SNF 계산 알고리즘
  */
ShortlexResult
computePartialShortlexNormalForm(const string& w,
    vector<int> X_vector,
    vector<int> Y_vector,
    int threshold) {
    int n = (int)w.size();

    int ALPHABET_SIZE = Alphabet::getInstance().size();
    if ((int)X_vector.size() < ALPHABET_SIZE)
        X_vector.assign(ALPHABET_SIZE, 1);
    if ((int)Y_vector.size() < ALPHABET_SIZE)
        Y_vector.assign(ALPHABET_SIZE, 1);

    // Now you can safely index X_vector[0..ALPHABET_SIZE-1] and Y_vector[…]
    vector<int> X(n, 0), Y(n, 0);
    string  shortlexNormalForm;
    deque<int> shortlexNormalX, shortlexNormalY;
    set<char> w_alphabet;

    // Make working copies that we'll resize as needed
    vector<int> Xcnt = std::move(X_vector);
    vector<int> Ycnt = std::move(Y_vector);

    // 1. Compute X-coordinates
    for (int i = 0; i < n; i++) {
        char c = w[i];
        int idx = Alphabet::getInstance().charToIndex(c);

        // If the alphabet has grown, resize both Xcnt/Ycnt
        int A = Alphabet::getInstance().size();
        if ((int)Xcnt.size() < A) {
            Xcnt.resize(A, 1);
            Ycnt.resize(A, 1);
        }

        // Safe to index now
        X[i] = Xcnt[idx];
        Xcnt[idx]++;

        // enforce min‐with‐current‐bucket invariant
        for (int j = 0; j < A; j++)
            Xcnt[j] = min(Xcnt[j], Xcnt[idx]);

        // record which letters appeared
        w_alphabet.insert(c);
    }

    // 2. Compute Y‐coords & build SNF
    //    (re-use Xcnt for Y counting by resetting it)
    fill(Ycnt.begin(), Ycnt.end(), 1);
    for (int i = n - 1; i >= 0; --i) {
        char c = w[i];
        int idx = Alphabet::getInstance().charToIndex(c);

        // guard again against alphabet growth
        int A = Alphabet::getInstance().size();
        if ((int)Ycnt.size() < A) {
            Xcnt.resize(A, 1);
            Ycnt.resize(A, 1);
        }

        Y[i] = Ycnt[idx];
        if (X[i] + Y[i] <= threshold) {
            Ycnt[idx]++;
            for (int j = 0; j < A; j++)
                Ycnt[j] = min(Ycnt[j], Ycnt[idx]);

            shortlexNormalForm.insert(shortlexNormalForm.begin(), c);
            shortlexNormalX.push_front(X[i]);
            shortlexNormalY.push_front(Y[i]);
        }
    }

    // 3. Compute the “new” X-vector off of the SNF
    vector<int> newX = Xcnt;         // snapshot Y-phase counts
    vector<int> newY = Ycnt;         // these will become your result vectors

    int m = (int)shortlexNormalForm.size();
    for (int i = 0; i < m; ++i) {
        char c = shortlexNormalForm[i];
        int idx = Alphabet::getInstance().charToIndex(c);

        // guard one last time
        int A = Alphabet::getInstance().size();
        if ((int)newX.size() < A) {
            newX.resize(A, 1);
            newY.resize(A, 1);
        }

        shortlexNormalX.push_back(newX[idx]);
        newX[idx]++;
        for (int j = 0; j < A; j++)
            newX[j] = min(newX[j], newX[idx]);
    }

    // 4. Lex‐sort blocks & compute stackForm / arch_ends
    deque<set<char>> stackForm;
    set<char> alphabet_track;
    vector<int> arch_ends;

    int start = 0;
    while (start < m) {
        int end = start + 1;
        while (end < m
            && shortlexNormalX[end] == shortlexNormalX[start]
            && shortlexNormalY[end] == shortlexNormalY[start]
            && shortlexNormalX[start] + shortlexNormalY[start] == threshold)
            ++end;

        if (end - start > 1)
            sort(shortlexNormalForm.begin() + start,
                shortlexNormalForm.begin() + end);

        set<char> block_cs;
        for (int i = start; i < end; ++i) {
            block_cs.insert(shortlexNormalForm[i]);
            alphabet_track.insert(shortlexNormalForm[i]);
        }
        stackForm.push_front(block_cs);

        // detect arch ends
        if (alphabet_track.size() == w_alphabet.size()) {
            arch_ends.push_back(end);
            alphabet_track.clear();
        }

        start = end;
    }

    // Package up results
    ShortlexResult result;
    result.shortlexNormalForm = std::move(shortlexNormalForm);
    result.X_vector           = std::move(newX);
    result.Y_vector           = std::move(newY);
    result.stackForm          = std::move(stackForm);
    result.arch_ends          = std::move(arch_ends);
    result.alphabet           = std::move(w_alphabet);
    result.universality       = result.arch_ends.size();
    return result;
}
