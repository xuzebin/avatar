#!/usr/bin/env python
import os, sys
import cv2
import numpy as np
import argparse
from calibration import StereoCamera, StereoPair
import rectification

if __name__ == "__main__":
    """
    Undistort and rectify a stereo image pair given the calibration parameters.
    Horizontal lines will be drawn on the undistorted & rectified pair to verify the result.

    Example usage:
    python show_rectified.py calib_params.yml stereo_images/0L.jpg stereo_images/0R.jpg
    """

    parser = argparse.ArgumentParser(description='Show undistorted & rectified stereo image pair. The input left img1 and right img2 is the original distorted and unrectified images. This program undistort and rectify img1 and img2 using calibration parameters read from a yaml file.')
    parser.add_argument('--calibration_file', default='calib_params2.yml', help='the yaml file containing the calibration pararmeters (.yml)')
    parser.add_argument('--img1', default='stereo_images/0L.jpg', help='left image')
    parser.add_argument('--img2', default='stereo_images/0R.jpg', help='right image')
    parser.add_argument('--scale', type=float, nargs=2, help='scaling factor on x and y')
    args = parser.parse_args()


    cam = StereoCamera(args.calibration_file)

    img1 = cv2.imread(args.img1, 0)
    img2 = cv2.imread(args.img2, 0)

    if args.scale is not None:
        (fx, fy) = args.scale
        img1 = cv2.resize(img1, (0, 0), fx=fx, fy=fy)
        img2 = cv2.resize(img2, (0, 0), fx=fx, fy=fy)
        
    (img1, img2) = rectification.undistort_rectify(cam, img1, img2)

    img1 = rectification.draw_horizontal_lines(img1)
    img2 = rectification.draw_horizontal_lines(img2)

    cv2.imshow("horizontal lines on stereo pair", np.hstack((img1, img2)))
    cv2.waitKey(0)
