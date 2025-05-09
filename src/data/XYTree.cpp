#include "data/XYTree.h"

#include <iostream>

#include "utils/Common.h"

using namespace std;
using namespace XYTree;

Node::Node(int index) : index(index) {};

/**
 * @brief X-Tree Construction given precomputed components.
 * 
 * @param ranker Prebuilt ranker table
 * @param shortlex Precomputed `ShortlexResult` of a pattern string
 * @param text Text.
 * @return `XYTree::Tree` the constructed X-tree.
 */
XYTree::Tree XYTree::buildXTree(const RankerTable& ranker, const ShortlexResult& shortlex, const string& text) {
    XYTree::Tree tree;

    shared_ptr<Node> root = make_shared<Node>(INF);
    tree.root = root;
    tree.parent = vector<shared_ptr<Node>>(text.size() + 1);

    unordered_map<int, shared_ptr<Node>> nodes;
    nodes[INF] = root;

    debug(cout << "Building X-tree..." << endl);

    //) Copy out s_p
    deque<set<char>> s_p;
    set<char> s;
    for (int i = 0; i < shortlex.stackForm.size(); i++) {
        debug(cout << "i: " << i << ", pushing s which consists of: " << endl);
        s = shortlex.stackForm.at(i);
        s_p.push_back(s);
        debug(for(auto a: s){ cout << a << " "; } cout << endl);
    }

    // ln 4-6
    set<char> deleted_chars;
    for (int i = 0; i < shortlex.universality; i++) {
        while (deleted_chars.size() < shortlex.alphabet.size()) {
            for(auto a: s_p.back()) deleted_chars.insert(a);
            s_p.pop_back();
        }
        deleted_chars.clear();
    }
    debug(cout << "s_p is left with: " << endl);
    debug(for(auto ss: s_p){ for(auto a: ss){ cout << a << endl; } } cout << endl);

    // ln 7-21
    deque<set<char>> sp_p;
    set<char> S;
    shared_ptr<Node> last_node = root;
    for (int i = 0; i < static_cast<int>(text.size()); i++) {
        int parent = -1;
        int x_rank;
        for (char a : shortlex.alphabet) {
            x_rank = ranker.getX(i, a);
            if (x_rank > parent) parent = x_rank;
        }

        // ln 9-18
        if (nodes.count(parent) == 0) {
            shared_ptr<Node> parent_node = make_shared<Node>(parent);
            nodes[parent] = parent_node;
            debug(cout << "Generate new node " << *parent_node << endl);
            last_node->next = parent_node;
            last_node = parent_node;

            // line 11: s'_p <- copy(s_p)
            sp_p.clear();
            for (auto entry = s_p.begin(); entry != s_p.end(); entry++) {
                sp_p.push_back(*entry);
            }

            // line 12: T_X(T).r(parent) <- parent : 논문에선 parent 대신 i로 써있는데, parent가 맞는 것으로 결론지음.
            parent_node->r = parent;

            // line 13: while s'_p is not empty
            while (!sp_p.empty()) {
                // line 14: S <- peek(s'_p)
                S = sp_p.back();

                // line 15: sigma = arg min (R_X(T, T_X(T).r(parent), c))
                int min_x_rank = -1;
                char sigma;
                for (char c : S) {
                    x_rank = ranker.getX(parent_node->r, c);
                    if (min_x_rank == -1 || x_rank < min_x_rank) {
                        min_x_rank = x_rank;
                        sigma = c;
                    }
                }

                // line 16: T_X(T).r(parent) <- R_X(T, T_X(T).r(parent), sigma)
                parent_node->r = ranker.getX(parent_node->r, sigma);
                if (parent_node->r == INF) {
                    parent_node->r = parent;
                    break;
                }

                // line 17: pop sigma from s'_p
                sp_p.back().erase(sigma);
                if (sp_p.back().empty()) {
                    sp_p.pop_back();
                }
            }

            // line 18: T_X(T).chld(parent) <- [i, i]   // 논문에서는 [i, i)지만 편의상 [i, i]를 쓰기로
            parent_node->children = Interval(i, i - 1);  // line 19 에서 [i, i]로 바뀔 것
        }

        // line 19: extend end point of T_X(`T`).chld(parent) by one
        nodes[parent]->children.end++;

        // line 21: T_X(T).prnt(i) <- parent (Note: 논문의 Algorithm 1 에서는 i가 Nodes에 포함인 경우에만 하도록 되어있지만, 6페이지에 모든 i에 대하여 prnt(i)가 작동하도록 하게끔 abuse한다는 내용이 있음)
        tree.parent[i] = nodes[parent];
        debug(cout << "Set parent of " << i << " to " << *nodes[parent] << endl);
    }

    // Clean up
    last_node->next = root;
    tree.parent[text.size()] = root;

    debug(cout << "End of X-tree construction" << endl << endl);
    return tree;
}

