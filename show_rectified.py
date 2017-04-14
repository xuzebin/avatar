#!/usr/bin/env python
import os, sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import calibration
from calibration import Calibration, StereoPair
import pickle
import cv2
import numpy as np
import argparse

if __name__ == "__main__":
    """
    Undistort and rectify a stereo image pair given a calibration parameter file.
    Horizontal lines will be drawn on the rectified pair to verify the result.

    Example usage:
    python show_rectified.py calib_result.pkl stereo_images/0L.jpg stereo_images/0R.jpg
    """
    parser = argparse.ArgumentParser(description='Show undistorted & rectified stereo image pair. The input left img1 and right img2 is the original distorted and unrectified images. This program undistort and rectify img1 and img2 based on the given calibration parameter file.')
    parser.add_argument('calibration_file', help='the file containing the calibration pararmeters (.pkl)')
    parser.add_argument('img1', help='left image')
    parser.add_argument('img2', help='right image')
    args = parser.parse_args()

    calib = pickle.load(open(args.calibration_file, 'rb'))

    img1 = cv2.imread(args.img1, 0)
    img2 = cv2.imread(args.img2, 0)

    (img1, img2) = calibration.rectify(calib, img1, img2)

    img1 = calibration.draw_horizontal_lines(img1)
    img2 = calibration.draw_horizontal_lines(img2)
    cv2.imshow("horizontal lines on stereo pair", np.hstack((img1, img2)))
    cv2.waitKey(0)
