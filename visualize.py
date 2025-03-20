import matplotlib.pyplot as plt
import matplotlib.patches as patches
import math
import sys, os

def read_rectangles(filename):
    rectangles = []
    with open(filename, 'r') as file:
        lines = file.readlines()[:-1]
        for line in lines:
            # Convert coordinates to ints and group them into (x, y) pairs
            coords = line.strip()[1:-1].split(') (')
            rectangle = []
            for coord in coords:
                rectangle.append([int(s) for s in coord.split(' ')]) 
            rectangles.append(rectangle)
    return rectangles

def sort_vertices(vertices):
    # Calculate the center point
    center_x = sum(point[0] for point in vertices) / len(vertices)
    center_y = sum(point[1] for point in vertices) / len(vertices)
    center = (center_x, center_y)
    
    # Calculate the angle of each vertex relative to the center
    def angle_from_center(point):
        return math.atan2(point[1] - center[1], point[0] - center[0])
    
    # Sort vertices in clockwise order
    sorted_vertices = sorted(vertices, key=angle_from_center, reverse=True)
    return sorted_vertices

def plot_rectangles(rectangles, filename, show_coords=False):
    fig, ax = plt.subplots()
    
    for i, rectangle in enumerate(rectangles):
        # Sort the vertices to ensure they are in clockwise or counterclockwise order
        sorted_rectangle = sort_vertices(rectangle)
        # Draw the rectangle
        rect = patches.Polygon(sorted_rectangle, closed=True, edgecolor='blue', facecolor='none')
        ax.add_patch(rect)

        # Calculate the center of the rectangle
        center_x = sum(point[0] for point in rectangle) / len(rectangle)
        center_y = sum(point[1] for point in rectangle) / len(rectangle)
        
        # Display the number i at the center
        ax.text(center_x, center_y, str(i), fontsize=12, ha='center', va='center', color='red')

        # Display the coordinates at each vertex
        if show_coords:
            for point in rectangle:
                ax.text(point[0], point[1], f'({point[0]}, {point[1]})', fontsize=10, ha='center', va='bottom', color='green')
    
    # Set the axis limits
    all_x = [point[0] for rect in rectangles for point in rect]
    all_y = [point[1] for rect in rectangles for point in rect]
    ax.set_xlim(min(all_x) - 1, max(all_x) + 1)
    ax.set_ylim(min(all_y) - 1, max(all_y) + 1)
    
    # Save the figure
    plt.gca().set_aspect('equal', adjustable='box')
    plt.savefig(filename, dpi=300)

if __name__ == "__main__":
    argc = len(sys.argv)
    if argc != 2 and argc != 3:
        print("Usage: python visualize.py <result.txt> [<result.png>]")
        exit(0)

    input_file = sys.argv[1]
    output_file = os.path.splitext(input_file)[0] + ".png" if argc != 3 else sys.argv[2]

    # Read rectangle coordinates
    rectangles = read_rectangles(input_file)

    # Plot the rectangles
    plot_rectangles(rectangles, output_file)
