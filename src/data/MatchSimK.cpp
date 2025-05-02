#include "data/MatchSimK.h"

#include <iostream>

#include "data/XYTree.h"
#include "utils/Alphabet.h"
#include "utils/CalculateUniversality.h"
#include "utils/Common.h"

auto printVector = [](const vector<int>& v, const string& name) {
    cout << name << " = [ ";
    for (int val : v) {
        cout << val << " ";
    }
    cout << "]" << endl;
};

/**
 * MatchSimK 알고리즘 구현
 */
vector<MatchSimK::triple> MatchSimK::matchSimK(const string& text, const string& pattern, int k) {
    // line 1: Given: a pattern p, a text T, an integer k

    // 일부 데이터 전처리
    set<char> alph_p;
    for (char c : pattern) {
        alph_p.insert(c);
    }
    string alph_p_string(alph_p.begin(), alph_p.end());
    Alphabet::getInstance().setAlphabet(alph_p_string);
    int pattern_universality = calculateUniversalityIndex(pattern);

    debug(cout << "Computing MatchSimK..." << endl);

    // line 2: Returns: a set S of tripes where, for space positions f and b of T,
    // T[f : b] ~k p if and only if there exists some element e = ([f_1, f_2], [b_1, b_2], offset) in S
    // such that space positions f - offset \in [f_1, f_2] and b - offset \in [b_1, b_2]

    // line 3: positions <- empty set
    vector<triple> positions;

    // line 4: s_p <-ShortLex_k(p) in stack form
    ShortlexResult shortlex_p = computePartialShortlexNormalForm(pattern,
        vector<int>(Alphabet::getInstance().size(), 1),
        vector<int>(Alphabet::getInstance().size(), 1),
        k + 1);  // stack form = shortlex_p.stackForm
    debug(cout << "shortlex normal form of pattern is: " << shortlex_p.shortlexNormalForm << endl);

    // line 5: Slice T whenever T[i] \not-in alph(p)
    vector<Interval> sub_Ts;
    int start = 0;
    int end = 0;
    for (char t : text) {
        if (alph_p.count(t) == 0) {
            if (start < end) sub_Ts.emplace_back(start, end);
            end++;
            start = end;
            continue;
        }
        end++;
    }
    if (start < end) {
        sub_Ts.emplace_back(start, end);
    }

    // line 6: A <- {σ | pσ not~k p}
    // line 7: B <- {σ | σp not~k p}
    set<char> A;
    set<char> B;
    for (char sigma : alph_p) {
        int index = Alphabet::getInstance().charToIndex(sigma);
        int X = shortlex_p.X_vector[index];
        int Y = shortlex_p.Y_vector[index];

        if (X + 1 <= k + 1) {
            A.insert(sigma);
        };

        if (1 + Y <= k + 1) {
            B.insert(sigma);
        };
    }
    debug(
        cout << "A: "; for (char sigma : A) { cout << sigma << " "; } cout << endl; cout << "B: ";
        for (char sigma : B) { cout << sigma << " "; } cout << endl;);

    // line 8: for all sliced substrings T' of T do
    for (Interval sub_T : sub_Ts) {
        string sub_T_string = text.substr(sub_T.start, sub_T.end - sub_T.start);
        debug(cout << "For sub_T string: " << sub_T_string << endl);

        // line 9: offset <- the start space position of T' in T
        int offset = sub_T.start;

        // line 10: Map <- empty map for saving vectors and substrings
        unordered_map<int, string> map;  // TODO: checkpoint 관련 구현 시 수정

        // line 11: Preprocess X- and Y-ranker array
        RankerTable rankers = RankerTable(sub_T_string);
        rankers.buildXRankerTable();
        rankers.buildYRankerTable();

        // line 12: Construct X-tree T_X(T') and Y-tree T_Y(T')
        XYTree::Tree x_tree = XYTree::buildXTree(rankers, shortlex_p, sub_T_string);
        XYTree::Tree y_tree = XYTree::buildYTree(rankers, shortlex_p, sub_T_string);

        // make a sub_tree length of vector
        // which stores check_point starting from such indexes
        vector<vector<CheckPoint>> check_points(sub_T.end - sub_T.start);

        // line 13: for all nodes i \in T_X(T').nodes do
        for (shared_ptr<XYTree::Node> node_i = x_tree.root->next; node_i != x_tree.root; node_i = node_i->next) {
            // preprocessing: make vector x_arch_indexes to save the end points of x-arch links
            vector<int> x_arch_indexes;
            x_arch_indexes.push_back(node_i->index);
            debug(cout << "add to x_arch_indexes: " << node_i->index << endl);
            // line 14: From i, go up the X-tree for ι(p)-1 edges
            shared_ptr<XYTree::Node> current_node = node_i;
            debug(cout << "starting X-tree traversal from: " << *current_node << endl);
            for (int i = 0; i < pattern_universality - 1; i++) {
                current_node = x_tree.parent[current_node->index];
                x_arch_indexes.push_back(current_node->index);
                debug(cout << "add to x_arch_indexes: " << current_node->index << endl);
                debug(cout << "traversing X-tree: " << *current_node << endl);
                if (current_node == x_tree.root) break;
            }
            if (current_node == x_tree.root) {
                debug(cout << "reached root while traversing X-tree. Skipping to next T'" << endl);
                continue;
            }
            debug(cout << "X-tree ends at: " << *current_node << endl);

            // line 15: j_1 <- T_X(T').r(current node)
            int j_1 = current_node->r;
            debug(cout << "j_1 value: " << j_1 << endl);

            // line 16: if j_1 = ∞, break.
            if (j_1 == INF) break;

            // preprocessing: make vector y_arch_indexes to save the end points of y-arch links
            vector<int> y_arch_indexes;
            y_arch_indexes.push_back(j_1);

            // line 17: From j_1, go up the Y-tree using ι(p) calls of T_Y(T').prnt()
            debug(cout << "starting Y-tree traversal from: " << *current_node << endl);
            current_node = y_tree.parent[j_1];
            y_arch_indexes.push_back(current_node->index);
            debug(cout << "add to y_arch_indexes: " << current_node->index << endl);
            debug(cout << "Y-tree start becomes: " << *current_node << endl);
            for (int i = 0; i < pattern_universality - 1; i++) {
                current_node = y_tree.parent[current_node->index];
                y_arch_indexes.push_back(current_node->index);
                debug(cout << "add to y_arch_indexes: " << current_node->index << endl);
                debug(cout << "traversing Y-tree: " << *current_node << endl);
                if (current_node == y_tree.root) break;
            }
            if (current_node == y_tree.root) {
                debug(cout << "reached root while traversing Y-tree. Skipping to next T'" << endl);
                continue;
            }
            debug(cout << "Y-tree ends at: " << *current_node << endl);

            // line 18: n <- current node
            int n = current_node->r;

            // line 19: j_2 <- max(T_X(T').chld(i) AND [max_{σ in B}{R_Y(T', n, σ)+1, n}])
            debug(cout << "children of " << *node_i << " are: " << node_i->children << endl);
            int max_r_y = -1;
            for (char sigma : B) {
                int r_y = rankers.getY(n, sigma) + 1;
                debug(cout << "ranker_Y = " << r_y-1 << " (n=" << n << ", sigma=" << sigma << ")" << endl);
                if (r_y != -1) {
                    max_r_y = max(r_y, max_r_y);
                }
            }
            Interval j_2_candidate = Interval(max_r_y, n);

            int j_2;
            int intersection_start = max(node_i->children.start, j_2_candidate.start);
            int intersection_end = min(node_i->children.end, j_2_candidate.end);
            debug(cout << "Intv 1 (chld): " << node_i->children << endl);
            debug(cout << "Intv 2 (j_2): " << j_2_candidate << endl);
            if (intersection_start <= intersection_end) {
                j_2 = intersection_end;
            } else {
                // line 20: if no such value exists, continue.
                debug(cout << "skipping due to invalid j_2" << endl << endl);
                continue;
            }
            debug(cout << "j_2 value: " << j_2 << endl);

            // make j_2 a starting point of x_arch_indexes
            x_arch_indexes.insert(x_arch_indexes.begin(), j_2);

            // line 21: z <- ShortLex_k(T'[j_2 : j_1]) using the checkpoint mechanism and Map
            // line 22: Save Checkpoints for each arch link of T'[j_2 : j_1]
            string z = shortlex_with_checkpoint(k, pattern_universality, sub_T_string, check_points, x_arch_indexes, y_arch_indexes);

            // line 23: if z ~k ShortLex(p) then
            if (z == shortlex_p.shortlexNormalForm) {
                cout << "\n[DEBUG] z == shortlex_p.shortlexNormalForm MATCHED\n";
                
                // line 24: interval1 <- T_X(T').chld(i) AND [max_{σ in B}{R_Y(T', j_2, σ)+1, j_2}]
                cout << "[interval1] Computing from B and getY(j_2 = " << j_2 << ")\n";

                int interval1_start = -1;
                for (char sigma : B) {
                    int r_y = rankers.getY(j_2, sigma);
                    cout << "  - B contains '" << sigma << "', getY(" << j_2 << ", '" << sigma << "') = "
                              << ((r_y == INF) ? "INF" : to_string(r_y)) << "\n";
                    if (r_y != INF) {
                        interval1_start = max(interval1_start, r_y + 1);
                    }
                }

                cout << "  -> After max with node_i->children.start = " << node_i->children.start << "\n";
                interval1_start = max(node_i->children.start, interval1_start);
                int interval1_end = min(node_i->children.end, j_2);

                Interval interval1(interval1_start, interval1_end);
                cout << "  => Final interval1 = [" << interval1.start << ", " << interval1.end << "]\n";

                // line 25: interval2 <- [j_1, min_{σ in A}{R_X(T', j_1, σ)-1}]
                cout << "[interval2] Computing from A and getX(j_1 = " << j_1 << ")\n";

                int interval2_end = sub_T.end - offset;
                for (char sigma : A) {
                    int r_x = rankers.getX(j_1, sigma);
                    cout << "  - A contains '" << sigma << "', getX(" << j_1 << ", '" << sigma << "') = " << r_x << "\n";
                    interval2_end = min(interval2_end, r_x - 1);
                }

                Interval interval2(j_1, interval2_end);
                cout << "  => Final interval2 = [" << interval2.start << ", " << interval2.end << "]\n";

                // line 26: add (interval1, interval2, offset) to positions
                positions.emplace_back(interval1, interval2, offset);
                cout << "[position ADDED] interval1 = " << interval1 << ", "
                          << "interval2 = " << interval2 << ", "
                          << "offset = " << offset << "\n\n";
            }
        }
    }

    // line 27: return positions
    return positions;
}

