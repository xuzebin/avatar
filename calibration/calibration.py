#!/usr/bin/env python

import os
import cv2
import csv
import numpy as np
import yaml

class StereoPair():
    """
    A stereo pair of images/cameras
    """    
    def __init__(self, left = None, right = None):
        self.left = left
        self.right = right

    def __len__(self):
        return len(self.left)

    def __str__(self):
        return "left: %s\n right: %s\n" % (self.left, self.right)

class StereoCamera():
    """
    Object that hold and manage the calibrated stereo camera parameters
    """
    def __init__(self, file_name = None):
        if file_name is not None:
            self.read(file_name)
        else:
            self.camera_mat = StereoPair()
            self.distortion_coeffs = StereoPair()
            self.rotation_mat = None
            self.translation_vec = None
            self.essential_mat = None
            self.fundamental_mat = None
            self.rms = None
            self.img_size = None
        
    def __str__(self):
        return '\n'.join('%s: %s' % item for item in vars(self).items())

    def write(self, file_name):
        """ Write the attributes (stereo params) to a yaml file """
        stereo_params = {
             "left_camera_matrix": self.camera_mat.left.tolist(),
             "right_camera_matrix": self.camera_mat.right.tolist(),
             "left_distortion_coefficients": self.distortion_coeffs.left.tolist(),
             "right_distortion_coefficients": self.distortion_coeffs.right.tolist(),
             "rotation_matrix": self.rotation_mat.tolist(),
             "translation_vector": self.translation_vec.tolist(),
             "essential_matrix": self.essential_mat.tolist(),
             "fundamental_matrix": self.fundamental_mat.tolist(),
             "rms": self.rms,
             "image_size": self.img_size
            }
        with open(file_name, 'w') as f:
            yaml.dump(stereo_params, f)
            
        print 'calibration parameters saved into %s' % file_name

    def read(self, file_name):
        """ Read the attributes (stereo params) from a yaml file """
        with open(file_name, 'r') as f:
            stereo_params = yaml.load(f)
            
            self.camera_mat = StereoPair(np.array(stereo_params["left_camera_matrix"]), np.array(stereo_params["right_camera_matrix"]))
            self.distortion_coeffs = StereoPair(np.array(stereo_params["left_distortion_coefficients"]), np.array(stereo_params["right_distortion_coefficients"]))
            self.rotation_mat = np.array(stereo_params["rotation_matrix"])
            self.translation_vec = np.array(stereo_params["translation_vector"])
            self.essential_mat = np.array(stereo_params["essential_matrix"])
            self.fundamental_mat = np.array(stereo_params["fundamental_matrix"])
            self.rms = stereo_params["rms"]
            self.img_size = stereo_params["image_size"]


def stereo_calibrate(obj_points, img_points, img_size):
    """
    Calibrate a stereo camera given corresponding image points and object points
    Args:
        obj_points: list of object points for each image pair
        img_points: a StereoPair with list of image points from left and right camera
        img_size: size of the left/right image

    Return:
        StereoCamera instance with calibrated results
    """
    cam = StereoCamera()
    cam.img_size = img_size

    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_COUNT, 1000, 1e-5)
#    flags = 0
#     flags= (cv2.CALIB_FIX_ASPECT_RATIO + cv2.CALIB_ZERO_TANGENT_DIST + cv2.CALIB_RATIONAL_MODEL + cv2.CALIB_FIX_K3 + cv2.CALIB_FIX_K4 + cv2.CALIB_FIX_K5 + cv2.CALIB_FIX_K6)

#     # calibrate left camera
#     rms1, calib.camera_mat.left, calib.distortion_coeffs.left, rvecs1, tvecs1 = cv2.calibrateCamera(obj_points, img_points.left, img_size, criteria, flags)
#     # calibrate right camera
#     rms2, calib.camera_mat.right, calib.distortion_coeffs.right, rvecs2, tvecs2 = cv2.calibrateCamera(obj_points, img_points.right, img_size, criteria, flags)

#     print 'left camera calibration:'
#     print 'rms1" %s' % rms1
#     print 'mtx1" %s' % calib.camera_mat.left
#     print 'dist1" %s' % calib.distortion_coeffs.left

#     print 'right camera calibration:'
#     print 'rms2" %s' % rms2
#     print 'mtx2" %s' % calib.camera_mat.right
#     print 'dist2" %s' % calib.distortion_coeffs.right


    flags= (cv2.CALIB_FIX_ASPECT_RATIO + cv2.CALIB_ZERO_TANGENT_DIST + cv2.CALIB_RATIONAL_MODEL + cv2.CALIB_FIX_K3 + cv2.CALIB_FIX_K4 + cv2.CALIB_FIX_K5 + cv2.CALIB_FIX_K6)
    print 'calibrating...'
    (cam.rms,
     cam.camera_mat.left, cam.distortion_coeffs.left,
     cam.camera_mat.right, cam.distortion_coeffs.right,
     cam.rotation_mat, 
     cam.translation_vec, 
     cam.essential_mat, 
     cam.fundamental_mat) = cv2.stereoCalibrate(obj_points,
                                                img_points.left, img_points.right,
                                                cam.camera_mat.left, cam.distortion_coeffs.left,
                                                cam.camera_mat.right, cam.distortion_coeffs.right,
                                                img_size,
                                                None,
                                                None,
                                                None,
                                                None,
                                                criteria=criteria, 
                                                flags=flags)
    print 'rms: %s' % cam.rms
    if cam.rms < 1:
        print '[SUCCESS] calibration done'
    else:
        print '[ERROR] rms too high: %s' % cam.rms
    return cam

