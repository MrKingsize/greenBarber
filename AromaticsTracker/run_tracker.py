import sys

import tensorflow as tf
import tensorflow_hub as hub

from tracker import filter_results, load_img, run_detector

if len(sys.argv) < 2:
    print("Usage: python run_tracker.py <image_path>")
    sys.exit(1)

img_path = sys.argv[1]

# Check if file exists
try:
    with open(img_path, 'r') as f:
        pass
except FileNotFoundError:
    print(f"File {img_path} not found.")
    sys.exit(1)

print("Num GPUs Available: ", len(tf.config.list_physical_devices('GPU')))
tf.debugging.set_log_device_placement(True)
gpus = tf.config.list_physical_devices('GPU')
if gpus:
    # Restrict TensorFlow to only use the first GPU
    try:
        tf.config.set_visible_devices(gpus[0], 'GPU')
        logical_gpus = tf.config.list_logical_devices('GPU')
        print(len(gpus), "Physical GPUs,", len(logical_gpus), "Logical GPU")
    except RuntimeError as e:
        # Visible devices must be set before GPUs have been initialized
        print(e)

# @param ["https://tfhub.dev/google/openimages_v4/ssd/mobilenet_v2/1", "https://tfhub.dev/google/faster_rcnn/openimages_v4/inception_resnet_v2/1"]
module_handle = "https://tfhub.dev/google/faster_rcnn/openimages_v4/inception_resnet_v2/1"

detector = hub.load(module_handle).signatures['default']

img = load_img(img_path)
result = run_detector(detector, img)
result = filter_results(result)
print(result)
