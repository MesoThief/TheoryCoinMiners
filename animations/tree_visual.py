from manim import *
from data.Ranker import RankerTable
from data.Trees import Trees
from data.Shortlex import compute_partial_shortlex_normal_form
from utils.RandomTextGenerator import generate_random_text
import numpy as np

class HorizontalTreeVisualizer(Scene):
    def construct(self):
        # Parameters
        alphabet_size = 3
        rand_text = "abcbacaabacccbaccaab"  # input string from paper-style example
        base_y = 0

        # Create character labels as VGroup and animate in
        char_labels = VGroup(*[
            Text(c, font_size=24) for c in rand_text
        ]).arrange(RIGHT, buff=0.4).move_to([0, base_y, 0])
        self.play(Write(char_labels))

        # Create title
        title = Text("Input String", font_size=16).next_to(char_labels, LEFT)
        self.play(FadeIn(title))
        self.wait()

        # Store positions for aligning trees later
        char_positions = [label.get_center()[0] for label in char_labels]

        # 2. Build X-ranker, Y-ranker, and shortlex guide
        ranker = RankerTable(rand_text, alphabet_size)
        ranker.build_x_ranker_table()
        ranker.build_y_ranker_table()

        # Generate a shortlex pattern (not shown — used internally to build trees)
        shortlex = compute_partial_shortlex_normal_form(
            rand_text,
            [1] * alphabet_size,
            [1] * alphabet_size,
            2
        )

        # 3. Build trees (from original string and shortlex structure)
        x_root = Trees.build_x_tree(ranker, shortlex, rand_text)
        y_root = Trees.build_y_tree(ranker, shortlex, rand_text)

        # 4. Visualize trees using X-tree (for example)
        self.animate_tree_layout(x_root, char_positions, base_y=0, color=RED)
        self.wait()
        # self.animate_tree_layout(y_root, char_positions, base_y=0, color=BLUE)

    def animate_tree_layout(self, root, char_positions, base_y, color):
        # Dictionaries to store the created node animations if needed later
        node_mobs = {}

        # Parameters for layout
        level_spacing = 0.3  # Vertical spacing per level
        font_size = 18
        initial_y_offset = 1  # How far above the string to start

        def animate_node(node, depth, y_offset):
            # Determine the x position:
            # If the node's label is "inf", we put it to the right of the last character.
            y = y_offset
            if str(node.start) == "inf":
                x = char_positions[-1] + 1  # offset to the right of the last character
                
                # Create the circle and its label
                print("Printing root")
                circle = Circle(radius=0.3, color=WHITE).move_to([x, y, 0])
                label_text = Text(str(node.start), font_size=font_size, color=WHITE).move_to([x, y, 0])
                group = VGroup(circle, label_text)
                self.play(Create(group), run_time=0.5)
                node_mobs[node] = group
            else:
                # Assume node.start is 1-indexed, so subtract 1 for indexing char_positions
                x = char_positions[int(node.start) - 1]
            

            # Draw children
            count = len(node.children)
            for i in range(count):
                child = node.children[i]
                print("Printing child", child.start)
                sibling_offset = 0.5 * (count // 2 - i)

                child_x = char_positions[int(child.start) - 1]
                child_y = y_offset + sibling_offset

                # Create the circle and its label
                circle = Circle(radius=0.3, color=WHITE).move_to([child_x, child_y, 0])
                label_text = Text(str(child.start), color=WHITE, font_size=font_size).move_to([child_x, child_y, 0])
                group = VGroup(circle, label_text)
                self.play(Create(group), run_time=0.5)
                node_mobs[node] = group

                parent = Circle(radius=0.3, color=WHITE).move_to([x, y, 0])

                # Draw a line from the current node to the child's expected position.
                arrow = Line(start=group.get_right(), end=parent.get_left(), color=color).add_tip(at_start=False, tip_width=0.2, tip_length=0.2)
                self.play(Create(arrow), run_time=0.5)

                animate_node(child, depth + 1, child_y)

        animate_node(root, 0, initial_y_offset)