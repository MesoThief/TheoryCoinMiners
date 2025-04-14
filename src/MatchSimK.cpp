#include <string>
#include <vector>

#include "data/Shortlex.h"
#include "data/XYTree.h"
#include "utils/Alphabet.h"

using namespace std;

int main(int argc, char* argv[]) {
  // line 1: Given: a pattern p, a text T, an integer k
  string pattern;
  string text;
  int k;

  // 일부 데이터 전처리
  set<char> alph_p;
  for (char c : pattern) {
    alph_p.insert(c);
  }

  // line 2: Returns: a set S of tripes where, for space positions f and b of T,
  // T[f : b] ~k p if and only if there exists some element e = ([f_1, f_2], [b_1, b_2], offset) in S
  // such that space positions f - offset \in [f_1, f_2] and b - offset \in [b_1, b_2]
  using interval = tuple<int, int>;
  using triple = tuple<interval, interval, int>;

  // line 3: positions <- empty set
  vector<triple> positions;

  // line 4: s_p <-ShortLex_k(p) in stack form
  ShortlexResult shortlex_p = computePartialShortlexNormalForm(
    pattern,
    vector<int>(Alphabet::getInstance().size(), 1),
    vector<int>(Alphabet::getInstance().size(), 1),
    k + 1
  ); // stack form = shortlex_p.stackForm

  // line 5: Slice T whenever T[i] \not-in alph(p)
  vector<string> sub_Ts;
  string sub_T;
  for (char t : text) {
    if (alph_p.count(t) > 0) {
      sub_T.push_back(t);
    }
    else if (!sub_T.empty()) {
      sub_Ts.push_back(sub_T);
      sub_T.clear();
    }
  }

  // line 6: A <- {σ | pσ not~k p}
  // line 7: B <- {σ | σp not~k p}
  set<char> A;
  set<char> B;
  for (char sigma : alph_p) {
    int index = Alphabet::getInstance().charToIndex(sigma);
    int X = shortlex_p.X_vector[index];
    int Y = shortlex_p.Y_vector[index];

    if (X + 1 > k + 1) {
      A.insert(sigma);
    };

    if (1 + Y > k + 1) {
      B.insert(sigma);
    };
  }

  // line 8: for all sliced substrings T' of T do
  for (string sub_T : sub_Ts) {
    // line 9: offset <- the start space position of T' in T
    // line 10: Map <- empty map for saving vectors and substrings
    // line 11: Preprocess X- and Y-ranker array
    // line 12: Construct X-tree T_X(T') and Y-tree T_Y(T')

    // line 13: for all nodes i \in T_X(T').nodes do
    for (XYTree::Node node : nodes) {
      // line 14: From i, go up the X-tree for ι(p)-1 edges
      // line 15: j_1 <- T_X(T').r(current node)
      // line 16: if j_1 = ∞, break.
      // line 17: From j_1, go up the Y-tree using ι(p) calls of T_Y(T').prnt()
      // line 18: n <- currrent node
      // line 19: j_2 <- max(T_Y(T').chld(i) AND [max_{σ in B}{R_Y(T', n, σ)+1, n}])
      // line 20: if no such value exists, continue.
      // line 21: z <- ShortLex_k(T'[j_2 : j_1]) using the checkpoint mechanism and Map
      // line 22: Save Checkpoints for each arch link of T'[j_2 : j_1]
      
      // line 23: if z ~k ShortLex(p) then
      if (z ~k Shortlex(p)) {
        // line 24: interval1 <- T_X(T').chld(i) AND [max_{σ in B}{R_Y(T', j_2, σ)+1, j_2}]
        // line 25: interval2 <- [j_1, min_{σ in A}{R_X(T', j_1, σ)-1}]
        // line 26: add (interval1, interval2, offset) to positions
      }
    }
  }

  // line 27: return positions

  return 0;
}