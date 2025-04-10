# data/Node.py

class Node:
    def __init__(self, start: int, end: int, depth: int, parent=None):
        self.start = start
        self.end = end
        self.depth = depth
        self.parent = parent  # type: Optional[Node]
        self.children = []    # type: List[Node]

    def is_root(self) -> bool:
        return self.parent is None

    def is_leaf(self) -> bool:
        return len(self.children) == 0

    @classmethod
    def from_node(cls, other_node: 'Node') -> 'Node':
        """Alternative constructor mimicking C++ copy constructor taking shared_ptr."""
        return cls(other_node.start, other_node.end, other_node.depth, other_node.parent)
