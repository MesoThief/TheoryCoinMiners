def calculate_universality_index(text: str, alphabet_size: int) -> int:
    if not (1 <= alphabet_size <= 26):
        raise ValueError("Alphabet size must be between 1 and 26.")

    arch_count = 0
    unique_count = 0
    left = 0
    n = len(text)
    window_count = [0] * alphabet_size

    for right in range(n):
        idx = ord(text[right]) - ord('a')

        # expand window
        if window_count[idx] == 0:
            unique_count += 1
        window_count[idx] += 1

        # check if window is a minimal arch
        if unique_count == alphabet_size:
            arch_count += 1
            right += 1
            left = right
            window_count = [0] * alphabet_size
            unique_count = 0

    return arch_count
