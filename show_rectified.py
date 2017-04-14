#!/usr/bin/env python
import os, sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import calibration
from calibration import Calibration, StereoPair
import cv2
import numpy as np
import argparse

if __name__ == "__main__":
    """
    Undistort and rectify a stereo image pair given the calibration parameters.
    Horizontal lines will be drawn on the undistorted & rectified pair to verify the result.

    Example usage:
    python show_rectified.py calib_params.yml stereo_images/0L.jpg stereo_images/0R.jpg
    """

    parser = argparse.ArgumentParser(description='Show undistorted & rectified stereo image pair. The input left img1 and right img2 is the original distorted and unrectified images. This program undistort and rectify img1 and img2 using calibration parameters read from a yaml file.')
    parser.add_argument('calibration_file', help='the yaml file containing the calibration pararmeters (.yml)')
    parser.add_argument('img1', help='left image')
    parser.add_argument('img2', help='right image')
    args = parser.parse_args()

    calib = Calibration()

    calib.read(args.calibration_file)

    img1 = cv2.imread(args.img1, 0)
    img2 = cv2.imread(args.img2, 0)

    (img1, img2) = calibration.undistort_rectify(calib, img1, img2)

    img1 = calibration.draw_horizontal_lines(img1)
    img2 = calibration.draw_horizontal_lines(img2)
    cv2.imshow("horizontal lines on stereo pair", np.hstack((img1, img2)))
    cv2.waitKey(0)
