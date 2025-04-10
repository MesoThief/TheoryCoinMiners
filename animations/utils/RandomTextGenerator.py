import random

def generate_random_text(alphabet_size: int, length: int) -> str:
    base_alphabet = "abcdefghijklmnopqrstuvwxyz"

    if not (1 <= alphabet_size <= 26):
        raise ValueError("Alphabet size must be between 1 and 26.")

    alphabet = base_alphabet[:alphabet_size]
    return ''.join(random.choice(alphabet) for _ in range(length))
