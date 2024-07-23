import argparse
import os
import re


def rename_files(directory, prefix):
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
            try:
                # Rename the file
                os.rename(old_file, new_file)
                # Increment the counter
                counter += 1
            except Exception as e:
                print(f"Error renaming file {old_file} to {new_file}: {e}")

    print("Renaming completed.")


if __name__ == "__main__":
    # Set up argument parser
    parser = argparse.ArgumentParser(
        description="Rename files in a directory with a specified prefix.")
    parser.add_argument('directory', type=str,
                        help='The directory containing the files to be renamed, e.g., ImagesDatasets/Images_dataset1')
    parser.add_argument('prefix', type=str,
                        help='The prefix for the new filenames, e.g., aromatic or notaromatic')

    # Parse the arguments
    args = parser.parse_args()

    # Verify the directory exists
    if not os.path.isdir(args.directory):
        print(f"Error: The directory {args.directory} does not exist.")
    else:
        # Call the rename_files function with the provided arguments
        rename_files(args.directory, args.prefix)
