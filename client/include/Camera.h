#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/core/core.hpp>

namespace avt {

    using namespace cv;
    using namespace std;

    struct Camera {

        Camera() {
/*             try { */
/*                 cv::VideoCapture cap(device); */
                
/*                 if (!cap.isOpened()) { */
/*                     cerr << "Unable to connect to camera" << endl; */
/*                     return 1; */
/*                 } */
/*             } catch(exception& e) { */
/*                 cout << e.what() << endl; */
/*             } */
        }

        ~Camera() {}

        friend std::ostream& operator << (std::ostream& os, const Camera& c) {
            os << "cameraMatrix: " << endl << c.cameraMatrix << endl;
            os << "distortionCoeffs: " << endl << c.distortionCoeffs << endl;
            return os;
        }
    
        Mat cameraMatrix;
        Mat distortionCoeffs;
    };
}

#endif /* CAMERA_H */
