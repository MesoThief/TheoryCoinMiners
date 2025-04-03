#ifndef TREES_H
#define TREES_H

#include <string>
#include <memory>
#include "Node.h"
#include "Ranker.h"
#include "Shortlex.h"

class Trees {
public:
    // Build X-tree using the X-ranker, ShortlexResult, and input text
    static std::shared_ptr<Node> buildXTree(const RankerTable& ranker, const ShortlexResult& shortlex, const std::string& text);

    // Build Y-tree using the Y-ranker, ShortlexResult, and input text
    static std::shared_ptr<Node> buildYTree(const RankerTable& ranker, const ShortlexResult& shortlex, const std::string& text);
};

#endif // TREES_H
