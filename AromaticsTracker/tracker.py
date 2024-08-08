import time

import tensorflow as tf


def load_img(path):
    img = tf.io.read_file(path)
    img = tf.image.decode_jpeg(img, channels=3)
    return img


def run_detector(detector, img):
    print("Running detector for image:", img.shape)

    converted_img = tf.image.convert_image_dtype(img, tf.float32)[
        tf.newaxis, ...]
    start_time = time.time()
    result = detector(converted_img)
    end_time = time.time()

    result = {key: value.numpy() for key, value in result.items()}
    print("Found %d objects." % len(result["detection_scores"]))
    print("Inference time: ", end_time - start_time)

    return result


def filter_results(result, min_score=0.5):
    i = 0
    for score in result["detection_scores"]:
        if score < min_score:
            break
        i += 1
    return {key: value[:i] for key, value in result.items()}


def detect_img(detector, image_path: str):
    start_time = time.time()
    run_detector(detector, image_path)
    end_time = time.time()
    print("Inference time:", end_time-start_time, "seconds")
