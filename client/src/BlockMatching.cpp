#include "BlockMatching.h"

namespace avt {

    BlockMatching::BlockMatching() {
    }

    BlockMatching::~BlockMatching() {
    }

    inline void truncate(int& v, const int maxValue) {
        if (v < 0) {
            v = 0;
        } else if (v >= maxValue) {
            v = maxValue - 1;
        }
    }

    inline void truncate(int& row, int& col, const int maxRow, const int maxCol) {
        truncate(row, maxRow);
        truncate(col, maxCol);
    }

    // the input stereo images must be rectified.
    void BlockMatching::findStereoCorrespondence(cv::Mat& img1, cv::Mat& img2, const std::vector<cv::Point2f>& p1s, std::vector<cv::Point2f>& p2s, int blockSize, int minDisparity, int maxDisparity) {
        assert(blockSize % 2 != 0 && blockSize > 1);
        assert(img1.size == img2.size);

        int cols = img1.cols;
        int rows = img1.rows;

        for (int i = 0; i < 1; ++i) {
//         for (int i = 0; i < p1s.size(); ++i) {
            const cv::Point2f& p1 = p1s[i];

            int halfBlock = (blockSize - 1) / 2;
            int minx = p1.x - halfBlock;
            int miny = p1.y - halfBlock;
            int maxx = p1.x + halfBlock;
            int maxy = p1.y + halfBlock;

            minx = minx < 0 ? 0 : minx;
            miny = miny < 0 ? 0 : miny;
            maxx = maxx >= cols ? (cols - 1) : maxx;
            maxy = maxy >= rows ? (rows - 1) : maxy;

            std::cout << p1 << std::endl;
            std::cout << "minx: " << minx << std::endl;
            std::cout << "miny: " << miny << std::endl;
            std::cout << "maxx: " << maxx << std::endl;
            std::cout << "maxy: " << maxy << std::endl;

            if (minx < 0 || miny < 0 || maxx >= cols || maxy >= rows) {
                std::cerr << "======================INVALID BOUND======================" << std::endl;
            }

            cv::Mat b1 = img1(cv::Range(miny, maxy + 1), cv::Range(minx, maxx + 1));
            // Draw bbox
            cv::rectangle(img1, cv::Rect(minx, miny, (blockSize - 1), (blockSize - 1)), cv::Scalar(0, 255, 0), 1);

            // Search along epipolar line in img2
            int startx = (minx + minDisparity >= cols) ? (cols - 1): (minx + minDisparity);
            //        int endx = (maxx + minDisparity + maxDisparity >= cols) ? (cols - 1) : (maxx + minDisparity + maxDisparity);
            int endx = (maxx + maxDisparity >= cols) ? (cols - 1) : (maxx + maxDisparity);
            std::cout << "start: " << startx << std::endl;
            std::cout << "end: " << endx << std::endl;
            if (startx < endx) {
                cv::Mat subImg2 = img2(cv::Range(miny, maxy + 1), cv::Range(startx, endx + 1));
                cv::rectangle(img2, cv::Point(startx, miny), cv::Point(endx, maxy), cv::Scalar(0, 255, 0), 1);
                cv::Mat result;
                cv::matchTemplate(subImg2, b1, result, CV_TM_CCORR_NORMED);//CV_TM_SQDIFF_NORMED
                cv::imshow("result", result);
                //Draw search range box
                //             cv::rectangle(img2, cv::Rect(((minx + minDisparity >= rows) ? (rows - 1) : (minx + minDisparity)), 
                //                                          miny, maxDisparity - 1, (blockSize - 1)), 
                //                           cv::Scalar(0, 255, 0), 1);

                // Find best match
                double minVal, maxVal;
                cv::Point minLoc, maxLoc;
                cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
                std::cout << "maxloc: " << minLoc << ", maxval: " << minVal << std::endl;
                cv::circle(img2, cv::Point(startx + minLoc.x , p1.y), 2, cv::Scalar(0, 0, 255), 2);
            }


        }
    }
    void BlockMatching::refineStereoCorrespondence(cv::Mat& img1, cv::Mat& img2, 
                                                   const std::vector<cv::Point2f>& p1s, std::vector<cv::Point2f>& p2s, 
                                                   int blockSize, int neighborRange) {
        assert(blockSize % 2 != 0 && blockSize >= 3);
        assert(neighborRange % 2 != 0 && neighborRange >= 3);
        assert(img1.size == img2.size);

        int cols = img1.cols;
        int rows = img1.rows;

        for (int i = 32; i < 33; ++i) { //p1s.size(); ++i) {
            const cv::Point2f& p1 = p1s[i];
            cv::Point2f& p2 = p2s[i];

            int halfBlock = (blockSize - 1) / 2;
            int minx = p1.x - halfBlock;
            int miny = p1.y - halfBlock;
            int maxx = p1.x + halfBlock;
            int maxy = p1.y + halfBlock;

            minx = minx < 0 ? 0 : minx;
            miny = miny < 0 ? 0 : miny;
            maxx = maxx >= cols ? (cols - 1) : maxx;
            maxx = maxx < 0 ? 0 : maxx;
            maxy = maxy >= rows ? (rows - 1) : maxy;
            maxy = maxy < 0 ? 0 : maxy;

            std::cout << p1 << std::endl;
            std::cout << "minx: " << minx << std::endl;
            std::cout << "miny: " << miny << std::endl;
            std::cout << "maxx: " << maxx << std::endl;
            std::cout << "maxy: " << maxy << std::endl;

            if (minx < 0 || miny < 0 || maxx >= cols || maxy >= rows) {
                std::cerr << "======================INVALID BOUND======================" << std::endl;
            }
            cv::Mat b1 = img1(cv::Range(miny, maxy + 1), cv::Range(minx, maxx + 1));
            // Draw bbox
            cv::rectangle(img1, cv::Rect(minx, miny, (blockSize - 1), (blockSize - 1)), cv::Scalar(0, 255, 0), 1);

            // Search along epipolar line in img2
            int startx = p2.x - (neighborRange - 1) / 2;
            int endx = p2.x + (neighborRange - 1) / 2;
            startx = startx < 0 ? 0 : startx;
            startx = startx > cols ? cols - 1 : startx;
            endx = endx < 0 ? 0 : endx;
            endx = endx > cols ? cols - 1 : endx;
            std::cout << "start: " << startx << std::endl;
            std::cout << "end: " << endx << std::endl;

            std::cout << "sub image : (" << miny << "," << maxy + 1 << "), (" << startx << "," << endx + 1 << ")" << std::endl;

            if (startx < endx) {
                // Compensate for not aligned horizontal lines
                int offsety = -3;
                int miny2 = miny + offsety;
                int maxy2 = maxy + offsety;
                truncate(miny2, rows);
                truncate(maxy2, rows);

                std::cout << "miny2, maxy2: " << miny2 << "," << maxy2 << std::endl;
                
                cv::Mat subImg2 = img2(cv::Range(miny2, maxy2 + 1), cv::Range(startx, endx + 1));
                cv::rectangle(img2, cv::Point(startx, miny2), cv::Point(endx, maxy2), cv::Scalar(0, 255, 0), 1);
                cv::Mat result;

                if (maxy2 - miny2 == maxy - miny) {
                    cv::matchTemplate(subImg2, b1, result, CV_TM_CCORR_NORMED);//CV_TM_SQDIFF_NORMED
                    cv::imshow("result", result);
                }

                // Find best match
                double minVal, maxVal;
                cv::Point minLoc, maxLoc;
                cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
                std::cout << "maxloc: " << minLoc << ", maxval: " << minVal << std::endl;
                cv::rectangle(img2, cv::Point(startx + maxLoc.x, miny2), cv::Point(startx + maxLoc.x + b1.cols, miny2 + b1.rows), cv::Scalar::all(0), 2, 8, 0);
                int matchedPointx = startx + maxLoc.x + halfBlock;
                int matchedPointy = miny2 + halfBlock;
                truncate(matchedPointx, cols);
                truncate(matchedPointy, rows);
                cv::circle(img2, cv::Point(matchedPointx, matchedPointy), 2, cv::Scalar(255, 0, 0), 2);
            }
        }
    }
}
