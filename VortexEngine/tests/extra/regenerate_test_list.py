################################################################################
# Use this script to regenerate the test_list file from the active test files
# This is useful if you made some new tests and want to update test_list
import os
import re

# Define the directory containing the .test files
directory = '../tests_general/'

# Get all test files sorted by the numerical part of their names
file_list = [f for f in os.listdir(directory) if f.endswith('.test')]
file_list.sort(key=lambda x: int(re.findall(r'\d+', x)[0]))

# Prepare to collect all lines for the new formatted file
lines = []

# Longest length placeholders for formatting
max_input_len = 0
max_brief_len = 0
max_filename_len = 0

# Process each file
for filename in file_list:
    with open(os.path.join(directory, filename), 'r') as file:
        content = file.readlines()
        input_command = content[0].strip().split('=')[1]
        brief = content[1].strip().split('=')[1]

        # Remove the extension and leading number from filename, and replace underscores with spaces
        formatted_filename = re.sub(r'^\d+_', '', filename[:-5]).replace('_', ' ')

        # Update max lengths for formatting
        max_input_len = max(max_input_len, len(input_command))
        max_brief_len = max(max_brief_len, len(brief))
        max_filename_len = max(max_filename_len, len(formatted_filename))

        # Add formatted line to the list
        lines.append((input_command, formatted_filename, brief))

# Generate the final formatted output
formatted_lines = []
for input_command, formatted_filename, brief in lines:
    formatted_line = f"{input_command.ljust(32)} | {formatted_filename.ljust(64)} | {brief}"
    formatted_lines.append(formatted_line)

# Path to save the formatted output
with open('test_list', 'w') as output_file:
    for line in formatted_lines:
        output_file.write(line + '\n')
