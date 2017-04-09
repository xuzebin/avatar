#!/usr/bin/env python

# Python 2/3 compatibility
from __future__ import print_function

import numpy as np
import cv2
import pickle
import calibration
from calibration import Calibration, StereoPair

#imgL = cv2.imread('stereo-pairs/tsukuba/imL.png',0)
#imgR = cv2.imread('stereo-pairs/tsukuba/imR.png',0)


#w, h = (imgL.shape[1]/8, imgL.shape[0]/8)
# imgL = cv2.resize(imgL,(w,h))
# imgR = cv2.resize(imgR,(w,h))

#stereo = cv2.StereoBM_create(numDisparities=32, blockSize=7)
# stereo = cv2.StereoSGBM_create(
#             0,
#             64,
#             7,
#             216,
#             864,
#             1,
#             63,
#             10,
#             100,
#             32,
#             True
#             )

class BlockMatcher():
    def __init__(self):
        self.min_disparity = 0
        self.num_disparities = 112 - self.min_disparity
        self.block_size = 3
        self.uniqueness_ratio = 10
        self.speckle_window_size = 100
        self.speckle_range = 32
        self.disp12_max_diff = 1
        self.p1 = 8 * 3 * self.block_size**2
        self.p2 = 32 * 3 * self.block_size**2
        self.prefilter_cap = 0

    def __str__(self):
        return '\n'.join('%s: %s' % item for item in vars(self).items())

    def save(self, file_name):
        pickle.dump(self, open(file_name, "wb"))
        print ('blockmathcer result saved into %s' % file_name)

#bm = pickle.load(open('blockMatcher.pkl', 'rb'))
bm = BlockMatcher()

stereo = cv2.StereoSGBM_create(minDisparity = bm.min_disparity,
                               numDisparities = bm.num_disparities,
                               blockSize = bm.block_size,
                               uniquenessRatio = bm.uniqueness_ratio,
                               speckleWindowSize = bm.speckle_window_size,
                               speckleRange = bm.speckle_range,
                               disp12MaxDiff = bm.disp12_max_diff,
                               P1 = bm.p1,
                               P2 = bm.p2,
                               preFilterCap = bm.prefilter_cap
                               )

def on_min_disparity(position):
    print ('on_minDisparity %s' % position)
    stereo.setMinDisparity(position)
    bm.min_disparity = position

def on_num_disparities(position):
    if position % 16 != 0:
        mult = position / 16
        position = int(mult * 16)

    if position == 0:
        position = 16
    
    # numDisparities must be a multiple of 16
    print ('on_numDisparities %s' % position)
    stereo.setNumDisparities(position)
    bm.num_disparities = position

def on_block_size(position):
    print ('on_blockSize %s' % position)
    stereo.setBlockSize(position)
    bm.block_size = position

def on_p1(position):
    position *= 10
    print ('on_p1 %s' % position)
    stereo.setP1(position)
    bm.p1 = position

def on_p2(position):
    position *= 10
    print ('on_p2 %s' % position)
    stereo.setP2(position)
    bm.p2 = position

def on_disp12_max_diff(position):
    print ('on_disp12_max_diff %s' % position)
    stereo.setDisp12MaxDiff(position)
    bm.disp12_max_diff = position

def on_uniqueness_ratio(position):
    print ('on_uniqueness_ratio %s' % position)
    stereo.setUniquenessRatio(position)
    bm.uniqueness_ratio = position

def on_speckle_window_size(position):
    print ('on_speckle_window_size %s' % position)
    stereo.setSpeckleWindowSize(position)
    bm.speckle_window_size = position

def on_speckle_range(position):
    print ('on_speckle_range %s' % position)
    stereo.setSpeckleRange(position)
    bm.speckle_range = position

def on_prefilter_cap(position):
    print ('on_prefilter_cap %s' % position)
    stereo.setPreFilterCap(position)
    bm.prefilter_cap = position

# window_size = 3
# min_disp = 16
# num_disp = 112-min_disp
# stereo = cv2.StereoSGBM_create(minDisparity = min_disp,
#                                numDisparities = num_disp,
#                                blockSize = 16,
#                                P1 = 8*3*window_size**2,
#                                P2 = 32*3*window_size**2,
#                                disp12MaxDiff = 1,
#                                uniquenessRatio = 10,
#                                speckleWindowSize = 100,
#                                speckleRange = 32
#                                )
cv2.namedWindow("disparity")
cv2.createTrackbar("minDisparity", "disparity", bm.min_disparity, 100, on_min_disparity)
cv2.createTrackbar("numDisparities", "disparity", bm.num_disparities, 150, on_num_disparities)
cv2.createTrackbar("blockSize", "disparity", bm.block_size, 100, on_block_size)
cv2.createTrackbar("P1", "disparity", bm.p1, 500, on_p1)
cv2.createTrackbar("P2", "disparity", bm.p2, 500, on_p2)
#cv2.createTrackbar("disp12MaxDiff", "disparity", bm.disp12_max_diff, 100, on_disp12_max_diff)
cv2.createTrackbar("uniquenessRatio", "disparity", bm.uniqueness_ratio, 100, on_uniqueness_ratio)
#cv2.createTrackbar("speckleWindowSize", "disparity", bm.speckle_window_size, 1000, on_speckle_window_size)
#cv2.createTrackbar("speckleRange", "disparity", bm.speckle_range, 1000, on_speckle_range)
cv2.createTrackbar("preFilterCap", "disparity", bm.prefilter_cap, 100, on_prefilter_cap)
# while True:
#     disparity = stereo.compute(imgL,imgR)
#     # Normalize the image for representation
#     min = disparity.min()
#     max = disparity.max()
#     disparity = np.uint8(255 * (disparity - min) / (max - min))

