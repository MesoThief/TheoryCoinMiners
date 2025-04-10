class Alphabet:
    _instance = None

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super(Alphabet, cls).__new__(cls)
            cls._instance._init()
        return cls._instance

    def _init(self):
        self.alphabet = "abcdefghijklmnopqrstuvwxyz"
        self.index_map = {c: i for i, c in enumerate(self.alphabet)}

    @classmethod
    def get_instance(cls):
        return cls()

    def set_alphabet(self, sigma: str):
        self.alphabet = sigma
        self.index_map = {c: i for i, c in enumerate(self.alphabet)}

    def size(self) -> int:
        return len(self.alphabet)

    def index_to_char(self, index: int) -> str:
        return self.alphabet[index]

    def char_to_index(self, c: str) -> int:
        return self.index_map[c]
