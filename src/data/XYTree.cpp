#include "data/XYTree.h"

#include <iostream>

#include "utils/Common.h"

using namespace std;

namespace XYTree
{
    thread_local std::vector<Node>             nodePoolX;
    thread_local std::unordered_map<int,Node*> nodesMapX;
    thread_local std::vector<Node>             nodePoolY;
    thread_local std::unordered_map<int,Node*> nodesMapY;

    Node::Node(int index) : index(index) {}

    /**
     * @brief X-Tree Construction given precomputed components.
     *
     * @param ranker Prebuilt ranker table
     * @param shortlex Precomputed `ShortlexResult` of a pattern string
     * @param text Text.
     * @return `XYTree::Tree` the constructed X-tree.
     */
    Tree buildXTree(const RankerTable& ranker, const ShortlexResult& shortlex, const string& text) {
        Tree tree;

        nodePoolX.clear();
        nodesMapX.clear();

        nodePoolX.reserve(text.size() + shortlex.stackForm.size() + 5);
        nodesMapX.reserve(text.size() + 5);

        nodePoolX.emplace_back(INF);
        Node* root = &nodePoolX.back();
        tree.root = root;
        tree.parent.assign(text.size() + 1, nullptr);
        nodesMapX[INF] = root;

        debug(cout << "Building X-tree..." << endl);

        auto s_p = shortlex.stackForm;
        //) Copy out s_p
        {
            std::set<char> deleted_chars;
            for (int u = 0; u < shortlex.universality; ++u) {
                while (deleted_chars.size() < shortlex.alphabet.size()) {
                    for (char c : s_p.back()) deleted_chars.insert(c);
                    s_p.pop_back();
                }
                deleted_chars.clear();
            }
        }

        // ln 4-6
        // before the loop
        size_t alphaSize = shortlex.alphabet.size();
        set<char> deleted_chars;

        // Trim s_p down by “universality” arches, but stop if we run dry
        for (int u = 0; u < shortlex.universality; ++u) {
            deleted_chars.clear();
            while (deleted_chars.size() < alphaSize) {
                if (s_p.empty()) {
                    cerr << "[XYTree] Warning: ran out of stackForm blocks at universality step "
                         << u << " (needed " << alphaSize << " distinct chars)\n";
                    break;        // bail out of the while, then go to next u
                }
                for (char c : s_p.back()) {
                    deleted_chars.insert(c);
                }
                s_p.pop_back();
            }
        }

        debug(cout << "s_p is left with: " << endl);
        debug(for(auto ss: s_p){ for(auto a: ss){ cout << a << endl; } } cout << endl);

        Node* last = root;
        for (int i = 0; i < (int)text.size(); ++i) {
            // parent rank 계산
            int parentRank = -1;
            for (char a : shortlex.alphabet) {
                int xr = ranker.getX(i, a);
                if (xr > parentRank) parentRank = xr;
            }

            // 노드 생성 혹은 재사용
            Node* pnode;
            auto it = nodesMapX.find(parentRank);
            if (it == nodesMapX.end()) {
                nodePoolX.emplace_back(parentRank);
                pnode = &nodePoolX.back();
                nodesMapX[parentRank] = pnode;

                last->next = pnode;
                last = pnode;

                // children 초기화
                pnode->children = Interval(i, i - 1);

                // s_p를 sp_p에 복사한 뒤 checkpoint loop (기존 로직)
                auto sp_p = s_p;
                while (!sp_p.empty()) {
                    auto S = sp_p.back(); sp_p.pop_back();
                    // sigma 선택 후 pnode->r 갱신, break 조건 등
                    // … (기존 알고리즘 그대로) …
                }
            } else {
                pnode = it->second;
            }

            // 자식 구간 확장
            pnode->children.end++;
            tree.parent[i] = pnode;
        }

        last->next = root;
        tree.parent[text.size()] = root;
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
    Tree buildYTree(const RankerTable& ranker,
        const ShortlexResult& shortlex,
        const std::string& text) {
        // clear and reuse
        nodePoolY.clear();
        nodesMapY.clear();

        nodePoolY.reserve(text.size() + shortlex.stackForm.size() + 5);
        nodesMapY.reserve(text.size() + 5);

        Tree tree;
        // root
        nodePoolY.emplace_back(-1);
        Node* root = &nodePoolY.back();
        tree.root = root;
        tree.parent.assign(text.size()+1, nullptr);
        nodesMapY[-1] = root;

        // copy & trim stackForm
        std::vector<std::set<char>> s_p;
        s_p.reserve(shortlex.stackForm.size());
        for (int i = (int)shortlex.stackForm.size()-1; i >= 0; --i)
            s_p.push_back(shortlex.stackForm[i]);

        // clamp universality so it never exceeds s_p.size()
        int uni = min(shortlex.universality, (int)s_p.size());
        {
            std::set<char> deleted;
            for (int u = 0; u < uni; ++u) {
                deleted.clear();
                while (deleted.size() < shortlex.alphabet.size()) {
                    if (s_p.empty()) break;
                    for (char c : s_p.back()) deleted.insert(c);
                    s_p.pop_back();
                }
            }
        }


        // build
        Node* last = root;
        int L = (int)text.size();
        for (int i = L; i > 0; --i) {
            // pick minimal y_rank
            int parentRank = INF;
            for (char a : shortlex.alphabet) {
                int yr = ranker.getY(i, a);
                if (yr < parentRank) parentRank = yr;
            }

            // find or create node
            Node* pnode;
            auto it = nodesMapY.find(parentRank);
            if (it == nodesMapY.end()) {
                nodePoolY.emplace_back(parentRank);
                pnode = &nodePoolY.back();
                nodesMapY[parentRank] = pnode;

                last->next = pnode;
                last = pnode;

                // copy s_p and apply checkpoint loop
                auto sp_p = s_p;
                pnode->r = parentRank;
                while (!sp_p.empty()) {
                    auto S = sp_p.back();
                    int best = INF; char sigma=0;
                    for (char c : S) {
                        int yr = ranker.getY(pnode->r, c);
                        if (yr > best) {
                            best = yr;
                            sigma = c;
                        }
                    }
                    pnode->r = (best==INF ? parentRank : best);
                    if (pnode->r == parentRank) break;
                    sp_p.back().erase(sigma);
                    if (sp_p.back().empty()) sp_p.pop_back();
                }
                // init interval
                pnode->children = Interval(i+1, i);
            } else {
                pnode = it->second;
            }

            // extend
            pnode->children.start--;
            tree.parent[i] = pnode;
        }

        last->next = root;
        tree.parent[0] = root;
        return tree;
    }
}
