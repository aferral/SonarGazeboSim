import cv2
import numpy as np
import pylab as plt
from skimage.morphology import watershed, disk
from skimage import data
from skimage.filters import rank
from skimage.util import img_as_ubyte
from scipy import ndimage as ndi
from PIL import Image, ImageDraw
from quads import main

class simpleNode:
    def __init__(self,e,next=None):
        self.elem = e
        self.n = next

#From binary image get rectangular representation
def rectangularize(image,center):
    print "hi"
    th = 20

    binary = np.zeros(image.shape)
    binary[image > th] = 1

    #calculate the segments of the iamge
    markers = rank.gradient(binary, disk(5)) < 10
    markers = ndi.label(markers)[0]

    #Just keep the segment where is the origin (is the inside)
    insideLabel = markers[center[0],center[1]]
    componentInside = markers == insideLabel


    plt.imshow(componentInside, cmap=plt.cm.gray, interpolation='nearest')

    plt.show()

    #Now we have to find a rectangular partition of this figure

    #Now we use a quadtree decomposition
    #CODE FROM https://github.com/fogleman/Quads

    cv2.imwrite('out.jpg',componentInside.astype(np.uint8)*255)
    image = Image.open("out.jpg").convert('I')
    main(image)


    plt.imshow(binary)
    plt.show()


    pass






if __name__ == '__main__':
    image= cv2.imread("test.jpe",0)
    rectangularize(image,(50,100))
    pass

