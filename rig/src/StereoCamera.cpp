#include "StereoCamera.h"
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

namespace avt {

    using namespace std;

    StereoCamera::StereoCamera(cv::Size sensorSize, cv::Size realSize) {
        camera1 = new Camera();
        camera2 = new Camera();

        this->sensorSize = sensorSize;
        this->px = (double)sensorSize.width / realSize.width;
        this->realSize = realSize;
        
        init(realSize);

        initRemap(realSize);
    }

    StereoCamera::~StereoCamera() {
        delete camera1;
        delete camera2;
    }

    ostream& operator << (ostream& os, const StereoCamera& c) {
        os << "R: " << endl << c.R << endl;
        os << "T: " << endl << c.T << endl;
        os << "E: " << endl << c.E << endl;
        os << "F: " << endl << c.F << endl;
        os << "leftCamera: " << endl << *(c.camera1) << endl;
        os << "rightCamera: " << endl << *(c.camera2) << endl;
        return os;
    }

    void StereoCamera::initRemap(cv::Size realSize) {
        //        cv::Mat remap[2][2];
        getUndistortRectifyMap(realSize, remap);
    }

    void StereoCamera::undistortRectify(const cv::Mat& img1, const cv::Mat& img2, cv::Mat& imgOut1, cv::Mat& imgOut2) {
        assert(img1.size() == img2.size());
//         cv::Size imgSize(img1.cols, img1.rows);
//         cv::Mat remap[2][2];
        //        getUndistortRectifyMap(imgSize, remap);
        cv::remap(img1, imgOut1, remap[0][0], remap[0][1], INTER_LINEAR);
        cv::remap(img2, imgOut2, remap[1][0], remap[1][1], INTER_LINEAR);
    }

    void StereoCamera::getUndistortRectifyMap(const cv::Size& imgSize, cv::Mat remap[2][2]) {
        //Stereo rectify
        cv::Rect validRoi[2];

        cv::stereoRectify(camera1->cameraMatrix, camera1->distortionCoeffs,
                          camera2->cameraMatrix, camera2->distortionCoeffs,
                          imgSize, R, T, R1, R2, P1, P2, Q,
                          0, 0, imgSize, &validRoi[0], &validRoi[1]);

//         cout << "R1:" << endl << R1 << endl;
//         cout << "R2:" << endl << R2 << endl;
//         cout << "P1:" << endl << P1 << endl;
//         cout << "P2:" << endl << P2 << endl;
//         cout << "Q:" << endl << Q << endl;


        //TODO Precompute maps for cv::remap
        /*             cv::initUndistortRectifyMap(camera1->cameraMatrix, camera1->distortionCoeffs, R1, P1, imgSize, CV_32FC1, remap[0][0], remap[0][1]); */
        /*             cv::initUndistortRectifyMap(camera2->cameraMatrix, camera2->distortionCoeffs, R2, P2, imgSize, CV_32FC1, remap[1][0], remap[1][1]); */
        cv::initUndistortRectifyMap(camera1->cameraMatrix, camera1->distortionCoeffs, R1, P1, imgSize, CV_16SC2, remap[0][0], remap[0][1]);
        cv::initUndistortRectifyMap(camera2->cameraMatrix, camera2->distortionCoeffs, R2, P2, imgSize, CV_16SC2, remap[1][0], remap[1][1]);
    }

