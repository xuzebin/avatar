#ifndef STEREOCAMERA_H
#define STEREOCAMERA_H
#include "Camera.h"
#include <string>

namespace avt {

    using namespace std;

    class StereoCamera {

    public:
        StereoCamera(cv::Size sensorSize, cv::Size actualSize);
        ~StereoCamera();

        void undistortRectify(const cv::Mat& img1, const cv::Mat& img2, cv::Mat& imgOut1, cv::Mat& imgOut2);
        void getUndistortRectifyMap(const cv::Size& imgSize, cv::Mat remap[2][2]);

        friend ostream& operator << (ostream& os, const StereoCamera& c);

        const cv::Mat& getLeftCameraMatrix() const {
            return camera1->cameraMatrix;
        }

        const cv::Mat& getRightCameraMatrix() const {
            return camera2->cameraMatrix;
        }

        const cv::Mat& getLeftDistortionCoeffs() const {
            return camera1->distortionCoeffs;
        }

        const cv::Mat& getRightDistortionCoeffs() const {
            return camera2->distortionCoeffs;
        }
        
        // Focal length after recitfication
        // This is not the orignal focal length (already multiplied by a scaling factor)
        double getFocalLength() const {
            return P1.at<double>(0, 0);
        }

        double getBaseline() const {
            return T.at<double>(0, 0);
        }

        // This is not the original sensor's principal point.
        // This is the principal point after rectificaiton an a scaling.
        cv::Point2f getPrincipalPoint() const {
            return cv::Point2f(Q.at<double>(0, 3), Q.at<double>(1, 3));
        }

        // Return the left camera matrix after rectification
        const cv::Mat& getCameraMatrix1() const {
            return P1;
        }
        const cv::Mat& getCameraMatrix2() const {
            return P2;
        }

        const cv::Size& getSensorSize() const {
            return sensorSize;
        }

        double getPixelSize() const {
            return px;
        }

/*         void readCalibrationFile(const std::string& filename) { */
/*         } */
    private:
        
        // Hardcode the intrinsic and extrinsic parameters.
        // Note that the intrinsic matrices are calibrated from 640x480 image pairs.
        // If the image pair to be rectified have a different size than 640x480, it must multiply some of the items (fx, fy, cx, cy) in the intrinsic matrices by the scaling factor.
        void init(cv::Size actualSize);

        Camera* camera1;
        Camera* camera2;

        double px;//pixel size in x (mm/px)
        cv::Size sensorSize;//mm

    public:
        cv::Mat R;
        cv::Mat T;
        cv::Mat E;
        cv::Mat F;
        cv::Size imageSize;

        cv::Mat R1, R2;
        cv::Mat P1, P2;
        cv::Mat Q;

    };

}

#endif /* STEREOCAMERA_H */
