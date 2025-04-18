import argparse
import os
from PIL import Image
import svgwrite
import math

def create_circular_pattern_svg(path_to_bmp, output_file):
    print(f"-- Creating circular pattern for {path_to_bmp}")
    pattern_strip = Image.open(path_to_bmp).convert('RGBA')

    canvas_size = 1000
    center = canvas_size // 2
    outer_radius = int(canvas_size * 0.45)
    thickness = 50
    num_rings = 3
    gap_between_rings = thickness * 0.5

    dwg = svgwrite.Drawing(output_file, size=(canvas_size, canvas_size))

    for ring in range(num_rings):
        inner_radius = outer_radius - thickness - (ring * (thickness + gap_between_rings))
        outer_r = inner_radius + thickness

        current_color = None
        start_angle = 0

        for angle in range(361):  # Go to 361 to close the loop
            pattern_index = angle % pattern_strip.width
            color = pattern_strip.getpixel((pattern_index, 0))

            if len(color) == 3:  # RGB
                fill_color = f'rgb{color}'
                opacity = 1
            elif len(color) == 4:  # RGBA
                fill_color = f'rgb{color[:3]}'
                opacity = color[3] / 255.0

            if color[:3] == (0, 0, 0):  # Black color
                fill_color = None
                opacity = 0

            if fill_color != current_color or angle == 360:
                if current_color is not None:
                    end_angle = angle
                    path = create_arc_path(center, inner_radius, outer_r, start_angle, end_angle)
                    dwg.add(dwg.path(d=path, fill=current_color, fill_opacity=current_opacity))

                current_color = fill_color
                current_opacity = opacity
                start_angle = angle

    print(f"-- Pattern mapped successfully")
    dwg.save()
    print(f"** Saved circular pattern to {output_file}")

def create_arc_path(center, inner_radius, outer_radius, start_angle, end_angle):
    start_angle_rad = math.radians(start_angle - 90)
    end_angle_rad = math.radians(end_angle - 90)

    x1 = center + inner_radius * math.cos(start_angle_rad)
    y1 = center + inner_radius * math.sin(start_angle_rad)
    x2 = center + outer_radius * math.cos(start_angle_rad)
    y2 = center + outer_radius * math.sin(start_angle_rad)
    x3 = center + outer_radius * math.cos(end_angle_rad)
    y3 = center + outer_radius * math.sin(end_angle_rad)
    x4 = center + inner_radius * math.cos(end_angle_rad)
    y4 = center + inner_radius * math.sin(end_angle_rad)

    large_arc_flag = 1 if end_angle - start_angle > 180 else 0

    path = f'M {x1} {y1} '
    path += f'L {x2} {y2} '
    path += f'A {outer_radius} {outer_radius} 0 {large_arc_flag} 1 {x3} {y3} '
    path += f'L {x4} {y4} '
    path += f'A {inner_radius} {inner_radius} 0 {large_arc_flag} 0 {x1} {y1}'

    return path

def main():
    parser = argparse.ArgumentParser(description="Generate a circular pattern from a BMP file as SVG.")
    parser.add_argument("bmp_file", type=str, help="Path to the BMP file")
    parser.add_argument("output_file", type=str, help="Path to the output SVG file")
    args = parser.parse_args()

    create_circular_pattern_svg(args.bmp_file, args.output_file)

if __name__ == "__main__":
    main()