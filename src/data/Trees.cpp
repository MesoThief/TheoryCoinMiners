#include <iostream>

#include "data/Trees.h"

/**
 * @brief X-Tree Construction given precomputed components.
 * 
 * @param ranker Prebuilt ranker table
 * @param shortlex Precomputed `ShortlexResult` of a pattern string
 * @param text Text.
 * @return `std::shared_ptr<Node>` Root of the tree.
 */
std::shared_ptr<Node> Trees::buildXTree(const RankerTable& ranker, const ShortlexResult& shortlex, const std::string& text) {
    std::shared_ptr<Node> root = std::make_shared<Node>(RankerTable::INF, RankerTable::INF, 0);
    std::unordered_map<int, std::shared_ptr<Node>> nodes;
    nodes[RankerTable::INF] = root;
    
    std::cout << "Building X-tree...\n";

    // Copy out s_p
    std::deque<std::set<char>> s_p;
    std::set<char> s;
    for(int i = 0; i < shortlex.stackForm.size(); i++){
        s = shortlex.stackForm.at(i);
        s_p.push_back(s);
    }

    // ln 4-6
    int last_arch = 0;
    for(int i = 0; i < shortlex.arch_ends.size(); i++){
        for(int j = 0; j < shortlex.arch_ends.at(i) - last_arch; j++){
            s_p.back().erase(shortlex.shortlexNormalForm.at(j+last_arch));
            if(s_p.back().empty()){ s_p.pop_back(); }
        }
        last_arch = shortlex.arch_ends.at(i);
    }

    // ln 7-21
    int parent;
    int x_rank;
    int min_x_rank = -1;
    std::deque<std::set<char>> sp_p;
    std::set<char> S;
    char sigma;
    for(int i = 0; i < static_cast<int>(text.size()); i++){
        parent = -1;
        for(char a : shortlex.alphabet){
            x_rank = ranker.getX(i, a);
            if(x_rank > parent) parent = x_rank;
        }

        // ln 9-18
        if(nodes.count(parent) == 0) {
            shared_ptr<Node> parent_node = std::make_shared<Node>(parent, parent, 0);
            nodes[parent] = parent_node;
            std::cout << "Generate new node " << parent << std::endl;

            // line 11: s'_p <- copy(s_p)
            sp_p.clear();
            for(auto entry = s_p.begin(); entry != s_p.end(); entry++){
                sp_p.push_back(*entry);
            }

            // line 12: T_X(T).r(parent) <- parent : 논문에선 parent 대신 i로 써있는데, parent가 맞는 것으로 결론지음.
            parent_node->r = parent;

            // line 13: while s'_p is not empty
            while(!sp_p.empty()){
                // line 14: S <- peek(s'_p)
                S = sp_p.back();

                // line 15: sigma = arg min (R_X(T, T_X(T).r(parent), c))
                min_x_rank = -1;
                for(char c : S) {
                    x_rank = ranker.getX(parent_node->r, c);
                    if (min_x_rank == -1 || x_rank < min_x_rank) { 
                        min_x_rank = x_rank;
                        sigma = c;
                    }
                }

                // line 16: T_X(T).r(parent) <- R_X(T, T_X(T).r(parent), sigma)
                parent_node->r = ranker.getX(parent_node->r, sigma);
                if (parent_node->r == RankerTable::INF) {
                    break;
                }

                // line 17: pop sigma from s'_p
                sp_p.back().erase(sigma);
                if (sp_p.back().empty()) {
                    sp_p.pop_back();
                }
            }

            // line 18: implemented within line 19
        }

        // line 19: extend end point of T_X(`T`).chld(parent) by one
        if(nodes.count(i) != 0){
            nodes[i]->parent = nodes[parent];
            nodes[parent]->children.push_back(nodes[i]);    // line 18: T_X(T).chld(parent) <- [i, i)
            std::cout << "Set parent of " << i << " to " << parent << std::endl;
        }
    }

    // Clean up
    for(auto pair : nodes){
        int index = pair.first;
        std::shared_ptr<Node> node = pair.second;
        if(node->parent == nullptr && node != root){
            node->parent = root;
            root->children.push_back(node);
            std::cout << "Set parent of " << index << " to " << RankerTable::INF << std::endl;
        }
    }

    std::cout << "End of X-tree construction\n\n"; // Double lb is intended
    return root;
}