#     cv2.imshow("disparity", np.hstack((imgL, imgR, disparity)))
#     if cv2.waitKey(1) & 0xFF == ord('q'):
#         break

calib = pickle.load(open('calib_result.pkl', 'rb'))

def depth_map_from_cams(left, right, width, height):

    cap_left = cv2.VideoCapture(left)
    cap_left.set(cv2.CAP_PROP_FRAME_WIDTH, width)
    cap_left.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
    
    cap_right = cv2.VideoCapture(right)
    cap_right.set(cv2.CAP_PROP_FRAME_WIDTH, width)
    cap_right.set(cv2.CAP_PROP_FRAME_HEIGHT, height)

    while True:
        ret1, img1 = cap_left.read()
        ret2, img2 = cap_right.read()

        # Wait until ret1/ret2 is true since camera setup needs some time.
        if ret1 and ret2:
            img1 = cv2.cvtColor(img1, cv2.COLOR_BGR2GRAY)
            img2 = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY)
            
            # Rectify, undistort
            (img1, img2) = calibration.rectify(calib, img1, img2)
            disparity = stereo.compute(img1,img2)
            
            rows = 6
            cols = 8
            # found_left, corners_left = cv2.findChessboardCorners(img1, (rows, cols))
#             found_right, corners_right = cv2.findChessboardCorners(img2, (rows, cols))
#             if found_left and found_right:
#                 img1 = calibration.draw_horizontal_lines(img1, corners_left)
#                 img2 = calibration.draw_horizontal_lines(img2, corners_right)
#                 cv2.imshow("horizontal lines on the stereo pair".format(0), np.hstack((img1, img2)))
            
#                 min = disparity.min()
#                 max = disparity.max()
#                 disparity = np.uint8(255 * (disparity - min) / (max - min))
            disparity = np.uint8(disparity)

#            cv2.imshow("disparity", np.hstack((img1, img2, disparity)))
            cv2.imshow("disparity", disparity)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            print (bm)
            break

        # Capture an image and save
        if cv2.waitKey(1) == ord('s'):
            cv2.imwrite("left.jpg", img1)
            cv2.imwrite("right.jpg", img2)
            print ("image saved")


    cap_left.release()
    cap_right.release()
    cv2.destroyAllWindows()


def depth_map(img_left, img_right):
#     bm = pickle.load(open('blockMatcher.pkl', 'rb'))
#     stereo = cv2.StereoSGBM_create(minDisparity = bm.min_disparity,
#                                    numDisparities = bm.num_disparities,
#                                    blockSize = bm.block_size,
#                                    uniquenessRatio = bm.uniqueness_ratio,
#                                    speckleWindowSize = bm.speckle_window_size,
#                                    speckleRange = bm.speckle_range,
#                                    disp12MaxDiff = bm.disp12_max_diff,
#                                    P1 = bm.p1,
#                                    P2 = bm.p2
#                                    )
    while True:
        disparity = stereo.compute(img_left, img_right)
        disparity = np.uint8(disparity)
        cv2.imshow('disparity', disparity)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            bm.save('blockMatcher.pkl')
            break
            

#     print('generating 3d point cloud...',)
#     h, w = img_left.shape[:2]
#     f = 0.8*w                          # guess for focal length
#     Q = np.float32([[1, 0, 0, -0.5*w],
#                     [0,-1, 0,  0.5*h], # turn points 180 deg around x-axis,
#                     [0, 0, 0,     -f], # so that y-axis looks up
#                     [0, 0, 1,      0]])
#     points = cv2.reprojectImageTo3D(disparity, Q)
#     colors = cv2.cvtColor(img_left, cv2.COLOR_BGR2RGB)
#     mask = disp > disp.min()
#     out_points = points[mask]
#     out_colors = colors[mask]
#     out_fn = 'out.ply'
#     write_ply('out.ply', out_points, out_colors)
#     print('%s saved' % 'out.ply')



if __name__ == "__main__":
#    depth_map_from_cams(1, 2, 640, 480)
    depth_map(cv2.imread('images2/10L.jpg', 0), cv2.imread('images2/10R.jpg', 0))


