#! /usr/bin/env python
import sys
import cv2
import numpy as np
from matplotlib import pyplot as plt

# read input image
img = cv2.imread('/home/pi/OCR/VR/tempImg.jpg')

# Extract only those image areas where VR information will exist
# Assumes that Rover positioning while imaging is perfect
# left_img = img[70:380, 80:320]
left_img = img[120:430, 80:320];
cv2.imwrite('/home/pi/OCR/VR/left_img.jpg', left_img)
# right_img = img[70:380, 325:600]
right_img = img[120:430, 325:600];
cv2.imwrite('/home/pi/OCR/VR/right_img.jpg', right_img)

exit(0)
