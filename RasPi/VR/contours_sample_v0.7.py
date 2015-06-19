#!/usr/bin/env python

import numpy as np
import cv2

if __name__ == '__main__':
    print __doc__

    img = cv2.imread('vrImg.jpg')
    img_copy = img.copy()
    # prepare image; convert to gray scale and perform adaptive thresholding
    gray_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    gray_img = cv2.GaussianBlur(gray_img, (5,5), 0)
    thres_img = cv2.adaptiveThreshold(gray_img,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,\
            cv2.THRESH_BINARY,11,2)
    thres_img = cv2.bilateralFilter(thres_img, 11, 17, 17)
    thres_img = cv2.Canny(thres_img, 30, 200)
    cv2.imwrite('thres_img.jpg', thres_img)

    h, w = thres_img.shape[:2]

	# identify contours
    contours0, hierarchy = cv2.findContours( thres_img.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_TC89_L1)
    # approximate polygone on contours
    contours = [cv2.approxPolyDP(cnt, 3, True) for cnt in contours0]
    # reverse sort based on contour area
    contours = sorted(contours, key=cv2.contourArea, reverse=True)[2:]

    screenCnt = None
    for cnt in contours:
		area = cv2.contourArea(cnt)
		peri = cv2.arcLength(cnt, True)
		sides = len(cnt)
		s = "sides = " + str(sides) + "; area = " + str(area) + "; peri = " + str(peri)
		print s
		screenCnt = cnt
		if sides == 4:
			screenCnt = cnt
			break
    if screenCnt == None:
		print "not found"
		exit -1

    print "found"
    cv2.drawContours(img, screenCnt, -1, (0,255,0), 5)
    cv2.imwrite('contour_outline.jpg', img)
	# bind the identified contour with rectangle and crop the input image
    x,y,w,h = cv2.boundingRect(screenCnt)
    crop_img = img[y:y+h, x:x+w]
    cv2.imwrite('crop_img.jpg', crop_img)
    # prepare for removing image skew
    pts = screenCnt.reshape(4, 2)
    rect = np.zeros((4, 2), dtype = "float32")
    s = pts.sum(axis = 1)
    # identify four corners of contour
    rect[0] = pts[np.argmin(s)]
    rect[2] = pts[np.argmax(s)]
    diff = np.diff(pts, axis = 1)
    rect[1] = pts[np.argmin(diff)]
    rect[3] = pts[np.argmax(diff)]

    # identify size of target image - calculate maximum width and height
    (tl, tr, br, bl) = rect
    widthA = br[0]-bl[0]
    widthB = tr[0]-tl[0]
    heightA = bl[1]-tl[1]
    heightB = br[1]-tr[1]

    maxW = max(int(widthA), int(widthB))
    maxH = max(int(heightA), int(heightB))

    dst = np.array([[0,0],
		[maxW-1,0],
		[maxW-1,maxH-1],
		[0,maxH-1]], dtype="float32")

	# fix perspective warp
    M = cv2.getPerspectiveTransform(rect, dst)
    warp = cv2.warpPerspective(img, M, (maxW, maxH))
    cv2.imwrite('final_op.jpg', warp)

	# extract left and right images

    ymax = 0.95 * maxH
    ymin = 0.05 * maxH
    xmaxl = 0.95 * (maxW / 2)
    xminl = 0.011 * (maxW / 2)
    xmaxr = (maxW / 2) + xmaxl
    xminr = (maxW / 2) + xminl

    left_img = warp[ymin:ymax, xminl:xmaxl]
    right_img = warp[ymin:ymax, xminr:xmaxr]

    cv2.imwrite('left_p.jpg',left_img)
    cv2.imwrite('right_p.jpg', right_img)
