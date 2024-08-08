# Image Capture

This is a simple image capture program that uses the OpenCV library to capture images from a webcam device.

## Requirements

- Python 3.6 or higher
- OpenCV library (cv2)

## Usage

```bash
python3 capture.py --help # Display help message
python3 capture.py --output <output_path> # Capture image and save it to the output path
python3 capture.py --device_id <device_id> --output <output_path> # Capture image from the specified device and save it to the output path
```

Check the help message for more information on how to use this program.

## Install v4l-utils

```bash
# Install v4l-utils
sudo apt install v4l-utils
# List attached devices
$ v4l2-ctl --list-devices
# List all info about a given device
$ v4l2-ctl --all -d /dev/videoX
# Where X is the device number. For example:
$ v4l2-ctl --all -d /dev/video0
# List the cameras pixel formats, images sizes, frame rates
$ v4l2-ctl --list-formats-ext -d /dev/videoX
```
