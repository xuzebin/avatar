#!/usr/bin/env python

import cv2
import numpy as np

left_cam_id = 2
right_cam_id = 1
width = 680
height = 480

def show_camera(cam_id, width, height):
    """
    Show one camera
    """
    cap = cv2.VideoCapture(cam_id)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
    
    while True:
        ret, img = cap.read()
        if ret:
            cv2.imshow('single camera', img)
             
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()
    

def show_cameras(left, right, width, height):

    """
    Show two cameras in a horizontal aligned window
    """
    
    cap_left = cv2.VideoCapture(left)
    cap_left.set(cv2.CAP_PROP_FRAME_WIDTH, width)
    cap_left.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
    
    cap_right = cv2.VideoCapture(right)
    cap_right.set(cv2.CAP_PROP_FRAME_WIDTH, width)
    cap_right.set(cv2.CAP_PROP_FRAME_HEIGHT, height)

    while True:
        ret1, img1 = cap_left.read()
        ret2, img2 = cap_right.read()
        # Wait until ret1 and ret2 are true since camera setup needs some time.
        if ret1 and ret2:
            cv2.imshow("stereo cameras", np.hstack((img1, img2)))
             
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap_left.release()
    cap_right.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
#    show_cameras(left_cam_id, right_cam_id, width, height)
    show_camera(0, width, height)
