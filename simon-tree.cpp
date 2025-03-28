#include <iostream>
#include <vector>
#include <unordered_map>
#include <stack>

using namespace std;

struct SimonNode {
    int start, end;
    vector<SimonNode*> children;
    
    SimonNode(int s, int e) : start(s), end(e) {}
};

// next 배열을 생성하는 함수
vector<int> computeNext(const string &s) {
    int n = s.size();
    unordered_map<char, int> lastPos;
    vector<int> next(n, n);
    
    for (int i = n - 1; i >= 0; --i) {
        if (lastPos.count(s[i])) {
            next[i] = lastPos[s[i]];
        }
        lastPos[s[i]] = i;
    }
    
    return next;
}

// Simon Tree를 구성하는 함수
SimonNode* buildSimonTree(const string &s) {
    int n = s.size();
    vector<int> next = computeNext(s);
    SimonNode* root = new SimonNode(0, n);
    stack<SimonNode*> nodeStack;
    nodeStack.push(root);

    for (int i = n - 1; i >= 0; --i) {
        SimonNode* cur = nodeStack.top();
        
        // 현재 문자에 대한 새로운 블록을 생성할지 결정
        while (!nodeStack.empty() && next[i] < cur->end) {
            nodeStack.pop();
            if (!nodeStack.empty()) cur = nodeStack.top();
        }

        // 새로운 블록 추가
        SimonNode* newNode = new SimonNode(i, cur->end);
        cur->children.push_back(newNode);
        nodeStack.push(newNode);
    }
    
    return root;
}

// Simon Tree 출력 (디버깅용)
void printSimonTree(SimonNode* node, int depth = 0) {
    if (!node) return;
    cout << string(depth * 2, ' ') << "[" << node->start << " : " << node->end << "]" << endl;
    for (auto child : node->children) {
        printSimonTree(child, depth + 1);
    }
}

// 메인 함수
int main() {
    string s = "bacbaabada";
    SimonNode* tree = buildSimonTree(s);
    
    cout << "Simon Tree for '" << s << "':\n";
    printSimonTree(tree);

    return 0;
}