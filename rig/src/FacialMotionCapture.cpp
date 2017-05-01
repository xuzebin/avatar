//#define FACIAL_MOTION
#ifdef FACIAL_MOTION
#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>

#include "TrackingData.h"
#include "SocketClient.h"
#include "FaceTracker.h"
#include "FaceTrackerOf.h"
#include "PoseEstimator.h"
#include "StereoCamera.h"

using namespace dlib;
using namespace std;

void overlayFps(double t, cv::Mat& img);
void overlayHorizontalLines(cv::Mat& img);
bool showLines = false;
bool showFps = false;
bool startTracking = true;
bool startSending = false;
void rotationMatrixToEulerAngles(const cv::Mat &R, float&x, float& y, float& z);

int main(int argc, char** argv) {
    try {
        if (argc > 2) {
            cout << "Usage:" << endl;
            cout << "./avatar data/shape_predictor_68_face_landmarks.dat" << endl;
            return 0;
        }

        cv::VideoCapture cap1(1);
        if (!cap1.isOpened()) {
            cerr << "Unable to connect to camera1" << endl;
            return 1;
        }

        int width = cap1.get(cv::CAP_PROP_FRAME_WIDTH);
        int height = cap1.get(cv::CAP_PROP_FRAME_HEIGHT);

        std::cout << width << "x" << height << std::endl;
        cv::Size size(320, 240);
        std::cout << "resized to: " << size << std::endl;

        std::string pose_model_file = (argc == 2 ? argv[1] : "../data/shape_predictor_68_face_landmarks.dat");

        avt::SocketClient socketClient;
        socketClient.connect("localhost", 5056);
        avt::FaceTrackerOf tracker(pose_model_file);
        avt::StereoCamera stereoCamera(cv::Size(32, 32), size);
        std::cout << stereoCamera << std::endl;

        double startTime = 0.0;
        // Grab and process frames until the main window is closed by the user.
        while(true) {
            startTime = (double)cv::getTickCount();

            // Grab and decode left right frames
            cv::Mat raw;
            cap1.grab();
            cap1.retrieve(raw);
            // Resize
            cv::Mat tmp;
            cv::resize(raw, tmp, size);

            cv::Mat face = tmp.clone();
            bool tracked = false;
            //            tracker.reset();
            tracked = tracker.track(tmp, face);
            if (tracked) {
                const std::vector<cv::Point2f> p1 = tracker.getFaceLandMarks();
                const std::vector<cv::Point2f> motion = tracker.getMotionTrackPts();
                
//                 for (int i = 0; i < d.num_parts(); ++i) {
//                     cv::circle(face, cv::Point(d.part(i).x(), d.part(i).y()), 2, cv::Scalar(0, 0, 255), -1);
//                     cv::circle(face2, cv::Point(d2.part(i).x(), d2.part(i).y()), 2, cv::Scalar(0, 0, 255), -1);
//                 }


                // Estimate pose
                cv::Mat rvec(3,1,cv::DataType<double>::type);
                cv::Mat tvec(3,1,cv::DataType<double>::type);

                double GLCoord[9] = {1,0,0,
                                     0,1,0,
                                     0,0,1};

                Rodrigues(cv::Mat(3, 3, CV_64FC1, GLCoord), rvec);
                tvec.at<double>(0, 0) = 0; tvec.at<double>(1, 0) = 0; tvec.at<double>(2, 0) = 1;
                
                cv::Mat distCoeffs(4,1,cv::DataType<double>::type);
                distCoeffs.at<double>(0) = -0.41979;
                distCoeffs.at<double>(1) = 0.206977;
                distCoeffs.at<double>(2) = 0;
                distCoeffs.at<double>(3) = 0;
                cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << 
                                 564.0351649907346, 0.0, 343.3765330243276,
                                 0.0, 564.0351649907346, 201.3194373417247,
                                 0.0, 0.0, 1.0);
                cv::Mat cameraMat33 = cameraMatrix(cv::Rect(0, 0, 3, 3));

                std::vector<cv::Point3f> modelPoints(6);
                modelPoints[0] = cv::Point3f(0, 0, 0);//nose
                modelPoints[1] = cv::Point3f(0, 11, 3.5);//jaw
                modelPoints[2] = cv::Point3f(-5.5, -6, 3.5);//left eye brow
                modelPoints[3] = cv::Point3f(5.5, -6, 3.5);//right eye brow
                modelPoints[4] = cv::Point3f(-2.5, 3.5, 3);//left mouth
                modelPoints[5] = cv::Point3f(2.5, 3.5, 3);//right mouth


                int poseFaceIds[6] = {33, 8, 36, 45, 48, 54};
                std::vector<cv::Point2f> subLandmarks1(6);
                for (int i = 0; i < 6; ++i) {
                    subLandmarks1[i] = p1[poseFaceIds[i]];
                }
                cv::solvePnP(modelPoints,
                             subLandmarks1,
                             cameraMat33,
                             distCoeffs,
                             rvec, tvec, true, CV_ITERATIVE);//CV_EPNP);CV_ITERATIVE);

//                 cv::Mat inliers;
//                 cv::solvePnPRansac(modelPoints,
//                                    subLandmarks1,
//                                    cameraMat33,
//                                    distCoeffs,
//                                    rvec, tvec, false, 100, 8.0, 0.95, inliers, CV_EPNP);//CV_ITERATIVE);

                //                std::cout << "rotation: " << rvec << std::endl;
                //                std::cout << "translation: " << tvec << std::endl;


                // Convert rotation vector to rotation matrix
                cv::Mat rotMat(3,3,cv::DataType<double>::type);
                cv::Rodrigues(rvec, rotMat);
                //                std::cout << "rotation matrix: " << rotMat << std::endl;
                
                cv::Mat projectionMat = (cv::Mat_<double>(4, 4) << 
                                         rotMat.at<double>(0, 0), rotMat.at<double>(0, 1), rotMat.at<double>(0, 2), tvec.at<double>(0, 0),
                                         rotMat.at<double>(1, 0), rotMat.at<double>(1, 1), rotMat.at<double>(1, 2), tvec.at<double>(1, 0),
                                         rotMat.at<double>(2, 0), rotMat.at<double>(2, 1), rotMat.at<double>(2, 2), tvec.at<double>(2, 0),
                                         0, 0, 0, 1);
                

                float x, y, z;
                rotationMatrixToEulerAngles(rotMat, x, y, z);

                // Project 3d points back to image points
                std::vector<cv::Point2f> projectedPoints;
                cv::projectPoints(modelPoints, rvec, tvec, cameraMat33, distCoeffs, projectedPoints);
                //Draw projected corners
                for(int i = 0; i < projectedPoints.size(); ++i) {
                    cv::circle(face, projectedPoints[i], 2, cv::Scalar(0, 255, 255), -1);
                }

                // Draw X,Y,Z axes
                std::vector<cv::Point3f> axis;
                std::vector<cv::Point2f> image_axes;
                axis.push_back(cv::Point3f(10, 0, 0));
                axis.push_back(cv::Point3f(0, 10, 0));
                axis.push_back(cv::Point3f(0, 0, -10));
                cv::projectPoints(axis, rvec, tvec, cameraMat33, distCoeffs, image_axes);

                cv::line(face, p1[33], image_axes[0], cv::Scalar(0, 0, 255), 2);
                cv::line(face, p1[33], image_axes[1], cv::Scalar(0, 255, 0), 2);
                cv::line(face, p1[33], image_axes[2], cv::Scalar(255, 0, 0), 2);

                // Send data to server
                if (startSending) {
                    //                    socketClient.sendModelMatrix2Avatar(projectionMat);
                    socketClient.send2Maya(motion);
                }

            }

            if (showLines) {
                overlayHorizontalLines(face);
            }
            if (showFps) {
                overlayFps(startTime, face);
            }

            // Flip
            cv::Mat temp;
            cv::flip(face, temp, 1);

            cv::imshow("HeadPose", temp);

            char key = cv::waitKey(1);
            if (key == 'q') {
                socketClient.close();
                cv::destroyAllWindows();
                break;
            } else if (key == 'r') {
                tracker.reset();
            } else if (key == 'h') {
                showLines = !showLines;
            } else if (key == 'f') {
                showFps = !showFps;
            } else if (key == 't') {
                startTracking = !startTracking;
            } else if (key == 's') {
                startSending = !startSending;
            }
        }
    } catch(serialization_error& e) {
        cout << "You need dlib's default face landmarking model file to run this example." << endl;
        cout << "You can get it from the following URL: " << endl;
        cout << "   http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
        cout << endl << e.what() << endl;
    }
    catch(exception& e) {
        cout << e.what() << endl;
    }

    return 0;
}


