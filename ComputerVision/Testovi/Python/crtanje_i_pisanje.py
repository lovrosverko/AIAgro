import numpy as np

import cv2

img = np.zeros((512,512,3), np.uint8)

img = cv2.line(img,(0,0),(511,511),(255,0,0),5)

slika = ("Linija")

cv2.imshow(slika, img)

cv2.waitKey(0)

cv2.destroyWindow(slika)