/**
 * @brief Y-Tree Construction given precomputed components.
 * 
 * @param ranker Prebuilt ranker table
 * @param shortlex Precomputed `ShortlexResult` of a pattern string
 * @param text Text.
 * @return `XYTree::Tree` the constructed Y-tree.
 */
XYTree::Tree XYTree::buildYTree(const RankerTable& ranker, const ShortlexResult& shortlex, const string& text) {
    XYTree::Tree tree;

    shared_ptr<Node> root = make_shared<Node>(-1);
    tree.root = root;
    tree.parent = vector<shared_ptr<Node>>(text.size() + 1);

    unordered_map<int, shared_ptr<Node>> nodes;
    nodes[-1] = root;

    debug(cout << "Building Y-tree..." << endl);

    // Copy out s_p (in reverse order) << not sure if reversing is mandatory
    vector<set<char>> s_p;
    set<char> s;
    for (int i = 0; i < shortlex.stackForm.size(); i++) {
        debug(cout << "i: " << i << ", pushing s which consists of: " << endl);
        s = shortlex.stackForm.at(shortlex.stackForm.size() - i - 1);
        s_p.push_back(s);
        debug(for(auto a: s){ cout << a << " "; } cout << endl);
    }

    // ln 4-6 (Slight rework as well)
    set<char> deleted_chars;
    for (int i = 0; i < shortlex.universality; i++) {
        while (deleted_chars.size() < shortlex.alphabet.size()) {
            for(auto a: s_p.back()) deleted_chars.insert(a);
            s_p.pop_back();
        }
        deleted_chars.clear();
    }
    debug(cout << "s_p is left with: " << endl);
    debug(for(auto ss: s_p){ for(auto a: ss){ cout << a << endl; } } cout << endl);

    // ln 7-21
    vector<set<char>> sp_p;
    set<char> S;
    shared_ptr<Node> last_node = root;
    for (int i = static_cast<int>(text.size()); i > 0; i--) {
        int parent = INF;
        int y_rank;
        for (auto a : shortlex.alphabet) {
            y_rank = ranker.getY(i, a);
            if (y_rank < parent) parent = y_rank;
        }


        // ln 9-18
        if (nodes.count(parent) == 0) {
            shared_ptr<Node> parent_node = make_shared<Node>(parent);
            nodes[parent] = parent_node;
            debug(cout << "Generate new node " << *parent_node << endl);
            last_node->next = parent_node;
            last_node = parent_node;

            // line 11: s'_p <- copy(s_p)
            sp_p.clear();
            for (auto entry = s_p.begin(); entry != s_p.end(); entry++) {
                sp_p.push_back(*entry);
            }

            // line 12: T_Y(T).r(parent) <- parent : 논문에선 parent 대신 i로 써있는데, parent가 맞는 것으로 결론지음.
            parent_node->r = parent;

            // line 13: while s'_p is not empty
            while (!sp_p.empty()) {
                // line 14: S <- peek(s'_p)
                S = sp_p.back();

                // line 15: sigma = arg min (R_Y(T, r(i), c))
                int max_y_rank = INF;
                char sigma;
                for (char c : S) {
                    y_rank = ranker.getY(parent_node->r, c);
                    if (max_y_rank == INF || y_rank > max_y_rank) {
                        max_y_rank = y_rank;
                        sigma = c;
                    }
                }

                // line 16: T_Y(T).r(i) <- R_Y(T, T_Y(T).r(i), sigma)
                parent_node->r = ranker.getY(parent_node->r, sigma);
                if (parent_node->r < 0) {
                    parent_node->r = parent;
                    break;
                }

                // line 17: pop sigma from s'_p
                sp_p.back().erase(sigma);
                if (sp_p.back().empty()) {
                    sp_p.pop_back();
                }
            }

            // line 18: T_Y(T).chld(parent) <- [i, i]   // 논문에서는 [i, i)지만 편의상 [i, i]를 쓰기로
            parent_node->children = Interval(i + 1, i);  // line 19 에서 [i, i]로 바뀔 것
        }

        // line 19: extend end point of T_Y(T).chld(parent) by one
        nodes[parent]->children.start--;

        // line 21: T_Y(T).prnt(i) <- parent (Note: 논문의 Algorithm 1 에서는 i가 Nodes에 포함인 경우에만 하도록 되어있지만, 6페이지에 모든 i에 대하여 prnt(i)가 작동하도록 하게끔 abuse한다는 내용이 있음)
        tree.parent[i] = nodes[parent];
        debug(cout << "Set parent of " << i << " to " << *nodes[parent] << endl);
    }

    // Clean up
    last_node->next = root;
    tree.parent[0] = root;

    debug(cout << "End of Y-tree construction" << endl << endl);
    return tree;
}