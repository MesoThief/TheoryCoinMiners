from collections import deque
from typing import List, Set, Deque, Dict
from utils.Alphabet import Alphabet


class ShortlexResult:
    def __init__(self):
        self.shortlexNormalForm: str = ""
        self.X_vector: List[int] = []
        self.Y_vector: List[int] = []
        self.stackForm: Deque[Set[str]] = deque()
        self.arch_ends: List[int] = []
        self.alphabet: Set[str] = set()
        self.universality: int = 0


def compute_shortlex_normal_form(w: str, k: int) -> str:
    n = len(w)
    ALPHABET_SIZE = Alphabet.get_instance().size()

    X = [0] * n
    Y = [0] * n
    shortlex_form = ""
    shortlex_x = deque()
    shortlex_y = deque()
    counter = [1] * ALPHABET_SIZE

    # Compute X-coordinates (left to right)
    for i, c in enumerate(w):
        idx = Alphabet.get_instance().char_to_index(c)
        X[i] = counter[idx]
        counter[idx] += 1
        counter = [min(x, counter[idx]) for x in counter]

    # Compute Y-coordinates (right to left)
    counter = [1] * ALPHABET_SIZE
    for i in reversed(range(n)):
        c = w[i]
        idx = Alphabet.get_instance().char_to_index(c)
        if X[i] + counter[idx] <= k + 1:
            Y[i] = counter[idx]
            counter[idx] += 1
            counter = [min(x, counter[idx]) for x in counter]
            shortlex_form = c + shortlex_form
            shortlex_x.appendleft(X[i])
            shortlex_y.appendleft(Y[i])

    # Recompute X based on shortlex form
    m = len(shortlex_form)
    counter = [1] * ALPHABET_SIZE
    for i in range(m):
        c = shortlex_form[i]
        idx = Alphabet.get_instance().char_to_index(c)
        shortlex_x.append(counter[idx])
        counter[idx] += 1
        counter = [min(x, counter[idx]) for x in counter]

    # Lex reorder blocks
    start = 0
    shortlex_form = list(shortlex_form)
    while start < m:
        end = start + 1
        while (end < m and
               shortlex_x[end] == shortlex_x[start] and
               shortlex_y[end] == shortlex_y[start] and
               shortlex_x[start] + shortlex_y[start] == k + 1):
            end += 1
        if end - start > 1:
            shortlex_form[start:end] = sorted(shortlex_form[start:end])
        start = end

    return ''.join(shortlex_form)


def compute_partial_shortlex_normal_form(w: str, X_vector: List[int], Y_vector: List[int], threshold: int) -> ShortlexResult:
    n = len(w)
    ALPHABET_SIZE = Alphabet.get_instance().size()
    w_alphabet = set(w)

    X = [0] * n
    Y = [0] * n
    shortlex_form = ""
    shortlex_x = deque()
    shortlex_y = deque()
    new_X_vector = X_vector[:]
    new_Y_vector = Y_vector[:]

    # 1. Compute X-coordinates
    for i, c in enumerate(w):
        idx = Alphabet.get_instance().char_to_index(c)
        X[i] = X_vector[idx]
        X_vector[idx] += 1
        X_vector = [min(x, X_vector[idx]) for x in X_vector]

    # 2. Compute Y-coordinates and build shortlex form
    for i in reversed(range(n)):
        c = w[i]
        idx = Alphabet.get_instance().char_to_index(c)
        Y[i] = Y_vector[idx]
        if X[i] + Y[i] <= threshold:
            Y_vector[idx] += 1
            Y_vector = [min(y, Y_vector[idx]) for y in Y_vector]
            shortlex_form = c + shortlex_form
            shortlex_y.appendleft(Y[i])

    # 3. Recompute X for shortlex form
    m = len(shortlex_form)
    for i in range(m):
        c = shortlex_form[i]
        idx = Alphabet.get_instance().char_to_index(c)
        shortlex_x.append(new_X_vector[idx])
        new_X_vector[idx] += 1
        new_X_vector = [min(x, new_X_vector[idx]) for x in new_X_vector]
    new_Y_vector = Y_vector[:]

    # 4. Lexicographic reorder and stackForm
    start = 0
    shortlex_form = list(shortlex_form)
    stack_form = deque()
    alphabet_track = set()
    arch_ends = []

    while start < m:
        end = start + 1
        while (end < m and
               shortlex_x[end] == shortlex_x[start] and
               shortlex_y[end] == shortlex_y[start] and
               shortlex_x[start] + shortlex_y[start] == threshold):
            end += 1
        if end - start > 1:
            shortlex_form[start:end] = sorted(shortlex_form[start:end])

        block = set(shortlex_form[start:end])
        stack_form.appendleft(block)
        alphabet_track.update(block)
        if len(alphabet_track) == len(w_alphabet):
            arch_ends.append(end)
            alphabet_track.clear()

        start = end

    result = ShortlexResult()
    result.shortlexNormalForm = ''.join(shortlex_form)
    result.X_vector = new_X_vector
    result.Y_vector = new_Y_vector
    result.stackForm = stack_form
    result.arch_ends = arch_ends
    result.alphabet = w_alphabet
    result.universality = len(arch_ends)
    return result
