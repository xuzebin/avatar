#!/usr/bin/env python
import os
import cv2
import numpy as np

"""
Capture stereo images from two webcams and save.
It can be used for capturing chessboard patterns and for further camera calibration.
"""

def capture_stereo_images(left_cam, right_cam, width, height, image_pair_num, output_dir):
    """
    Capture stereo image pairs from two cameras and save.

    Args:
        left_cam: left camera id
        right_cam: right camera id
        width: width of the camera frame
        height: height of the camera frame
        image_pair_num: number of stereo image pairs to capture
        output_dir: output directory where the captures images are saved into
    """
    if left_cam < 0 or right_cam < 0 or width < 0 or height < 0 or image_pair_num < 1:
        print 'Invalid input'
        return

    # If the output folder not exist, create it
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    cap_left = cv2.VideoCapture(left_cam)
    cap_left.set(cv2.CAP_PROP_FRAME_WIDTH, width)
    cap_left.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
    
    cap_right = cv2.VideoCapture(right_cam)
    cap_right.set(cv2.CAP_PROP_FRAME_WIDTH, width)
    cap_right.set(cv2.CAP_PROP_FRAME_HEIGHT, height)

    count = 0
    flags = (cv2.CALIB_CB_FAST_CHECK)
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
    rows = 6
    cols = 8
    while count < image_pair_num:
        ret1, img1 = cap_left.read()
        ret2, img2 = cap_right.read()
        # Wait until ret1 and ret2 are true since camera setup needs some time.
        if ret1 and ret2:
            img1_gray = cv2.cvtColor(img1, cv2.COLOR_BGR2GRAY)
            img2_gray = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY)
            found_left, corners_left = cv2.findChessboardCorners(img1_gray, (rows, cols), flags)
            found_right, corners_right = cv2.findChessboardCorners(img2_gray, (rows, cols), flags)
            if found_left and found_right:
#                cv2.cornerSubPix(img1_gray, corners_left, (11, 11), (-1, -1), criteria)
#                cv2.cornerSubPix(img2_gray, corners_right, (11, 11), (-1, -1), criteria)

                img1_gray = cv2.cvtColor(img1_gray, cv2.COLOR_GRAY2BGR)
                img2_gray = cv2.cvtColor(img2_gray, cv2.COLOR_GRAY2BGR)
        
                cv2.drawChessboardCorners(img1_gray, (rows, cols), corners_left, found_left)
                cv2.drawChessboardCorners(img2_gray, (rows, cols), corners_right, found_right)


#            cv2.imshow("stereo cameras", np.hstack((img1, img2)))
            cv2.imshow("stereo cameras", np.hstack((img1_gray, img2_gray)))
             
        # When space key is pressed, save the image of left and right respectively
        if ret1 and ret2 and cv2.waitKey(1) & 0xFF == ord(' '):
            save_images(img1, img2, count)
            count = count + 1

    cap_left.release()
    cap_right.release()
    cv2.destroyAllWindows()

def save_images(left, right, count):
    left_filename = "{0}L.jpg".format(count)
    right_filename = "{0}R.jpg".format(count)

    output_path_left = os.path.join(output_dir, left_filename)
    output_path_right = os.path.join(output_dir, right_filename)

    cv2.imwrite(output_path_left, left)
    cv2.imwrite(output_path_right, right)

    print 'Images %s and %s saved' % (left_filename, right_filename)

def print_prompt(output_dir):
    print 'Press space key to capture images...'
    print 'Images will be saved in %s' % output_dir

if __name__ == "__main__":
    output_dir = './images_320x240/'
    left_cam = 1
    right_cam = 2
    width = 320
    height = 240
#     width = 800
#     height = 600
    image_pair_num = 50

    print_prompt(output_dir)
    capture_stereo_images(left_cam, right_cam, width, height, image_pair_num, output_dir)
