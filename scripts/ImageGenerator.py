import cv2
import numpy as np

image_w = 1280
image_h = 1024

blank_image = np.zeros((image_w, image_h,3), np.uint8)


for i in range(image_w):
    for j in range(image_h):

        if i%2 == 0:
            if j%2 == 0:
                blank_image[i, j] = (255, 255, 255)  # (B, G, R)
            else:
                blank_image[i, j] = (0, 0, 0)  # (B, G, R)
        else:
            if j%2 == 0:
                blank_image[i, j] = (0, 0, 0)  # (B, G, R)
            else:
                blank_image[i, j] = (255, 255, 255)  # (B, G, R)



cv2.imwrite('checkerboard.bmp',blank_image)