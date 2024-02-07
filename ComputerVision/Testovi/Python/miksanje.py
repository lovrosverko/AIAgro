from imutils.video import VideoStream

import numpy as np

import imutils

import time

import cv2

print("[INFO] starting video stream...")

vs = VideoStream(src=0).start()

time.sleep(2.0)

frame2 = cv2.imread("cover2.jpg")

while True:

    frame1 = vs.read()

    frame1 = imutils.resize(frame1, width=800)

    frame = frame2.astype("int32") + frame1.astype("int32")

    frame = np.absolute(frame).astype("uint8")

    cv2.imshow("Frame", frame)

    key = cv2.waitKey(1) & 0xFF

    if key == ord("q"):

        break

cv2.destroyAllWindows()

vs.stop