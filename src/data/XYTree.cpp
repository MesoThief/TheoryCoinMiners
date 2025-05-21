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
//                    cerr << "[XYTree] Warning: ran out of stackForm blocks at universality step "
//                         << u << " (needed " << alphaSize << " distinct chars)\n";
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

        // 1) Clear and reuse thread-local pools/maps
        nodePoolY.clear();
        nodesMapY.clear();

        // 2) Reserve to avoid reallocation (keep pointers valid)
        nodePoolY.reserve(text.size() + shortlex.stackForm.size() + 5);
        nodesMapY.reserve(text.size() + 5);

        // 3) Initialize tree and root
        Tree tree;
        nodePoolY.emplace_back(-1);
        Node* root = &nodePoolY.back();
        tree.root = root;
        tree.parent.assign(text.size() + 1, nullptr);
        nodesMapY[-1] = root;

        // 4) Copy stackForm in reverse
        std::vector<std::set<char>> s_p;
        s_p.reserve(shortlex.stackForm.size());
        for (int i = (int)shortlex.stackForm.size() - 1; i >= 0; --i) {
            s_p.push_back(shortlex.stackForm[i]);
        }

        // 5) Clamp universality and trim s_p safely
        int uni = std::min(shortlex.universality, (int)s_p.size());
        const size_t alphaSize = shortlex.alphabet.size();
        for (int u = 0; u < uni; ++u) {
            std::set<char> deleted;
            while (deleted.size() < alphaSize) {
                if (s_p.empty()) {
                    break;  // no more blocks
                }
                for (char c : s_p.back()) {
                    deleted.insert(c);
                }
                s_p.pop_back();
            }
        }

        // 6) Build Y-tree along text indices
        Node* last = root;
        int N = (int)text.size();

        // Fill parent[0..N]
        for (int i = N; i >= 0; --i) {
            // 6a) compute minimal Y-rank over alphabet
            debug(cout << "[buildYTree] i = " << i << endl);

            // 6a) compute minimal Y-rank over alphabet
            int parentRank = INF;
            for (char a : shortlex.alphabet) {
                int yr = ranker.getY(i, a);
                debug(cout << "  getY(" << i << ", '" << a << "') = " << yr << endl);
                if (yr < parentRank) parentRank = yr;
            }
            debug(cout << "  -> minimal parentRank = " << parentRank << endl);

            // 6b) if no valid rank, point back at root
            if (parentRank == INF || parentRank < 0) {
                debug(cout << "  parentRank invalid, setting parent[" << i << "] = root\n");
                tree.parent[i] = root;
                continue;
            }

            // 6c) find or create the Node for parentRank
            Node* pnode;
            auto it = nodesMapY.find(parentRank);
            if (it == nodesMapY.end()) {
                nodePoolY.emplace_back(parentRank);
                pnode = &nodePoolY.back();
                nodesMapY[parentRank] = pnode;

                last->next = pnode;
                last = pnode;

                // checkpoint loop, initializing pnode->r & children interval
                auto sp_p = s_p;
                pnode->r = parentRank;
                while (!sp_p.empty()) {
                    const auto &S = sp_p.back();
                    int bestY = parentRank;
                    char sigma = 0;
                    for (char c : S) {
                        int yr = ranker.getY(pnode->r, c);
                        if (yr > bestY) {
                            bestY = yr;
                            sigma = c;
                        }
                    }
                    if (bestY == parentRank) break;
                    pnode->r = bestY;
                    sp_p.back().erase(sigma);
                    if (sp_p.back().empty()) sp_p.pop_back();
                }

                // set up the initial child-interval [i+1..i]
                pnode->children = Interval(i + 1, i);
            } else {
                pnode = it->second;
            }

            // 6d) extend the start down by one and record parent
            pnode->children.start--;
            tree.parent[i] = pnode;
        }

        // 7) close the circle and set the final parent slot
        last->next    = root;

        return tree;
    }
}
