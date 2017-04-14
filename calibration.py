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

    def __str__(self):
        return "left: %s\n right: %s\n" % (self.left, self.right)

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
        self.undistort_map = StereoPair()
        self.rectify_map = StereoPair()
        self.rms = None
        self.Q = None
        self.r1 = None
        self.r2 = None
        self.p1 = None
        self.p2 = None
        
    def __str__(self):
        return '\n'.join('%s: %s' % item for item in vars(self).items())

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
    calib = Calibration()
    calib.img_size = img_size

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
    (calib.rms,
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
                                                  None,
                                                  None,
                                                  None,
                                                  None,
                                                  criteria=criteria, 
                                                  flags=flags)
    print 'rms: %s' % calib.rms
    if calib.rms < 1:
        print '[SUCCESS] calibration done'
    else:
        print '[ERROR] rms too high: %s' % calib.rms
    return calib

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


def stereo_rectify(calib):
    print 'perform rectification and undistortion...'

    (r1, r2, p1, p2, calib.Q, roi1, roi2) = cv2.stereoRectify(calib.camera_mat.left, 
                                                              calib.distortion_coeffs.left, 
                                                              calib.camera_mat.right, 
                                                              calib.distortion_coeffs.right,
                                                              calib.img_size,
                                                              calib.rotation_mat,
                                                              calib.translation_vec,
                                                              flags=0,
                                                              alpha=0
                                                              )

    calib.r1 = r1
    calib.r2 = r2
    calib.p1 = p1
    calib.p2 = p2


    (calib.undistort_map.left, calib.rectify_map.left) = cv2.initUndistortRectifyMap(calib.camera_mat.left, calib.distortion_coeffs.left,
                                                                        r1, p1, calib.img_size, cv2.CV_32FC1)

    (calib.undistort_map.right, calib.rectify_map.right) = cv2.initUndistortRectifyMap(calib.camera_mat.right, calib.distortion_coeffs.right,
                                                                        r2, p2, calib.img_size, cv2.CV_32FC1)

    print '[SUCCESS] rectification and undistortion done'

def rectify(calib, img_left, img_right):
    img_left = cv2.remap(img_left, calib.undistort_map.left, calib.rectify_map.left, cv2.INTER_LINEAR)
    img_right = cv2.remap(img_right, calib.undistort_map.right, calib.rectify_map.right, cv2.INTER_LINEAR)
    return (img_left, img_right)

def stereo_remap(input_dir, file_format, rows, cols, calib, img_num):
    for x in range(0, img_num):
        path_left = os.path.join(input_dir, file_format.left.format(idx=x))
        path_right = os.path.join(input_dir, file_format.right.format(idx=x))

        img_left_ = cv2.imread(path_left, 0)
        img_right_ = cv2.imread(path_right, 0)
        
        img_left = cv2.remap(img_left_, calib.undistort_map.left, calib.rectify_map.left, cv2.INTER_LINEAR)
        img_right = cv2.remap(img_right_, calib.undistort_map.right, calib.rectify_map.right, cv2.INTER_LINEAR)


        found_left, img_points_left = cv2.findChessboardCorners(img_left, (rows, cols))
        found_right, img_points_right = cv2.findChessboardCorners(img_right, (rows, cols))

        criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)

        if found_left and found_right:
            cv2.cornerSubPix(img_left, img_points_left, (11, 11), (-1, -1), criteria)
            cv2.cornerSubPix(img_right, img_points_right, (11, 11), (-1, -1), criteria)
        else:
            print '[ERROR] corner not found in the rectified %sth pair' % x
        
        if found_left and found_left:
            #show_depthmap(img_left, img_right)
            img_left = draw_horizontal_lines(img_left)
            img_right = draw_horizontal_lines(img_right)
            cv2.imshow("horizontal lines on stereo pair", np.hstack((img_left, img_right)))
            print 'horizontal lines on the %dth pair' % x
            cv2.waitKey(0)

            # Check quality
            lines1 = cv2.computeCorrespondEpilines(img_points_right.reshape(-1,2), 2, calib.fundamental_mat)
            lines1 = lines1.reshape(-1, 3)
            lines2 = cv2.computeCorrespondEpilines(img_points_left.reshape(-1,2), 1, calib.fundamental_mat)
            lines2 = lines2.reshape(-1, 3)
            error = 0.0
            for i in range(len(img_points_left)):
                # lines2[i][:] = [a, b, c] line parameters
                error += abs(img_points_left[i][0][0] * lines2[i][0] + img_points_left[i][0][1] * lines2[i][1] + lines2[i][2])
                error += abs(img_points_right[i][0][0] * lines1[i][0] + img_points_right[i][0][1] * lines1[i][1] + lines1[i][2])

            avg_error = error / (len(img_points_left) * 2)
#            print 'avg_error: %s' % avg_error

        else:
            print 'Not found in %dth pair' % x

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
    input_dir = 'checkerboards/'

    rows = 6
    cols = 8
    img_num = 25#32#48#28
    file_format = StereoPair("{idx:d}L.jpg", "{idx:d}R.jpg")
    img_points_left, img_points_right = capture_chessboard(input_dir, file_format, img_num, rows, cols)

    square_size = 27#28
    obj_points = calc_object_points(square_size, rows, cols)

    obj_points_all = [obj_points for _ in range(len(img_points_left))]

    img_size = (640, 480)# (weight, height) or (cols, row)
    calib = stereo_calibrate(obj_points_all, StereoPair(img_points_left, img_points_right), img_size)

    show_corners(input_dir, file_format, rows, cols, StereoPair(img_points_left, img_points_right))
    stereo_rectify(calib)
    stereo_remap(input_dir, file_format, rows, cols, calib, img_num)

    print calib
    calib.save('calib_result.pkl')

#     image_points = StereoPair(img_points_left, img_points_right)
#     pickle.dump(image_points, open("image_points.pkl", "wb"))
#     pickle.dump(obj_points_all, open("object_points.pkl", "wb"))

#    calib = pickle.load(open('calib_result.pkl', 'rb'))
#    stereo_remap(input_dir, rows, cols, calib, img_num)
