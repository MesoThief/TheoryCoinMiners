#include <iostream>

#include "data/Trees.h"


/**
 * @brief Builds X-tree, given a pre-computed ranker table and shortlex result of the pattern string, as well as a text.
 * 
 * As like Algorithm 1 in paper!
 */
std::shared_ptr<Node> Trees::buildXTree(const RankerTable& ranker, const ShortlexResult& shortlex, const std::string& text) {
    std::shared_ptr<Node> root = std::make_shared<Node>(std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), 0);
    std::unordered_map<int, std::shared_ptr<Node>> nodes;
    nodes.insert({{std::numeric_limits<int>::max(), root}});
    
    std::cout << "Building X-tree...\n";

    // Copy out s_p
    std::vector<std::set<unsigned char>> s_p; std::set<unsigned char> s;
    for(int i = 0; i < shortlex.s_p.size(); i++){
        s = shortlex.s_p.at(i);
        s_p.push_back(s);
    }

    // ln 4-6 (Slight Rework)
    int deleted_chars = 0;
    for(int i = 0; i < shortlex.universality; i++){
        while(deleted_chars < shortlex.alph.size()){
            deleted_chars += s_p.back().size();
            s_p.pop_back();
        }
        deleted_chars = 0;
    }

    // ln 7-21
    int parent; int x_rnk; int min_x_rnk;
    std::vector<std::set<unsigned char>> sp_p;
    std::unordered_map<int, int> r; std::set<unsigned char> S;
    unsigned char sig;
    for(int i = 0; i < static_cast<int>(text.size()); i++){
        parent = -69;
        for(auto a : shortlex.alph){
            x_rnk = ranker.getX(i, a);
            if(x_rnk > parent) parent = x_rnk;
        }

        // ln 9-18
        if(nodes.count(parent) == 0){
            nodes.insert({{parent, std::make_shared<Node>(parent, parent, 0)}});
            std::cout << "Generate new node " << parent << std::endl;

            // Copy out s_p (ln 11)
            for(int j = 0; j < s_p.size(); j++){
                s = s_p.at(j);
                sp_p.push_back(s);
            }

            r.insert({{i, i}});
            while(!sp_p.empty()){ // ln 13-17
                S = sp_p.back();
                min_x_rnk = -69;
                for(auto c : S){
                    x_rnk = ranker.getX(r.find(i)->second, c);
                    if(min_x_rnk == -69 || x_rnk < min_x_rnk){ min_x_rnk = x_rnk; sig = c; }
                }
                r.find(i)->second = ranker.getX(r.find(i)->second, sig);
                sp_p.back().erase(sig);
                if(sp_p.back().empty()){ sp_p.pop_back(); }
            }

            // T_X(`T`).`chld`(parent) <- [i, i)
        }
        // extend end point of T_X(`T`).`chld`(parent) by one
        if(nodes.count(i) != 0){
            nodes.find(i)->second->parent = nodes.find(parent)->second;
            nodes.find(parent)->second->children.push_back(nodes.find(i)->second);
            std::cout << "Set parent of " << i << " to " << parent << std::endl;
        }
    }

    // Clean up
    for(auto pr : nodes){
        if(pr.second->parent == nullptr && pr.second != root){
            pr.second->parent = root;
            root->children.push_back(pr.second);
            std::cout << "Set parent of " << pr.first << " to root" << std::endl;
        }
    }

    return root;
}

/**
 * @brief Builds Y-tree, given a pre-computed ranker table and shortlex result of the pattern string, as well as a text.
 * 
 * As like Algorithm 1 in paper, but reversed.
 * 
 * Note that this implementation accounts for slight *oversight* of ranker implementation.
 * The oversight is just that the Y-ranks are 1 bigger than the expected value.
 * If one is going to fix that oversight, fix this one too (vise versa).
 */
std::shared_ptr<Node> Trees::buildYTree(const RankerTable& ranker, const ShortlexResult& shortlex, const std::string& text) {
    std::shared_ptr<Node> root = std::make_shared<Node>(-1, -1, 0);
    std::unordered_map<int, std::shared_ptr<Node>> nodes;
    nodes.insert({{-1, root}});
    
    std::cout << "Building Y-tree...\n";

    // Copy out s_p (in reverse order) << not sure if reversing is mandatory
    std::vector<std::set<unsigned char>> s_p; std::set<unsigned char> s;
    for(int i = 0; i < shortlex.s_p.size(); i++){
        s = shortlex.s_p.at(shortlex.s_p.size() - i - 1);
        s_p.push_back(s);
    }

    // ln 4-6 (Slight rework as well)
    int deleted_chars = 0;
    for(int i = 0; i < shortlex.universality; i++){
        while(deleted_chars < shortlex.alph.size()){
            deleted_chars += s_p.back().size();
            s_p.pop_back();
        }
        deleted_chars = 0;
    }

    // ln 7-21
    int parent; int y_rnk; int max_y_rnk;
    std::vector<std::set<unsigned char>> sp_p;
    std::unordered_map<int, int> r; std::set<unsigned char> S;
    unsigned char sig;
    for(int i = static_cast<int>(text.size()); i > 0; i--){
        parent = std::numeric_limits<int>::max();
        for(auto a : shortlex.alph){
            y_rnk = ranker.getY(i, a);
            if(y_rnk < parent) parent = y_rnk;
        }

        // TODO: Fix Ranker implementation (`Ranker.cpp`) and remove this line
        //       or just leave both as they are
        if(parent != -1) parent--;

        // ln 9-18
        if(nodes.count(parent) == 0){
            nodes.insert({{parent, std::make_shared<Node>(parent, parent, 0)}});
            std::cout << "Generate new node " << parent << std::endl;

            // Copy out s_p (ln 11)
            for(int j = 0; j < s_p.size(); j++){
                s = s_p.at(j);
                sp_p.push_back(s);
            }

            r.insert({{i, i}});
            while(!sp_p.empty()){ // ln 13-17
                S = sp_p.back();
                max_y_rnk = std::numeric_limits<int>::max();
                for(auto c : S){
                    y_rnk = ranker.getY(r.find(i)->second, c);
                    if(max_y_rnk == std::numeric_limits<int>::max() || y_rnk > max_y_rnk){ max_y_rnk = y_rnk; sig = c; }
                }
                r.find(i)->second = ranker.getY(r.find(i)->second, sig);
                sp_p.back().erase(sig);
                if(sp_p.back().empty()){ sp_p.pop_back(); }
            }

            // T_X(`T`).`chld`(parent) <- [i, i)
        }
        // extend end point of T_X(`T`).`chld`(parent) by one
        if(nodes.count(i) != 0){
            nodes.find(i)->second->parent = nodes.find(parent)->second;
            nodes.find(parent)->second->children.push_back(nodes.find(i)->second);
            std::cout << "Set parent of " << i << " to " << parent << std::endl;
        }
    }

    // Clean up
    for(auto pr : nodes){
        if(pr.second->parent == nullptr && pr.second != root){
            pr.second->parent = root;
            root->children.push_back(pr.second);
            std::cout << "Set parent of " << pr.first << " to root" << std::endl;
        }
    }

    return root;
}
