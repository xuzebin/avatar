#!/usr/bin/env python

import cv2

width = 680
height = 480

def show_cameras(left, right):

    """
    Show two cameras in separate windows
    """

    cap_left = cv2.VideoCapture(left)
    cap_left.set(cv2.CAP_PROP_FRAME_WIDTH, width)
    cap_left.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
    
    cap_right = cv2.VideoCapture(right)
    cap_right.set(cv2.CAP_PROP_FRAME_WIDTH, width)
    cap_right.set(cv2.CAP_PROP_FRAME_HEIGHT, height)

    cv2.namedWindow("left")
    cv2.namedWindow("right")
    # Align left and right windows
    cv2.moveWindow("left", 50, 150);
    cv2.moveWindow("right", width + 50, 150);

    while True:
        ret1, img1 = cap_left.read()
        ret2, img2 = cap_right.read()
        # Wait until ret1/ret2 is true since camera setup needs some time.
        if ret1:
            cv2.imshow('left', img1)
        if ret2:
            cv2.imshow('right', img2)
             
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap_left.release()
    cap_right.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    show_cameras(2, 1)
