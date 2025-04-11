#ifndef SIMON_TREE_H
#define SIMON_TREE_H

#include <memory>
#include <string>
#include <vector>

using namespace std;

namespace SimonTree {
  struct Node {
    int start;
    int end;
    int depth;
    shared_ptr<Node> parent = nullptr;
    vector<shared_ptr<Node>> children;

    Node(int start, int end, int depth);
    Node(int start, int end, int depth, shared_ptr<Node> parent);
    
    bool isRoot() const;
    bool isLeaf() const;
  };

  vector<int> computeX(const string& w);
  shared_ptr<Node> findNode(int i, const vector<int>& X, shared_ptr<Node> a);
  pair<shared_ptr<Node>, shared_ptr<Node>> splitNode(int i, shared_ptr<Node> root, shared_ptr<Node> a);
  void fixTree(shared_ptr<Node> root, int n);
  void printTree(const shared_ptr<Node>& node, const string& w, string indent = "", bool isLast = true);
}
#endif // SIMON_TREE_H