/**
 * @brief Y-Tree Construction given precomputed components.
 * 
 * @param ranker Prebuilt ranker table
 * @param shortlex Precomputed `ShortlexResult` of a pattern string
 * @param text Text.
 * @return `std::shared_ptr<Node>` Root of the tree.
 */
std::shared_ptr<Node> Trees::buildYTree(const RankerTable& ranker, const ShortlexResult& shortlex, const std::string& text) {
    std::shared_ptr<Node> root = std::make_shared<Node>(-1, -1, 0);
    std::unordered_map<int, std::shared_ptr<Node>> nodes;
    nodes[-1] = root;
    
    std::cout << "Building Y-tree...\n";

    // Copy out s_p (in reverse order) << not sure if reversing is mandatory
    std::vector<std::set<char>> s_p; std::set<char> s;
    for(int i = 0; i < shortlex.stackForm.size(); i++){
        s = shortlex.stackForm.at(shortlex.stackForm.size() - i - 1);
        s_p.push_back(s);
    }

    // ln 4-6 (Slight rework as well)
    int deleted_chars = 0;
    for(int i = 0; i < shortlex.universality; i++){
        while(deleted_chars < shortlex.alphabet.size()){
            deleted_chars += s_p.back().size();
            s_p.pop_back();
        }
        deleted_chars = 0;
    }

    // ln 7-21
    int parent;
    int y_rank;
    int max_y_rank = RankerTable::INF;
    std::vector<std::set<char>> sp_p;
    std::set<char> S;
    char sigma;
    for(int i = static_cast<int>(text.size()); i > 0; i--){
        parent = RankerTable::INF;
        for(auto a : shortlex.alphabet){
            y_rank = ranker.getY(i, a);
            if(y_rank < parent) parent = y_rank;
        }

        if(parent != -1) parent--;

        // ln 9-18
        if(nodes.count(parent) == 0){
            shared_ptr<Node> parent_node = std::make_shared<Node>(parent, parent, 0);
            nodes[parent] = parent_node;
            std::cout << "Generate new node " << parent << std::endl;

            // line 11: s'_p <- copy(s_p)
            sp_p.clear();
            for(auto entry = s_p.begin(); entry != s_p.end(); entry++){
                sp_p.push_back(*entry);
            }

            // line 12: T_Y(T).r(parent) <- parent : 논문에선 parent 대신 i로 써있는데, parent가 맞는 것으로 결론지음.
            parent_node->r = parent;

            // line 13: while s'_p is not empty
            while(!sp_p.empty()){
                // line 14: S <- peek(s'_p)
                S = sp_p.back();

                // line 15: sigma = arg min (R_Y(T, r(i), c))
                max_y_rank = RankerTable::INF;
                for(char c : S) {
                    y_rank = ranker.getY(parent_node->r, c);
                    if (max_y_rank == RankerTable::INF || y_rank > max_y_rank) {
                        max_y_rank = y_rank;
                        sigma = c;
                    }
                }

                // line 16: T_Y(T).r(i) <- R_Y(T, T_Y(T).r(i), sigma)
                parent_node->r = ranker.getY(parent_node->r, sigma);
                if (parent_node->r < 0) {
                    break;
                }

                // line 17: pop sigma from s'_p
                sp_p.back().erase(sigma);
                if(sp_p.back().empty()) {
                    sp_p.pop_back();
                }
            }

            // line 18: implemented within line 19
        }

        // line 19: extend end point of T_Y(T).chld(parent) by one
        if(nodes.count(i) != 0){
            nodes[i]->parent = nodes[parent];
            nodes[parent]->children.push_back(nodes[i]);    // line 18: T_Y(T).chld(parent) <- [i, i)
            std::cout << "Set parent of " << i << " to " << parent << std::endl;
        }
    }

    // Clean up
    for(auto pair : nodes){
        int index = pair.first;
        std::shared_ptr<Node> node = pair.second;
        if(node->parent == nullptr && node != root){
            node->parent = root;
            root->children.push_back(node);
            std::cout << "Set parent of " << index << " to -1" << std::endl;
        }
    }

    std::cout << "End of Y-tree construction\n\n"; // Double lb is intended
    return root;
}