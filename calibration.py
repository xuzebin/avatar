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
        self.undistort_map = StereoPair()
        self.rectify_map = StereoPair()
        
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
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_COUNT, 100, 1e-5)
#    flags = (cv2.CALIB_FIX_ASPECT_RATIO + cv2.CALIB_USE_INTRINSIC_GUESS + cv2.CALIB_ZERO_TANGENT_DIST + cv2.CALIB_SAME_FOCAL_LENGTH)
    flags = (cv2.CALIB_FIX_ASPECT_RATIO + cv2.CALIB_ZERO_TANGENT_DIST)
#    flags = cv2.CALIB_ZERO_TANGENT_DIST
    calib = Calibration()
    calib.img_size = img_size


    # Initial camera matrix
#    calib.camera_mat.left = cv2.initCameraMatrix2D(obj_points, img_points.left, img_size, 0)
#    calib.camera_mat.right = cv2.initCameraMatrix2D(obj_points, img_points.right, img_size, 0)

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
        A tuple containing (img_points_left, img_points_right, img_size), where
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
        img_left = cv2.cvtColor(img_left, cv2.COLOR_GRAY2BGR)
        img_right = cv2.cvtColor(img_right, cv2.COLOR_GRAY2BGR)

        cv2.drawChessboardCorners(img_left, (rows, cols), img_points.left[x], True)
        cv2.drawChessboardCorners(img_right, (rows, cols), img_points.right[x], True)

        cv2.imshow("chessboard corners on the {0}th pair".format(x), np.hstack((img_left, img_right)))        

        cv2.waitKey(0)

        

def draw_epipolarlines(img_left, img_right, img_points, fundamental_mat):
    lines = cv2.computeCorrespondEpilines(img_right.reshape(-1, 2), 2, fundamental_mat)
    lines = lines.reshape(-1, 3)

    row, col = img_left.shape
    ep_img = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
    for param, point in zip(lines, img_points):
        color = tuple(np.random.randint(0, 255, 3).tolist())
        (x0, y0) = map(int, [0, -param[2] / param[1]])
        (x1, y1) = map(int, [col, -(param[2] + parm[0] * col) / param[1]])
        
        ep_img = cv2.line(ep_img, (x0, y0), (x1, y1), color, 1)
        ep_img = cv2.circle(ep_img, tuple((point[:, 0], point[:, 1])), 4, color, 1)
        
    cv2.imshow('epipolar lines', ep_img)
    cv2.waitkey(0)

def stereo_rectify(calib):
    print 'perform rectification and undistortion...'
    (r1, r2, p1, p2, disparity_to_depth_mat, roi1, roi2) = cv2.stereoRectify(calib.camera_mat.left, 
                                     calib.distortion_coeffs.left, 
                                     calib.camera_mat.right, 
                                     calib.distortion_coeffs.right,
                                     calib.img_size,
                                     calib.rotation_mat,
                                     calib.translation_vec,
                                     flags=0)

    (calib.undistort_map.left, calib.rectify_map.left) = cv2.initUndistortRectifyMap(calib.camera_mat.left, calib.distortion_coeffs.left,
                                                                        r1, p1, calib.img_size, cv2.CV_32FC1)

    (calib.undistort_map.right, calib.rectify_map.right) = cv2.initUndistortRectifyMap(calib.camera_mat.right, calib.distortion_coeffs.right,
                                                                        r2, p2, calib.img_size, cv2.CV_32FC1)
    print '[SUCCESS] rectification and undistortion done'

def stereo_remap(input_dir, rows, cols, calib, img_num):
    for x in range(0, img_num):
        path_left = os.path.join(input_dir, "{0}L.jpg".format(x))
        path_right = os.path.join(input_dir, "{0}R.jpg".format(x))

        img_left = cv2.imread(path_left, 0)
        img_right = cv2.imread(path_right, 0)

        img_left = cv2.remap(img_left, calib.undistort_map.left, calib.rectify_map.left, cv2.INTER_LINEAR)
        img_right = cv2.remap(img_right, calib.undistort_map.right, calib.rectify_map.right, cv2.INTER_LINEAR)

        found_left, img_points_left = cv2.findChessboardCorners(img_left, (rows, cols))
        found_right, img_points_right = cv2.findChessboardCorners(img_right, (rows, cols))

        criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
        cv2.cornerSubPix(img_left, img_points_left, (11, 11), (-1, -1), criteria)
        cv2.cornerSubPix(img_right, img_points_right, (11, 11), (-1, -1), criteria)
        
        if found_left and found_left:
            img_left = draw_horizontal_lines(img_left, img_points_left)
            img_right = draw_horizontal_lines(img_right, img_points_right)
            cv2.imshow("horizontal lines on the {0}th pair".format(x), np.hstack((img_left, img_right)))
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

def draw_horizontal_lines(img, img_points, draw_point=True):
    """
    Draw horizontal lines that pass img_points on img

    Args:
        img: the image to draw on
        img_points: the image points that the horizontal lines pass through
        draw_point: true if we want to circle the image points

    Return
        the image with lines (and points) drawn
    """
    row, col = img.shape
    img = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
    for point in img_points:
        color = tuple(np.random.randint(0, 255, 3).tolist())
        (x0, y0) = map(int, [0, point[:,1]])
        (x1, y1) = map(int, [col, point[:,1]])
        img = cv2.line(img, (x0, y0), (x1, y1), color, 1)
        if draw_point:
            img = cv2.circle(img, tuple((point[:, 0], point[:, 1])), 4, color, 1)
    return img


if __name__ == "__main__":
    input_dir = './images/'
    rows = 6
    cols = 8
    img_num = 50
    img_points_left, img_points_right = capture_chessboard(input_dir, img_num, rows, cols)

    square_size = 28
    obj_points = calc_object_points(square_size, rows, cols)
    obj_points_all = [obj_points for _ in range(len(img_points_left))]


    img_size = (640, 480)# (weight, height) or (cols, row)
    calib = stereo_calibrate(obj_points_all, StereoPair(img_points_left, img_points_right), img_size)

#    show_corners(input_dir, 6, 8, StereoPair(img_points_left, img_points_right))
    stereo_rectify(calib)
    stereo_remap(input_dir, rows, cols, calib, img_num)

    print calib
    calib.save('calib_result.pkl')

    image_points = StereoPair(img_points_left, img_points_right)
    pickle.dump(image_points, open("image_points.pkl", "wb"))
    pickle.dump(obj_points_all, open("object_points.pkl", "wb"))
    

