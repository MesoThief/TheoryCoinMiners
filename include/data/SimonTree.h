#ifndef SIMON_TREE_H
#define SIMON_TREE_H

#include <string>
#include <limits>

#include "Node.h"

using namespace std;

vector<int> computeX(const string& w);
shared_ptr<Node> findNode(int i, const vector<int>& X, shared_ptr<Node> a);
pair<shared_ptr<Node>, shared_ptr<Node>> splitNode(int i, shared_ptr<Node> root, shared_ptr<Node> a);
void fixTree(shared_ptr<Node> root, int n);
void printTree(const shared_ptr<Node>& node, const string& w, string indent = "", bool isLast = true);

#endif // SIMON_TREE_H