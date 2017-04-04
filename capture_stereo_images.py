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
    while count < image_pair_num:
        ret1, img1 = cap_left.read()
        ret2, img2 = cap_right.read()
        # Wait until ret1 and ret2 are true since camera setup needs some time.
        if ret1 and ret2:
            cv2.imshow("stereo cameras", np.hstack((img1, img2)))
             
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

        # When space key is pressed, save the image of left and right respectively
        if cv2.waitKey(1) & 0xFF == ord(' '):
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
    output_dir = './images/'
    left_cam = 1
    right_cam = 2
    width = 480
    height = 320
    image_pair_num = 50

    print_prompt(output_dir)
    capture_stereo_images(left_cam, right_cam, width, height, image_pair_num, output_dir)
