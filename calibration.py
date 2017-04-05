#!/usr/bin/env python

import os
import cv2
import csv
import numpy as np

class StereoPair():
    """
    A stereo pair of images/cameras
    """    
    def __init__(self, left = None, right = None):
        self.left = left
        self.right = right

class Calibration():
    """
    Stereo calibration results
    """
    def __init__(self):
        self.camera_mat = StereoPair()
        self.distortion_coeffs = StereoPair()
        self.rotation_mat = None
        self.translation_vec = None
        self.essential_mat = None
        self.fundamental_mat = None

    def print_params(self):
        print 'left camera matrix:\n%s' % self.camera_mat.left
        print 'left distortion coefficients:' % self.distortion_coeffs.left
        print 'right camera matrix:\n%s' % self.camera_mat.right
        print 'right distortion coefficients:' % self.distortion_coeffs.right
        print 'rotation matrix: \n%s' % self.rotation_mat
        print 'translation matrix: \n%s' % self.translation_vec
        print 'essential matrix: \n%s' % self.essential_mat
        print 'fundamental matrix: \n%s' % self.fundamental_mat
    

def stereo_calibrate(obj_points, img_points, img_size):
    """
    Calibrate a stereo camera given corresponding image points and object points
    """
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
#    flags = (cv2.CALIB_FIX_ASPECT_RATIO + cv2.CALIB_ZERO_TANGENT_DIST + cv2.CALIB_FIX_FOCAL_LENGTH)
    flags = (cv2.CALIB_FIX_ASPECT_RATIO + cv2.CALIB_ZERO_TANGENT_DIST)
#    flags = cv2.CALIB_ZERO_TANGENT_DIST
    calib = Calibration()
    print 'calibrating...'
    (retval,
     calib.camera_mat.left, calib.distortion_coeffs.left,
     calib.camera_mat.right, calib.distortion_coeffs.right,
     calib.rotation_mat, 
     calib.translation_vec, 
     calib.essential_mat, 
     calib.fundamental_mat) = cv2.stereoCalibrate(obj_points,
                                                  img_points.left, img_points.right,
                                                  calib.camera_mat.left, calib.distortion_coeffs.left,
                                                  calib.camera_mat.right, calib.distortion_coeffs.right,
                                                  img_size,
                                                  calib.rotation_mat,
                                                  calib.translation_vec,
                                                  calib.essential_mat,
                                                  calib.fundamental_mat,
                                                  criteria=criteria, 
                                                  flags=flags)
    if retval:
        print '[SUCCESS] calibration done'
        calib.print_params()
    else:
        print '[ERROR] calibration failed'
    
def capture_chessboard(input_dir, stereo_pair_num, rows, cols):
    """
    Detect chessboard pattern and find corners from stereo image pairs

    Args:
        input_dir: the directory containing the stereo image pairs with the chessboard pattern
        stereo_pair_num: number of stereo pairs to be captured under the directory
        rows: number of rows of inner corners per chessboard pattern
        cols: number of colr of inner corners per chessboard pattern

    Returns:
        A tuple containing (img_points_left, img_points_right), where
        img_points_left: image coordinates of the corners in the left chessboard
        img_points_right: image coordinates of the corners in the right chessboard
    """
    img_points_left = []
    img_points_right = []
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
    print 'start capturing chessboard...'
    for x in range(0, stereo_pair_num):
        path_left = os.path.join(input_dir, "{0}L.jpg".format(x))
        path_right = os.path.join(input_dir, "{0}R.jpg".format(x))

        img_left = cv2.imread(path_left, 0)
        img_right = cv2.imread(path_right, 0)

        found_left, corners_left = cv2.findChessboardCorners(img_left, (rows, cols))
        found_right, corners_right = cv2.findChessboardCorners(img_right, (rows, cols))

        if found_left and found_right:
            cv2.cornerSubPix(img_left, corners_left, (11, 11), (-1, -1), criteria)
            cv2.cornerSubPix(img_right, corners_right, (11, 11), (-1, -1), criteria)

            # Draw corners
#             cv2.drawChessboardCorners(imgL, (rows, cols), cornersL, foundL)
#             cv2.drawChessboardCorners(imgR, (rows, cols), cornersR, foundR)
#             cv2.imshow("left", imgL)
#             cv2.imshow("right", imgR)
#            print 'Detected pattern in %s and %s' % (pathL, pathR)

            # Add to the points list
            img_points_left.append(corners_left.reshape(-1, 2))
            img_points_right.append(corners_right.reshape(-1, 2))

        else:
            print '[ERROR] chessboard pattern not found in the %dth pair' % x

    print '[SUCCESS] chessboard capture done'
    cv2.destroyAllWindows()

    return (img_points_left, img_points_right)

def calc_object_points(square_size, rows, cols):
    """
    Calculate 3D corner points on chessboard based on the square size, colums, rows
    Args:
        square_size: size of a square in chessboard in mm
        rows: number of rows of inner corners per chessboard pattern
        cols: number of colr of inner corners per chessboard pattern
    Returns:
        World coordinates of the corners on a chessboard
    """
    obj_points = np.zeros((rows * cols, 3), np.float32)
    obj_points[:, :2] = np.mgrid[0:rows, 0:cols].T.reshape(-1, 2)
    obj_points *= square_size
    return obj_points

if __name__ == "__main__":
    img_points_left, img_points_right = capture_chessboard('./images/', 50, 6, 8)

    obj_points = calc_object_points(28, 6, 8)
    obj_points_all = [obj_points for _ in range(len(img_points_left))]

    stereo_calibrate(obj_points_all, StereoPair(img_points_left, img_points_right), (480, 320))
