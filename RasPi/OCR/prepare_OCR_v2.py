#! /usr/bin/env python
import sys
import cv2
import numpy as np
from matplotlib import pyplot as plt

# read input image
img = cv2.imread('output.jpg')

# convert to gray scale
gray_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
cv2.imwrite('output_gray.jpg', gray_img)

#crop bottom half of image; where we assume the text for OCR exists
crop_img = gray_img[240:480, 50:590]
cv2.imwrite('crop_img.jpg', crop_img)

# apply adaptive thresholding on the cropped image
crop_img = cv2.medianBlur(crop_img,5)
thres_img = cv2.adaptiveThreshold(crop_img,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,\
            cv2.THRESH_BINARY,11,2)
cv2.imwrite('thres_img.jpg', thres_img)
exit(0)
