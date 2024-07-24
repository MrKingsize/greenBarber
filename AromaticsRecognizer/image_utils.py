from matplotlib import pyplot
from matplotlib.image import imread


def plot_images(folder: str, num_images: int, prefix: str):
    """
    Plot a number of images from a folder with a given prefix
    :param folder: folder with images
    :param num_images: number of images to plot
    :param prefix: prefix of the image files
    """

    # plot first few images
    for i in range(num_images):
        # define subplot
        pyplot.subplot(330 + 1 + i)
        # define filename
        filename = folder + prefix + '_' + str(i+1) + '.png'
        # load image pixels
        image = imread(filename)
        # plot raw pixel data
        pyplot.imshow(image)
    # show the figure
    pyplot.show()
