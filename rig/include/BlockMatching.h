#ifndef BLOCKMATCHING_H
#define BLOCKMATCHING_H

#include <opencv2/opencv.hpp>
#include <vector>

namespace avt {
    class BlockMatching {
        BlockMatching();
        ~BlockMatching();
        void findStereoCorrespondence(cv::Mat& img1, cv::Mat& img2, const std::vector<cv::Point2f>& p1s, std::vector<cv::Point2f>& p2s, int blockSize, int minDisparity, int maxDisparity);
        void refineStereoCorrespondence(cv::Mat& img1, cv::Mat& img2, const std::vector<cv::Point2f>& p1, std::vector<cv::Point2f>& p2, int blockSize = 15, int neighborRange = 30);
    };
}


#endif /* BLOCKMATCHING_H */
