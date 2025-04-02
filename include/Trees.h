#ifndef TREES_H
#define TREES_H

#include <string>
#include <memory>
#include "Ranker.h"
#include "shortlex.hpp"
#include "Node.h"

class Trees {
public:
    // Build X-tree using the X-ranker, ShortlexResult, and input text
    static std::shared_ptr<Node> buildXTree(const RankerTable& xRanker, const ShortlexResult& shortlex, const std::string& text);

    // Build Y-tree using the Y-ranker, ShortlexResult, and input text
    static std::shared_ptr<Node> buildYTree(const RankerTable& yRanker, const ShortlexResult& shortlex, const std::string& text);
};

#endif // TREES_H
