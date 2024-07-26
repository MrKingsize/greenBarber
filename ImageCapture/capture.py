import argparse

import cv2

# Set up argument parser
parser = argparse.ArgumentParser(
    description='Capture an image from the camera.')
parser.add_argument('--device_id', type=int, default=0,
                    help='ID of the camera device (default: 0)')
parser.add_argument('--output_file', type=str, default='output.jpg',
                    help='File name for the captured image (default: output.jpg)')
parser.add_argument('--frame_width', type=int, default=1920,
                    help='Frame width (default: 1920)')
parser.add_argument('--frame_height', type=int, default=1080,
                    help='Frame height (default: 1080)')

# Parse arguments
args = parser.parse_args()

# Open the device at the specified ID
cap = cv2.VideoCapture(args.device_id)

# Check if the camera was opened correctly
if not cap.isOpened():
    print(f"Could not open video device with ID {args.device_id}")
    exit()

# Set the resolution
cap.set(cv2.CAP_PROP_FRAME_WIDTH, args.frame_width)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, args.frame_height)

# Read and capture the current frame
ret, frame = cap.read()

if ret:
    # Save the captured frame to a file
    cv2.imwrite(args.output_file, frame)
    print(f"Image saved as {args.output_file}")
else:
    print("Failed to capture image")

# When everything is done, release the capture
cap.release()
cv2.destroyAllWindows()
