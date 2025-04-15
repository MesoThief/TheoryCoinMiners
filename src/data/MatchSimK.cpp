#include <iostream>

#include "data/MatchSimK.h"
#include "data/XYTree.h"
#include "utils/Alphabet.h"
#include "utils/CalculateUniversality.h"
#include "utils/Common.h"

/**
 * MatchSimK 알고리즘 구현
 */
vector<MatchSimK::triple> MatchSimK::matchSimK(string text, string pattern, int k) {
  // line 1: Given: a pattern p, a text T, an integer k
  
  // 일부 데이터 전처리
  set<char> alph_p;
  for (char c : pattern) {
    alph_p.insert(c);
  }
  int pattern_universality = calculateUniversalityIndex(pattern);
  
  cout << "Computing MatchSimK..." << endl;

  // line 2: Returns: a set S of tripes where, for space positions f and b of T,
  // T[f : b] ~k p if and only if there exists some element e = ([f_1, f_2], [b_1, b_2], offset) in S
  // such that space positions f - offset \in [f_1, f_2] and b - offset \in [b_1, b_2]

  // line 3: positions <- empty set
  vector<triple> positions;

  // line 4: s_p <-ShortLex_k(p) in stack form
  ShortlexResult shortlex_p = computePartialShortlexNormalForm(
    pattern,
    vector<int>(Alphabet::getInstance().size(), 1),
    vector<int>(Alphabet::getInstance().size(), 1),
    k + 1
  ); // stack form = shortlex_p.stackForm
  debug(cout << "shortlex normal form of pattern is: " << shortlex_p.shortlexNormalForm << endl);

  // line 5: Slice T whenever T[i] \not-in alph(p)
  vector<Interval> sub_Ts;
  int start = 0;
  int end = 0;
  for (char t : text) {
    if (alph_p.count(t) == 0) {
      if (start < end) sub_Ts.push_back(Interval(start, end - 1));
      end++;
      start = end;
      continue;
    }
    end++;
  }
  if (start < end) {
    sub_Ts.push_back(Interval(start, end - 1));
  }

  // DEBUG: check sub_Ts
  // for (auto sub_T : sub_Ts) {
  //   cout << "(" << get<0>(sub_T) << "," << get<1>(sub_T) << ")" << endl;
  // }

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
  for (Interval sub_T : sub_Ts) {
    string sub_T_string = text.substr(sub_T.start, sub_T.end);

    // line 9: offset <- the start space position of T' in T
    int offset = sub_T.start;

    // line 10: Map <- empty map for saving vectors and substrings
    unordered_map<int, string> map;
    
    // line 11: Preprocess X- and Y-ranker array
    RankerTable rankers = RankerTable(sub_T_string);
    rankers.buildXRankerTable();
    rankers.buildYRankerTable();

    // line 12: Construct X-tree T_X(T') and Y-tree T_Y(T')
    XYTree::Tree x_tree = XYTree::buildXTree(rankers, shortlex_p, sub_T_string);
    XYTree::Tree y_tree = XYTree::buildYTree(rankers, shortlex_p, sub_T_string);

    // line 13: for all nodes i \in T_X(T').nodes do
    for (shared_ptr<XYTree::Node> node_i = x_tree.root->next; node_i != x_tree.root; node_i = node_i->next) {
      // line 14: From i, go up the X-tree for ι(p)-1 edges
      shared_ptr<XYTree::Node> current_node = node_i;
      debug(cout << "starting X-tree traversal from: " << *current_node << endl);
      for (int i = 0; i < pattern_universality-1; i++) {
        current_node = x_tree.parent[current_node->index];
        debug(cout << "traversing X-tree: " << *current_node << endl);
        if (current_node == x_tree.root) break;
      }
      debug(cout << "X-tree ends at: " << *current_node << endl);


      // line 15: j_1 <- T_X(T').r(current node)
      int j_1 = current_node->r;

      // line 16: if j_1 = ∞, break.
      if (j_1 == INF) break;  // TODO: 이거 맞나?

      // line 17: From j_1, go up the Y-tree using ι(p) calls of T_Y(T').prnt()
      debug(cout << "starting Y-tree traversal from: " << *current_node << endl);
      current_node = y_tree.parent[j_1];
      debug(cout << "Y-tree start becomes: " << *current_node << endl);
      for (int i = 0; i < pattern_universality - 1; i++) {
        current_node = y_tree.parent[current_node->index];
        debug(cout << "traversing Y-tree: " << *current_node << endl);
        if (current_node == y_tree.root) break;
      }
      debug(cout << "Y-tree ends at: " << *current_node << endl);

      // line 18: n <- current node
      int n = current_node->index;

      // line 19: j_2 <- max(T_Y(T').chld(i) AND [max_{σ in B}{R_Y(T', n, σ)+1, n}])
      Interval chld = node_i->children;
      debug(cout << "Children of " << n << " are: " << current_node->children << endl);

      int r_y_bound = n; // if all R_Y return INF, then default to n + 1
      for (char sigma : B) {
        int ry = rankers.getY(n, sigma);  // R_Y(T', n, σ)
        if (ry != INF) {
          r_y_bound = std::max(r_y_bound, ry);
        }
      }
      debug(cout << "R_Y bounds interval: [" << r_y_bound << ", " << (n + 1) << "]" << endl);

      int j_2 = -1;
      for (int chld_pos = chld.start; chld_pos <= chld.end; ++chld_pos) { // T_Y(T').chld(i)
        if (chld_pos >= r_y_bound) {  //&& chld_pos <= n) {
          j_2 = std::max(j_2, chld_pos);
        }
      }

      // line 20: if no such value exists, continue.
      if (j_2 == -1) continue;

      // line 21: z <- ShortLex_k(T'[j_2 : j_1]) using the checkpoint mechanism and Map
      string T_prime = text.substr(offset + j_2, j_1 - j_2 + 1);
      ShortlexResult shortlex_z = computePartialShortlexNormalForm(
        T_prime,
        vector<int>(Alphabet::getInstance().size(), 1),
        vector<int>(Alphabet::getInstance().size(), 1),
        k + 1
      );
      
      // line 22: Save Checkpoints for each arch link of T'[j_2 : j_1]
      // TODO memoization, checkpoints 통해서 최척화
      
      // line 23: if z ~k ShortLex(p) then
      string z = shortlex_z.shortlexNormalForm;
      debug(cout << "z becomes: " << z << endl);

      if (z == shortlex_p.shortlexNormalForm) {
        // line 24: interval1 <- T_X(T').chld(i) AND [max_{σ in B}{R_Y(T', j_2, σ)+1, j_2}]
        int interval1_start = -1;
        for (char sigma : B) {
          int r_y = rankers.getY(j_2, sigma);
          if (r_y != INF) {
            interval1_start = std::max(interval1_start, r_y + 1);
          }
        }
        interval1_start = std::min(chld.start, interval1_start); // intersection with T_X(T').chld(i) using intervals
        int interval1_end = std::max(chld.end, j_2); // intersection with T_X(T').chld(i) using intervals
        Interval interval1 = Interval(interval1_start, interval1_end);

        // line 25: interval2 <- [j_1, min_{σ in A}{R_X(T', j_1, σ)-1}]
        int interval2_start = j_1;
        int interval2_end = INF;
        for (char sigma : A) {
          int r_x = rankers.getX(j_1, sigma);
          if (r_x != INF) {
            interval2_end = std::min(interval2_end, r_x - 1);
          }
        }
        Interval interval2 = Interval(interval2_start, interval2_end);

        // line 26: add (interval1, interval2, offset) to positions
        positions.push_back({
          Interval(interval1_start, interval1_end),
          Interval(interval2_start, interval2_end),
          offset
        });
        debug(cout << "push new position: " << interval1 << ", " << interval2 << ", " << offset << endl);
      }
    }
  }

  // line 27: return positions
  return positions;
}