void overlayHorizontalLines(cv::Mat& img) {
    static const int LINES_NUM = 10;
    int interval = img.rows / LINES_NUM;
    int col = img.cols;
    for (int i = 1; i <= LINES_NUM; i++) {
        cv::Point p1(0, i * interval);
        cv::Point p2(col, i * interval);
        cv::Scalar color(0, 255, 0);
        cv::line(img, p1, p2, color, 1);
    }
}
void overlayFps(double t, cv::Mat& face) {  
    //Calculate fps
    double fps;
    char string[10];
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    fps = 1.0 / t;

    //Save it in a string
    sprintf(string, "%.2f", fps);      
    std::string fpsString("FPS:");
    fpsString += string; 
                  
    //output
    putText(face, // matrix
            fpsString,                  // string
            cv::Point(5, 20),           // coordinate
            cv::FONT_HERSHEY_SIMPLEX,   // font
            0.5, // size
            cv::Scalar(0, 0, 255));       // color
}


// Checks if a matrix is a valid rotation matrix.
bool isRotationMatrix(const cv::Mat &R) {
    cv::Mat Rt;
    transpose(R, Rt);
    cv::Mat shouldBeIdentity = Rt * R;
    cv::Mat I = cv::Mat::eye(3,3, shouldBeIdentity.type());
     
    return  norm(I, shouldBeIdentity) < 1e-6;
}
 
// Calculates rotation matrix to euler angles
// The result is the same as MATLAB except the order
// of the euler angles ( x and z are swapped ).
void rotationMatrixToEulerAngles(const cv::Mat &R, float&x, float& y, float& z) {
    assert(isRotationMatrix(R));
     
    float sy = sqrt(R.at<double>(0,0) * R.at<double>(0,0) +  R.at<double>(1,0) * R.at<double>(1,0));
 
    bool singular = sy < 1e-6;
 
    //    float x, y, z;
    if (!singular) {
        x = atan2(R.at<double>(2,1) , R.at<double>(2,2));
        y = atan2(-R.at<double>(2,0), sy);
        z = atan2(R.at<double>(1,0), R.at<double>(0,0));
    } else {
        x = atan2(-R.at<double>(1,2), R.at<double>(1,1));
        y = atan2(-R.at<double>(2,0), sy);
        z = 0;
    }
    x = x * 180.0 / 3.1415926;
    y = y * 180.0 / 3.1415926;
    z = z * 180.0 / 3.1415926;
    std::cout << "euler angle: " << x << "," << y << "," << z << std::endl;
}
#endif
