import os
import cv2

# set the current working directory
os.chdir('/home/pi/OCR/VR/')

# read input image
img = cv2.imread('tempImg.jpg')

# Extract only those image areas where VR information will exist
# Assumes that Rover positioning while imaging is perfect
# left_img = img[70:380, 80:320]
left_img = img[120:430, 80:320]
cv2.imwrite('left_img.jpg', left_img)
# right_img = img[70:380, 325:600]
right_img = img[120:430, 325:600]
cv2.imwrite('right_img.jpg', right_img)

exit(0)
