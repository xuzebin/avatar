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
        print 'LEFT:'
        print 'cameraMatrix1:'
        print self.camera_mat.left
        print 'distortion coefficients1:'
        print self.distortion_coeffs.left
        print 'RIGHT:'
        print 'cameraMatrix2:'
        print self.camera_mat.right
        print 'distortion coefficients2:'
        print self.distortion_coeffs.right
        print 'rotation matrix: %s' % self.rotation_mat
        print 'translation matrix: %s' % self.translation_vec
        print 'essential matrix: %s' % self.essential_mat
        print 'fundamental matrix: %s' % self.fundamental_mat
    

def stereo_calibrate(obj_points, img_points, img_size):
    """
    Calibrate a stereo camera given corresponding image points and object points
    """
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
#    flags = (cv2.CALIB_FIX_ASPECT_RATIO + cv2.CALIB_ZERO_TANGENT_DIST + cv2.CALIB_FIX_FOCAL_LENGTH)
    flags = (cv2.CALIB_FIX_ASPECT_RATIO + cv2.CALIB_ZERO_TANGENT_DIST)
#    flags = cv2.CALIB_ZERO_TANGENT_DIST
    calib = Calibration()
    print 'Calibrating...'
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
        calib.print_params()
    else:
        print 'Calibration failed'
    
def capture_chessboard(input_dir, stereo_pair_num, rows, cols):
    """
    Detect chessboard pattern and find corners from stereo image pairs

    Args:
        inputDir: the directory containing the stereo image pairs with the chessboard pattern
        stereoPairNum: number of stereo pairs to be captured under the directory
        rows: number of rows of inner corners per chessboard pattern
        cols: number of colr of inner corners per chessboard pattern
        refineCorners: True if we refine the corner position using cornerSubPix()

    Returns:
        A tuple containing (img_points_left, img_points_right), where
        img_points_left: image coordinates of the corners in the left chessboard
        img_points_right: image coordinates of the corners in the right chessboard
    """
    imgPointsL = []
    imgPointsR = []
    objPointsPerPattern = calc_object_points(28, rows, cols)
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
    print 'Start capturing chessboard...'
    for x in range(0, stereo_pair_num):
        pathL = os.path.join(input_dir, "{0}L.jpg".format(x))
        pathR = os.path.join(input_dir, "{0}R.jpg".format(x))

        imgL = cv2.imread(pathL, 0)
        imgR = cv2.imread(pathR, 0)

        foundL, cornersL = cv2.findChessboardCorners(imgL, (rows, cols))
        foundR, cornersR = cv2.findChessboardCorners(imgR, (rows, cols))

        if foundL and foundR:
            cv2.cornerSubPix(imgL, cornersL, (11, 11), (-1, -1), criteria)
            cv2.cornerSubPix(imgR, cornersR, (11, 11), (-1, -1), criteria)

            # Draw corners
#             cv2.drawChessboardCorners(imgL, (rows, cols), cornersL, foundL)
#             cv2.drawChessboardCorners(imgR, (rows, cols), cornersR, foundR)
#             cv2.imshow("left", imgL)
#             cv2.imshow("right", imgR)
#            print 'Detected pattern in %s and %s' % (pathL, pathR)

            # Add to the points list
            imgPointsL.append(cornersL.reshape(-1, 2))
            imgPointsR.append(cornersR.reshape(-1, 2))

        else:
            print '[ERROR] Chessboard pattern not found in the %dth pair' % x

    print '[SUCCESS] Chessboard capture done!'
    cv2.destroyAllWindows()

    return (imgPointsL, imgPointsR)

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
