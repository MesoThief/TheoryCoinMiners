from typing import Tuple
from manim import *
from data.Node import Node
from data.Ranker import RankerTable
from data.Trees import Trees
from data.Shortlex import compute_partial_shortlex_normal_form
from utils.RandomTextGenerator import generate_random_text
import numpy as np

class HorizontalTreeVisualizer(Scene):
    def construct(self) -> None:
        # Parameters
        alphabet_size = 3
        rand_text = "abcbacaabacccbaccaab"  # input string from paper-style example
        base_y = 0

        # Create character labels as VGroup and animate in
        char_labels = VGroup(*[
            Text(c, font_size=24) for c in rand_text
        ]).arrange(RIGHT, buff=0.4).move_to([0, base_y, 0])
        for label in char_labels:
            label.align_to(char_labels[0], DOWN)  # or CENTER
        self.play(Write(char_labels))

        # Create title
        title = Text("Input String", font="consolas", font_size=16).next_to(char_labels, LEFT)
        self.play(FadeIn(title))
        self.wait()

        # Store positions for aligning trees later
        char_positions = [label.get_center()[0] for label in char_labels]
        char_y = label.get_center()[1]

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

        # 4. Visualize trees
        self.animate_tree_layout(x_root, char_positions, base_y=1, char_y=char_y, color=RED, inverse_color=BLUE)
        # self.wait()
        self.animate_tree_layout(y_root, char_positions, base_y=-1, char_y=char_y, color=BLUE, inverse_color=RED)

    def animate_tree_layout(self, root, char_positions, base_y, char_y, color, inverse_color) -> None:
        # Dictionaries to store the created node animations if needed later
        self.node_mobs = {}

        # Parameters for layout
        font_size = 18
        initial_y_offset = base_y  # How far above the string to start

        def animate_node(node: Node, depth: Integer, y_offset: np.float64) -> None:
            # Determine the x position:
            # If the node's label is "inf", we put it to the right of the last character.
            y = y_offset
            char_offset = char_positions[1] - char_positions[0]
            half_offset = char_offset / 2

            if str(node.start) == "inf":
                x = char_positions[-1] + char_offset + half_offset  # offset to the right of the last character
                
                # Create the circle and its label
                print("Printing root")
                self.create_node_at(x, y, node.start, font_size)
            elif node.start == -1:
                x = char_positions[0] - 2 * char_offset + half_offset  # offset to the left of the first character
                
                # Create the circle and its label
                print("Printing root")
                self.create_node_at(x, y, node.start, font_size)
            elif node.start == 0:
                x = char_positions[0] - char_offset + half_offset # virtual -1 position
            else:
                # Assume node.start is 1-indexed, so subtract 1 for indexing char_positions
                x = char_positions[int(node.start) - 1] + half_offset

            sibling_offset = 0.0
            step = 0.35 #if base_y > 0 else -0.5

            # Draw children
            count = len(node.children)
            for i in range(count):
                child = node.children[i]
                print("Printing child", child.start)

                if child.start == 0:
                    child_x = char_positions[0] - char_offset + half_offset
                else:
                    child_x = char_positions[int(child.start) - 1] + half_offset
                child_y = y_offset + sibling_offset

                # Create the circle and its label
                circle, label_text, group = self.create_node_at(child_x, child_y, child.start, font_size)

                parent = Circle(radius=0.2, color=WHITE).move_to([x, y, 0])

                # Draw a line from the current node to the child's expected position.
                start = group.get_right() if base_y > 0 else group.get_left()
                parent_center = parent.get_center()
                radius = 0.2  # same as your Circle radius

                if base_y < 0:
                    dashed_start = circle.get_top()
                else:
                    dashed_start = circle.get_bottom()
                dashed = DashedLine(dashed_start, [child_x, char_y, 0], color=inverse_color, dash_length=0.1, dashed_ratio=0.6)
                self.play(Create(dashed), run_time=0.3)

                # Compute the approx angle of line incident to parent circle
                if y == child_y:
                    if base_y < 0:
                        angle = 0.0
                    else:
                        angle = np.pi
                elif y > child_y:
                    angle = -np.pi * 3 / 4 if base_y > 0 else -np.pi / 4
                else:
                    angle = np.pi * 3 / 4 if base_y > 0 else np.pi / 4

                # Compute the point on the edge of the circle at that angle
                end = parent_center + radius * np.array([np.cos(angle), np.sin(angle), 0])
                arrow = self.curved_arrow_from_parent_to_child(start, end, color=color)
                self.play(Create(arrow), run_time=0.5)

                # Update the sibling offset
                sibling_offset -= step

                if(sibling_offset == -step * ((count + 1)// 2)):
                   sibling_offset = step * (count // 2)
                
                animate_node(child, depth + 1, child_y)

        animate_node(root, 0, initial_y_offset)

    def create_node_at(self, x: np.float64, y: np.float64, label: any, font_size=24) -> Tuple[Circle, Label, VGroup]:
        circle = Circle(radius=0.2, color=WHITE).move_to([x, y, 0])
        label_text = Text(str(label), font_size=font_size, color=WHITE).move_to([x, y, 0])
        group = VGroup(circle, label_text)
        self.play(Create(group), run_time=0.5)
        self.node_mobs[label] = group

        return circle, label_text, group
    
    def curved_arrow_from_parent_to_child(self, start: np.ndarray, end: np.ndarray, color=WHITE, tip_shape=StealthTip) -> VGroup:
        child = np.array(start)
        parent = np.array(end)

        dx = parent[0] - child[0]
        dy = parent[1] - child[1]

        # If nearly same y level → straight horizontal line
        if abs(dy) < 1e-3:
            line = Line(child, parent, color=color)
            tip = tip_shape().scale(0.3).set_color(color).rotate(line.get_angle()).move_to(parent)
            return VGroup(line, tip)

        # Compute control points dynamically based on dx/dy
        # First control: further along horizontally, minimal Y change (delayed curve)
        ctrl1 = child + np.array([dx * 0.75, dy * 0.05, 0])  # Almost flat, mostly horizontal

        # Second control: near parent, with sharper angle toward it
        ctrl2 = parent - np.array([dx * 0.1, dy * 0.4, 0])  # Curve kicks in late, near end

        # Bezier curve
        curve = CubicBezier(child, ctrl1, ctrl2, parent).set_color(color)

        # Compute arrow tip direction
        tangent = curve.point_from_proportion(1) - curve.point_from_proportion(0.99)
        angle = np.arctan2(tangent[1], tangent[0])
        tip = tip_shape().scale(0.3).set_color(color).rotate(angle).move_to(parent)

        return VGroup(curve, tip)