def capture_chessboard(input_dir, file_format, stereo_pair_num, rows, cols):
    """
    Detect chessboard pattern and find corners from stereo image pairs

    Args:
        input_dir: the directory containing the stereo image pairs with the chessboard pattern, 
                the image must be named as XL.jpg, XR.jpg where X is the index.
        stereo_pair_num: number of stereo pairs to be captured under the directory
        rows: number of rows of inner corners per chessboard pattern
        cols: number of colr of inner corners per chessboard pattern

    Returns:
        A tuple containing (img_points_left, img_points_right, img_size), where
        img_points_left: image coordinates of the corners in the left chessboard
        img_points_right: image coordinates of the corners in the right chessboard
    """
    img_points_left = []
    img_points_right = []
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.01)
    flags = (cv2.CALIB_CB_FILTER_QUADS + cv2.CALIB_CB_ADAPTIVE_THRESH + cv2.CALIB_CB_NORMALIZE_IMAGE + cv2.CALIB_CB_FAST_CHECK)
#    flags = cv2.CALIB_CB_FAST_CHECK
    print 'start capturing chessboard from images in %s' % input_dir
    for x in range(0, stereo_pair_num):
        path_left = os.path.join(input_dir, file_format.left.format(idx=x))
        path_right = os.path.join(input_dir, file_format.right.format(idx=x))

        img_left = cv2.imread(path_left, 0)
        img_right = cv2.imread(path_right, 0)

        found_left, corners_left = cv2.findChessboardCorners(img_left, (rows, cols), flags)
        found_right, corners_right = cv2.findChessboardCorners(img_right, (rows, cols), flags)

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

def show_corners(input_dir, file_format, rows, cols, img_points):
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
        path_left = os.path.join(input_dir, file_format.left.format(idx=x))
        path_right = os.path.join(input_dir, file_format.right.format(idx=x))

        img_left = cv2.imread(path_left, 0)
        img_right = cv2.imread(path_right, 0)
        img_left = cv2.cvtColor(img_left, cv2.COLOR_GRAY2BGR)
        img_right = cv2.cvtColor(img_right, cv2.COLOR_GRAY2BGR)

        cv2.drawChessboardCorners(img_left, (rows, cols), img_points.left[x], True)
        cv2.drawChessboardCorners(img_right, (rows, cols), img_points.right[x], True)

        cv2.imshow("chessboard corners".format(x), np.hstack((img_left, img_right)))
        print 'chessboard corners on the %dth pair' % x

        cv2.waitKey(0)

def draw_epipolarlines(img_left, img_points_left, img_points_right, fundamental_mat, flag):
    lines = cv2.computeCorrespondEpilines(img_points_right.reshape(-1, 2), flag, fundamental_mat)
    lines = lines.reshape(-1, 3)

    row, col = img_left.shape
    ep_img = cv2.cvtColor(img_left, cv2.COLOR_GRAY2BGR)
    counter = 0
    interval = 10
    for param, point_left in zip(lines, img_points_left):
        counter += 1
        if counter % interval != 0:
            continue
        color = tuple(np.random.randint(0, 255, 3).tolist())
        (x0, y0) = map(int, [0, -param[2] / param[1]])
        (x1, y1) = map(int, [col, -(param[2] + param[0] * col) / param[1]])
        
        ep_img = cv2.line(ep_img, (x0, y0), (x1, y1), color, 1)
        ep_img = cv2.circle(ep_img, tuple((point_left[:, 0], point_left[:, 1])), 4, color, 1)

    return ep_img


if __name__ == "__main__":
    input_dir = 'checkerboards2/'

    rows = 6
    cols = 8
    img_num = 30#25#32#48#28
    file_format = StereoPair("{idx:d}L.jpg", "{idx:d}R.jpg")
    img_points_left, img_points_right = capture_chessboard(input_dir, file_format, img_num, rows, cols)

    square_size = 27#28
    obj_points = calc_object_points(square_size, rows, cols)

    obj_points_all = [obj_points for _ in range(len(img_points_left))]

    img_size = (640, 480)# (weight, height) or (cols, row)
    cam = stereo_calibrate(obj_points_all, StereoPair(img_points_left, img_points_right), img_size)

    show_corners(input_dir, file_format, rows, cols, StereoPair(img_points_left, img_points_right))


    print cam
    cam.write('calib_params2.yml')