    void StereoCamera::init(cv::Size actualSize) {
        R = (cv::Mat_<double>(3, 3) <<
             0.9999791642320877, -0.0064549653431208096, 6.726153924038991e-05,
             0.00645521731525248, 0.9999671767003152, -0.004896497872304895,
             -3.5652607426324446e-05, 0.004896830037864399, 0.9999880098203537);
        T = (cv::Mat_<double>(3, 1) << -92.07132474097523, -0.9503618376306747, 0.03449382486186875);
        E = (cv::Mat_<double>(3, 3) << 
             -0.0001887822580076149, -0.03914645305406793, -0.9501815436814688,
             0.031210523360327896, 0.45063497217354004, 92.07022310935906,
             0.3560016264058158, -92.07443720901621, 0.4508909684945126);
        F = (cv::Mat_<double>(3, 3) <<
             2.9203772606723024e-10, 6.055781541211587e-08, 0.0008167770204985497,
             -4.828128642867844e-08, -6.971121860117902e-07, -0.08017777582695934,
             -0.00030066159094108906, 0.08060853101476491, 1.0);
        camera1->cameraMatrix = (cv::Mat_<double>(3, 3) << 
                                 564.0351649907346, 0.0, 343.3765330243276,
                                 0.0, 564.0351649907346, 201.3194373417247,
                                 0.0, 0.0, 1.0);
        camera2->cameraMatrix = (cv::Mat_<double>(3, 3) << 
                                 564.982507555492, 0.0, 286.8738893670728,
                                 0.0, 564.982507555492, 218.89591686665315,
                                 0.0, 0.0, 1.0);

        camera1->distortionCoeffs = (cv::Mat_<double>(1, 14) <<
                                     -0.41979467401240395, 0.20697666630377856,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
        camera2->distortionCoeffs = (cv::Mat_<double>(1, 14) <<
                                     -0.4252824437565117, 0.2306932362524328,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
            
        imageSize = cv::Size(640, 480);

        if (imageSize != actualSize) {
            assert(imageSize.width > 0 && imageSize.height > 0);
            double sw = (double)actualSize.width / imageSize.width;
            double sh = (double)actualSize.height / imageSize.height;
            cout << "scaling factor: " << sw << ", " << sh << endl;

            camera1->cameraMatrix.at<double>(0, 0) *= sw;
            camera1->cameraMatrix.at<double>(0, 2) *= sw;
            camera1->cameraMatrix.at<double>(1, 1) *= sh;
            camera1->cameraMatrix.at<double>(1, 2) *= sh;

            camera2->cameraMatrix.at<double>(0, 0) *= sw;
            camera2->cameraMatrix.at<double>(0, 2) *= sw;
            camera2->cameraMatrix.at<double>(1, 1) *= sh;
            camera2->cameraMatrix.at<double>(1, 2) *= sh;
        }
        /*             R = (cv::Mat_<double>(3, 3) << */
        /*                  1.0, 0.0054, 0.0047, */
        /*                  -0.0055, 0.9996, 0.0281, */
        /*                  -0.0045, -0.0281, 0.9996); */
        /*             R = (cv::Mat_<double>(3, 3) << */
        /*                  0.9999400510564399, -0.005872707123126066, -0.009241515259430181, */
        /*                  0.005613526875871187, 0.9995970323017617, -0.02782555173801323, */
        /*                  0.009401202543194587, 0.02777200613129998, 0.9995700741149591); */

        /*             T = (cv::Mat_<double>(3, 1) << -92.2289, -1.8742, 1.8302); */
        /*             T = (cv::Mat_<double>(3, 1) << -92.33655419332558, -1.8895733182055152, -0.11352047572711924); */
        /*             E = (cv::Mat_<double>(3, 3) <<  */
        /*                  -0.0186, -1.8822, -1.8219, */
        /*                  2.2624, 2.5820, 92.1831, */
        /*                  1.3797, -92.2014, 2.5859); */
        /*             E = (cv::Mat_<double>(3, 3) <<  */
        /*                  -0.017127011243210456, 0.06099748886356814, -1.8919197115950048, */
        /*                  0.7545609778175904, 2.5650380217065742, 92.29790541975109, */
        /*                  1.37112631169174, -92.31044245530454, 2.5518530453622352); */
        /*             F = (cv::Mat_<double>(3, 3) << */
        /*                  -5.7477e-08, -5.83e-06, -0.0020, */
        /*                  6.9987e-06, 8.0013e-06, 0.1579, */
        /*                  9.2399e-04, -0.1629, 0.1455); */

        /*             F = (cv::Mat_<double>(3, 3) << */
        /*                  -9.779393985712206e-07, 3.482910516406645e-06, -0.06192919902090507, */
        /*                  4.308486158814036e-05, 0.00014646173256028242, 2.9610298427112283, */
        /*                  0.035760906006510794, -3.0456239992821517, 1.0); */

        /*             camera1->cameraMatrix = (cv::Mat_<double>(3, 3) <<  */
        /*                                      567.6214, 0.0, 0.0, */
        /*                                      1.1269, 566.4352, 0.0, */
        /*                                      342.5395, 197.6715, 1.0); */
        /*             camera2->cameraMatrix = (cv::Mat_<double>(3, 3) <<  */
        /*                                      569.9420, 0.0, 0.0, */
        /*                                      1.0759, 569.5006, 0.0, */
        /*                                      282.9639, 217.5993, 1.0); */

        /*             camera1->distortionCoeffs = (cv::Mat_<double>(1, 14) << */
        /*                                          -0.4132, 0.2,  */
        /*                                          0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0); */
        /*             camera2->distortionCoeffs = (cv::Mat_<double>(1, 14) << -0.4199, 0.2080, */
        /*                                          0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0); */

        /*             camera1->cameraMatrix = (cv::Mat_<double>(3, 3) <<  */
        /*                                      570.0976536080545, 0.0, 344.7959814265909, */
        /*                                      0.0, 570.0976536080545, 195.33762661869207, */
        /*                                      0.0, 0.0, 1.0); */
        /*             camera2->cameraMatrix = (cv::Mat_<double>(3, 3) <<  */
        /*                                      571.6510518719991, 0.0, 287.9213500551263, */
        /*                                      0.0, 571.6510518719991, 215.28170270393227, */
        /*                                      0.0, 0.0, 1.0); */

        /*             camera1->distortionCoeffs = (cv::Mat_<double>(1, 14) << */
        /*                                          -0.41768432007095896, 0.21049421555652553,  */
        /*                                          0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0); */
        /*             camera2->distortionCoeffs = (cv::Mat_<double>(1, 14) << -0.4160474716070939, 0.19746555657692402, */
        /*                                          0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0); */
    }

}