string MatchSimK::shortlex_with_checkpoint(
    int k,
    int pattern_universality,
    const string& sub_T_string,
    vector<vector<MatchSimK::CheckPoint>>& check_points,
    const vector<int>& x_arch_indexes,
    const vector<int>& y_arch_indexes
) {
    vector<string> partial_shortlex_z(2 * pattern_universality + 1);
    vector<vector<int>> x_vectors(pattern_universality + 1);
    vector<vector<int>> y_vectors(pattern_universality + 1);

    cout << "\nTargeting minimal candidate: ["
                <<  x_arch_indexes[0] << ", " << x_arch_indexes[pattern_universality]
                <<  "]" << endl;

    // compute YX-link first
    for (int i = 0; i <= pattern_universality; i++) {
        int x_val = x_arch_indexes[i];
        int y_val = y_arch_indexes[pattern_universality - i];

        Interval yx_link(x_val, y_val);

        bool found = false;

        for (const MatchSimK::CheckPoint& cp : check_points[x_val]) {
            if (cp.link.end != y_val) continue;

            partial_shortlex_z[2 * i] = cp.partial_shortlex;
            x_vectors[i] = cp.x_vector;
            y_vectors[i] = cp.y_vector;

            cout << "[YX-link FOUND] i = " << i
                        << ", Interval = (" << x_val << ", " << y_val << ")"
                        << ", Partial ShortLex = " << cp.partial_shortlex << endl;

            found = true;
            break;
        }

        if (!found || check_points[x_val].empty()) {
            cout << "[YX-link COMPUTE] i = " << i
                        << ", Will compute shortlex for substring [" << x_val << ", " << y_val << "]"
                        << " = " << sub_T_string.substr(x_val, y_val - x_val) << endl;

            int threshold = k + 1 - pattern_universality;
            cout <<  "threshold: " << threshold << endl;
            ShortlexResult partialShortlex = computePartialShortlexNormalForm(
                sub_T_string.substr(x_val, y_val - x_val),
                vector<int>(Alphabet::getInstance().size(), 1),
                vector<int>(Alphabet::getInstance().size(), 1),
                k + 1 - pattern_universality
            );

            partial_shortlex_z[2 * i] = partialShortlex.shortlexNormalForm;

            check_points[x_val].emplace_back(
                yx_link,
                partialShortlex.shortlexNormalForm,
                partialShortlex.X_vector,
                partialShortlex.Y_vector
                );
            x_vectors[i] = partialShortlex.X_vector;
            y_vectors[i] = partialShortlex.Y_vector;

            cout << "[YX-link COMPUTED] i = " << i
                        << ", Computed ShortLex = " << partialShortlex.shortlexNormalForm << endl;
        }
    }

    // compute XY-link next
    for (int i = 0; i < pattern_universality; i++) {
        int x_val = y_arch_indexes[pattern_universality - i];
        int y_val = x_arch_indexes[i + 1];

        Interval xy_link(x_val, y_val);

        bool found = false;

        for (const MatchSimK::CheckPoint& cp : check_points[x_val]) {
            if (cp.link.end == y_val) {
                partial_shortlex_z[2 * i + 1] = cp.partial_shortlex;

                cout << "[XY-link FOUND] i = " << i
                            << ", Interval = (" << x_val << ", " << y_val << ")"
                            << ", Partial ShortLex = " << cp.partial_shortlex << endl;

                found = true;
                break;
            }
        }

        if (!found) {
            vector<int> x_vector =
                (i == 0) ? vector<int>(Alphabet::getInstance().size(), 1) : x_vectors[i];
            vector<int> y_vector =
                (i == pattern_universality - 1) ? vector<int>(Alphabet::getInstance().size(), 1) : y_vectors[i + 1];

            cout << "[XY-link COMPUTE] i = " << i
                        << ", Will compute shortlex for substring [" << x_val << ", " << y_val << "]"
                        << " = " << sub_T_string.substr(x_val, y_val - x_val) << endl;

            printVector(x_vector, "X_vector");
            printVector(y_vector, "Y_vector");

            ShortlexResult partialShortlex = computePartialShortlexNormalForm(
                sub_T_string.substr(x_val, y_val - x_val),
                x_vector,
                y_vector,
                k + 2 - pattern_universality
            );

            partial_shortlex_z[2 * i + 1] = partialShortlex.shortlexNormalForm;

            check_points[x_val].emplace_back(xy_link, partialShortlex.shortlexNormalForm);

            cout << "[XY-link COMPUTED] i = " << i
                        << ", Computed ShortLex = " << partialShortlex.shortlexNormalForm << endl;
        }
    }

    // finally, combine
    string z;
    for (const string& part : partial_shortlex_z) {
        z += part;
    }
    cout << "[Final Z Combined String] = " << z << endl << endl;

    return z;
}