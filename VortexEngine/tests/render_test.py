import sys

def render_color(color_code):
    red = int(color_code[0:2], 16)
    green = int(color_code[2:4], 16)
    blue = int(color_code[4:6], 16)
    return f"[\033[48;2;{red};{green};{blue}m  \033[0m]"

def process_file(filename):
    with open(filename, 'r') as file:
        in_color_section = False
        for line in file:
            if line.strip() == '-' * len(line.strip()):
                in_color_section = True
                print(line, end='')
                continue
            if in_color_section:
                for i in range(0, len(line) - 1, 6):
                    color_code = line[i:i+6]
                    print(render_color(color_code), end='')
                print()
            else:
                print(line, end='')

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python render_colors.py <filename>")
        sys.exit(1)
    process_file(sys.argv[1])

