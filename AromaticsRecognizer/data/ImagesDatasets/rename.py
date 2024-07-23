import os
import re

# Specify the directory containing the files
directory = 'Images_dataset2/Aromatics'  # Change this to your directory
prefix = 'aromatic'  # Change this to your desired prefix

# Pattern to match the original filenames
pattern = re.compile(r'^Captura de ecrã \d{4}-\d{2}-\d{2} \d{6}\.png$')

# Get a list of files in the directory
files = os.listdir(directory)

# Initialize a counter for the new filenames
counter = 1

# Loop through the files and rename them
for filename in files:
    if pattern.match(filename):
        # Construct the new filename
        new_filename = f'{prefix}_{counter}.png'
        # Get the full path for the current and new filenames
        old_file = os.path.join(directory, filename)
        new_file = os.path.join(directory, new_filename)
        # Rename the file
        os.rename(old_file, new_file)
        # Increment the counter
        counter += 1

print("Renaming completed.")
