#!/usr/bin/env python

import os
import cv2
import csv
import numpy as np
import pickle

class StereoPair():
    """
    A stereo pair of images/cameras
    """    
    def __init__(self, left = None, right = None):
        self.left = left
        self.right = right

    def __len__(self):
        return len(self.left)

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
        self.img_size = None

    def __str__(self):
        return 'left camera matrix:\n{0}\n left distortion coefficients:\n{1}\n right camera matrix:\n{2}\n right distortion coefficients:\n {3}\n rotatin matrix:\n{4}\n translation vector:\n{5}\n essential matrix:\n{6}\n fundamental matrix:\n{7}\n'.format(self.camera_mat.left, self.distortion_coeffs.left, self.camera_mat.right, self.distortion_coeffs.right, self.rotation_mat, self.translation_vec, self.essential_mat, self.fundamental_mat)

    def save(self, file_name):
        pickle.dump(self, open(file_name, "wb"))
        print 'calibration result saved into %s' % file_name

def stereo_calibrate(obj_points, img_points, img_size):
    """
    Calibrate a stereo camera given corresponding image points and object points
    Args:
        obj_points: list of object points for each image pair
        img_points: a StereoPair with list of image points from left and right camera
        img_size: size of the left/right image

    Return:
        Calibration instance with calibrated results or None if calibration failed
    """
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
#    flags = (cv2.CALIB_FIX_ASPECT_RATIO + cv2.CALIB_ZERO_TANGENT_DIST + cv2.CALIB_FIX_FOCAL_LENGTH)
    flags = (cv2.CALIB_FIX_ASPECT_RATIO + cv2.CALIB_ZERO_TANGENT_DIST)
#    flags = cv2.CALIB_ZERO_TANGENT_DIST
    calib = Calibration()
    calib.img_size = img_size
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
        return calib
    else:
        print '[ERROR] calibration failed'
        return None

def capture_chessboard(input_dir, stereo_pair_num, rows, cols):
    """
    Detect chessboard pattern and find corners from stereo image pairs

    Args:
        input_dir: the directory containing the stereo image pairs with the chessboard pattern, 
                the image must be named as XL.jpg, XR.jpg where X is the index.
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
    print 'start capturing chessboard from images in %s' % input_dir
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

def show_corners(input_dir, rows, cols, img_points):
    """
    Draw the corners/image points on chessboard
    Args:
        input_dir: the directory containing the stereo image pairs with the chessboard pattern, 
                the image must be named as XL.jpg, XR.jpg where X is the index.
        rows: number of rows of inner corners per chessboard pattern
        cols: number of colr of inner corners per chessboard pattern
        img_points: a StereoPair with list of image points from left and right camera
    """

    print 'press any key to show the next pair of images'
    for x in range(0, len(img_points)):
        path_left = os.path.join(input_dir, "{0}L.jpg".format(x))
        path_right = os.path.join(input_dir, "{0}R.jpg".format(x))

        img_left = cv2.imread(path_left, 0)
        img_right = cv2.imread(path_right, 0)

        cv2.drawChessboardCorners(img_left, (rows, cols), img_points.left[x], True)
        cv2.drawChessboardCorners(img_right, (rows, cols), img_points.right[x], True)

        cv2.imshow("chessboard corners", np.hstack((img_left, img_right)))        

        cv2.waitKey(0)


if __name__ == "__main__":
    input_dir = './images/'
    img_points_left, img_points_right = capture_chessboard(input_dir, 50, 6, 8)

    obj_points = calc_object_points(28, 6, 8)
    obj_points_all = [obj_points for _ in range(len(img_points_left))]

    calib = stereo_calibrate(obj_points_all, StereoPair(img_points_left, img_points_right), (480, 320))
    print calib
    calib.save('calib_result.pkl')

#    show_corners(input_dir, 6, 8, StereoPair(img_points_left, img_points_right))
    
