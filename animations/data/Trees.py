from collections import deque
from typing import Dict, Set
from data.Node import Node


class Trees:
    @staticmethod
    def build_x_tree(ranker, shortlex, text: str) -> Node:
        print("Building X-tree...")
        INF = float('inf')
        root = Node(INF, INF, 0)
        nodes: Dict[int, Node] = {INF: root}

        # Copy s_p from shortlex.stackForm
        s_p = deque(set(block) for block in shortlex.stackForm)

        # ln 4-6: consume blocks based on arch ends
        last_arch = 0
        for end in shortlex.arch_ends:
            for j in range(end - last_arch):
                ch = shortlex.shortlexNormalForm[j + last_arch]
                s_p[-1].discard(ch)
                if not s_p[-1]:
                    s_p.pop()
            last_arch = end

        # ln 7-21: main construction
        r: Dict[int, int] = {}
        for i in range(len(text)):
            parent = -69
            for a in shortlex.alphabet:
                x_rnk = ranker.get_x(i, a)
                if x_rnk > parent:
                    parent = x_rnk

            if parent not in nodes:
                nodes[parent] = Node(parent, parent, 0)
                print(f"Generate new node {parent}")

                # Copy s_p to sp_p
                sp_p = deque(set(block) for block in s_p)
                r[i] = i

                while sp_p:
                    S = sp_p[-1]
                    min_x_rnk = None
                    sig = None
                    for c in S:
                        x_rnk = ranker.get_x(r[i], c)
                        if min_x_rnk is None or x_rnk < min_x_rnk:
                            min_x_rnk = x_rnk
                            sig = c
                    r[i] = ranker.get_x(r[i], sig)
                    S.discard(sig)
                    if not S:
                        sp_p.pop()

            if i in nodes:
                nodes[i].parent = nodes[parent]
                nodes[parent].children.append(nodes[i])
                print(f"Set parent of {i} to {parent}")

        # Clean up
        for i, node in nodes.items():
            if node.parent is None and node is not root:
                node.parent = root
                root.children.append(node)
                print(f"Set parent of {i} to root")

        return root

    @staticmethod
    def build_y_tree(ranker, shortlex, text: str) -> Node:
        print("Building Y-tree...")
        return Node(-1, len(text), 0)  # Placeholder like in the C++ version
