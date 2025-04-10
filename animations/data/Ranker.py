# data/Ranker.py

class RankerTable:
    INF = float('inf')

    def __init__(self, text: str, alphabet_size: int):
        self.text = text
        self.alphabet_size = alphabet_size
        self.char_to_index_map = {chr(ord('a') + i): i for i in range(alphabet_size)}

        n = len(text)
        self.x_table = [[RankerTable.INF] * alphabet_size for _ in range(n + 1)]
        self.y_table = [[-1] * alphabet_size for _ in range(n + 1)]

    def char_to_index(self, c: str) -> int:
        if c not in self.char_to_index_map:
            raise ValueError(f"Character '{c}' not in alphabet")
        return self.char_to_index_map[c]

    def build_x_ranker_table(self):
        n = len(self.text)
        next_pos = [RankerTable.INF] * self.alphabet_size

        for i in range(n - 1, -1, -1):
            next_pos[self.char_to_index(self.text[i])] = i + 1
            for c in range(self.alphabet_size):
                self.x_table[i][c] = next_pos[c]

    def build_y_ranker_table(self):
        n = len(self.text)
        prev = [-1] * self.alphabet_size

        for i in range(n):
            prev[self.char_to_index(self.text[i])] = i + 1
            for c in range(self.alphabet_size):
                self.y_table[i + 1][c] = prev[c]

    def get_x(self, index: int, c: str) -> int:
        return self.x_table[index][self.char_to_index(c)]

    def get_y(self, index: int, c: str) -> int:
        return self.y_table[index][self.char_to_index(c)]
