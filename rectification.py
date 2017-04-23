#!/usr/bin/env python

import os
import cv2
import numpy as np
from calibration import StereoCamera, StereoPair

def get_rectify_map(stereo_camera, img_size):
    """
    Get the undistortion and rectification map given calibrated parameters and image's size

    Args:
        stereo_camera: the StereoCamera object with calibrated parameters
        img_size: a tuple of image's size (width, height)

    Returns:
        a tuple of maps: (left_map1, left_map2, right_map1, right_map2)
    """
    (r1, r2, p1, p2, Q, roi1, roi2) = cv2.stereoRectify(stereo_camera.camera_mat.left, 
                                                              stereo_camera.distortion_coeffs.left, 
                                                              stereo_camera.camera_mat.right, 
                                                              stereo_camera.distortion_coeffs.right,
                                                              img_size,
                                                              stereo_camera.rotation_mat,
                                                              stereo_camera.translation_vec,
                                                              flags=0,
                                                              alpha=0
                                                              )

    (left_map1, left_map2) = cv2.initUndistortRectifyMap(stereo_camera.camera_mat.left, stereo_camera.distortion_coeffs.left,
                                                                        r1, p1, img_size, cv2.CV_32FC1)

    (right_map1, right_map2) = cv2.initUndistortRectifyMap(stereo_camera.camera_mat.right, stereo_camera.distortion_coeffs.right,
                                                                        r2, p2, img_size, cv2.CV_32FC1)

    return (left_map1, left_map2, right_map1, right_map2)

def undistort_rectify(stereo_camera, img1, img2):
    """
    Undistort and rectify a stereo image pair to make epipolar lines horizontal.
    
    Args:
        stereo_camera: the StereoCamera object with calibrated parameters
        img1: left image
        img2: right image

    Returns:
        a tuple of the undistored and rectified stereo images
    """
    if img1.shape != img2.shape:
        raise ValueError('left and right images size not matched')
    height, width = img1.shape
    img_size = (width, height)

    (left_map1, left_map2, right_map1, right_map2) = get_rectify_map(stereo_camera, img_size)
    img1_ = cv2.remap(img1, left_map1, left_map2, cv2.INTER_LINEAR)
    img2_ = cv2.remap(img2, right_map1, right_map2, cv2.INTER_LINEAR)
    return (img1_, img2_)

def test_rectification(stereo_camera, input_dir, file_format, img_num):
    """ Undistort and rectify a group of stereo image pairs in a directory and display them with horizontal lines """
    for x in range(0, img_num):
        path_left = os.path.join(input_dir, file_format.left.format(idx=x))
        path_right = os.path.join(input_dir, file_format.right.format(idx=x))

        img_left = cv2.imread(path_left, 0)
        img_right = cv2.imread(path_right, 0)

        (img_left, img_right) = undistort_rectify(stereo_camera, img_left, img_right)

        img_left = draw_horizontal_lines(img_left)
        img_right = draw_horizontal_lines(img_right)
        print 'horizontal lines on the %dth pair' % x
        cv2.imshow("horizontal lines on stereo pair", np.hstack((img_left, img_right)))
        cv2.waitKey(0)

def draw_horizontal_lines(img):
    """
    Draw horizontal lines on img

    Args:
        img: the image to draw on

    Return
        the image with lines drawn
    """
    row, col = img.shape
    img = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
    interval = row / 10
    for i in range(1, 10):
        (x0, y0) = map(int, [0, i * interval])
        (x1, y1) = map(int, [col, i * interval])
        img = cv2.line(img, (x0, y0), (x1, y1), (0, 255, 0), 1)

    return img

if __name__ == "__main__":
    cam = StereoCamera('calib_params.yml')
    input_dir = 'checkerboards/'
    file_format = StereoPair("{idx:d}L.jpg", "{idx:d}R.jpg")
    img_num = 25
    test_rectification(cam, input_dir, file_format, img_num)
    
