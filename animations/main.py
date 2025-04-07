import random
from data.Ranker import RankerTable
from data.Trees import Trees
from data.Shortlex import compute_partial_shortlex_normal_form
from utils.RandomTextGenerator import generate_random_text
from utils.CalculateUniversality import calculate_universality_index

def format_x_ranker_output(ranker, text_length, alphabet_size):
    lines = ["X-ranker (next position +1 for each char):"]
    for i in range(text_length + 1):
        row = []
        for j, c in enumerate(chr(ord('a') + k) for k in range(alphabet_size)):
            val = ranker.get_x(i, c)
            val_str = "INF" if val == float('inf') else str(val)
            entry = f"X({i}, {c}) = {val_str}"
            # Pad all columns except the last one
            if j < alphabet_size - 1:
                row.append(f"{entry:<16}")
            else:
                row.append(entry)
        lines.append("".join(row))
    return "\n".join(lines)

def main():
    # Set alphabet size and text length
    alphabet_size = 5
    text_length = 20

    # Generate random text
    rand_text = 'eeacccbcbdbebcdcabdc' # generate_random_text(alphabet_size, text_length)
    print(f"Random text: {rand_text}")

    # Build both X-ranker and Y-ranker tables
    ranker = RankerTable(rand_text, alphabet_size)
    ranker.build_x_ranker_table()
    ranker.build_y_ranker_table()

    # Load C++ X-ranker block from file
    with open("./animations/check_x_ranker.txt", "r") as f:
        cpp_x_ranker = f.read().strip()

    # Generate Python X-ranker block
    py_x_ranker = format_x_ranker_output(ranker, len(rand_text), alphabet_size)

    # Optional: write to file for debugging/diffing
    with open("./animations/py_x_ranker.txt", "w") as f:
        f.write(py_x_ranker)

    # Compare and assert
    if cpp_x_ranker == py_x_ranker:
        print("✅ X-ranker output matches exactly with C++.")
    else:
        print("❌ X-ranker mismatch detected!")
        print("Check /animations/py_x_ranker.txt for the generated Python output.")

    # Display X-ranker table
    print("\nX-ranker (next position +1 for each char):")
    for i in range(text_length + 1):
        for c in (chr(ord('a') + j) for j in range(alphabet_size)):
            result = ranker.get_x(i, c)
            display = "INF" if result == RankerTable.INF else str(result)
            print(f"X({i}, {c}) = {display}", end="\t")
        print()

    # Display Y-ranker table
    print("\nY-ranker (previous position for each char):")
    for i in range(text_length + 1):
        for c in (chr(ord('a') + j) for j in range(alphabet_size)):
            result = ranker.get_y(i, c)
            print(f"Y({i}, {c}) = {result}", end="\t")
        print()

    # Generate random pattern
    pattern_length = 6
    congruence_class = 2
    rand_pattern = generate_random_text(alphabet_size, pattern_length)

    # Calculate universality index
    universality = calculate_universality_index(rand_pattern, alphabet_size)

    # Compute Shortlex form of pattern
    shortlex_pattern = compute_partial_shortlex_normal_form(
        rand_pattern,
        [1] * alphabet_size,
        [1] * alphabet_size,
        congruence_class
    )

    # Build X-tree and Y-tree
    x_root = Trees.build_x_tree(ranker, shortlex_pattern, rand_text)
    y_root = Trees.build_y_tree(ranker, shortlex_pattern, rand_text)

if __name__ == "__main__":
    main()
