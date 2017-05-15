/**
 * Estimate head pose using a single camera, and send pose (extrinsic matrix) data to the avatar server.
 *
 * Usage:
 * ./avatar shape_predictor_68_face_landmarks.dat localhost 5055
 *
 */

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
#include "Utils.h"

using namespace dlib;
using namespace std;

bool showFps = true;
bool startTracking = true;
bool startSending = false;

// World points for pose estimation
void constructWorldPoints(std::vector<cv::Point3f>& modelPoints) {
    modelPoints.reserve(6);
    modelPoints.push_back(cv::Point3f(0, 0, 0));//nose
    modelPoints.push_back(cv::Point3f(0, 11, 3.5));//jaw
    modelPoints.push_back(cv::Point3f(-5.5, -6, 3.5));//left corner of left eye
    modelPoints.push_back(cv::Point3f(5.5, -6, 3.5));//right corner of right eye
    modelPoints.push_back(cv::Point3f(-2.5, 3.5, 3));//left corner of mouth
    modelPoints.push_back(cv::Point3f(2.5, 3.5, 3));//right corner of mouth
}

int main(int argc, char** argv) {
    try {
        if (argc >= 5 || argc == 3) {
            cout << "Usage:" << endl;
            cout << "./avatar data/shape_predictor_68_face_landmarks.dat localhost 5055" << endl;
            return 0;
        }
        
        std::string poseModelFile = ((argc == 2 || argc == 4) ? argv[1] : "../../../data/shape_predictor_68_face_landmarks.dat");
        avt::FaceTrackerOf tracker(poseModelFile);

        const char* host = (argc == 4 ? argv[2] : "localhost");
        int portNum = (argc == 4 ? std::stoi(argv[3]) : 5055);
        avt::SocketClient socketClient;
        socketClient.connect(host, portNum);

        // By default, capture vidoe data from device 0 (laptop's default camera).
        cv::VideoCapture cap1(0);
        if (!cap1.isOpened()) {
            cerr << "Unable to connect to camera1" << endl;
            return 1;
        }

        int width, height;
        if (CV_MAJOR_VERSION == 3) {
            width = cap1.get(cv::CAP_PROP_FRAME_WIDTH);
            height = cap1.get(cv::CAP_PROP_FRAME_HEIGHT);
            std::cout << width << "x" << height << std::endl;
        } else if (CV_MAJOR_VERSION == 2) {
            width = cap1.get(CV_CAP_PROP_FRAME_WIDTH);
            height = cap1.get(CV_CAP_PROP_FRAME_HEIGHT);
            std::cout << width << "x" << height << std::endl;
        }

        cv::Size size(640, 360);
        std::cout << "resized to: " << size << std::endl;

        // Preallocation
        
        // Rotation and translation vectors
        cv::Mat rvec(3,1,cv::DataType<double>::type);
        cv::Mat tvec(3,1,cv::DataType<double>::type);

        double GLCoord[9] = {1,0,0,
                             0,1,0,
                             0,0,1};

        // to rotation matrix
        Rodrigues(cv::Mat(3, 3, CV_64FC1, GLCoord), rvec);

        // Landmark positions used for pose estimation
        int poseFaceIds[6] = {33, 8, 36, 45, 48, 54};

        // Axes to draw
        std::vector<cv::Point2f> image_axes(3);
        std::vector<cv::Point3f> world_axes = {cv::Point3f(10, 0, 0),
                                               cv::Point3f(0, 10, 0), 
                                               cv::Point3f(0, 0, -10)};

        // Estimate camera's intrinsic matrix
        double focalLength = size.width;
        cv::Point2d center(size.width / 2, size.height / 2);
        cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << 
                                focalLength, 0.0, center.x,
                                0.0, focalLength, center.y,
                                0.0, 0.0, 1.0);

        cv::Mat distCoeffs(4,1,cv::DataType<double>::type);
        distCoeffs.at<double>(0) = 0;
        distCoeffs.at<double>(1) = 0;
        distCoeffs.at<double>(2) = 0;
        distCoeffs.at<double>(3) = 0;

        // Landmark positions in a right-hand coordinate 3d world frame, centered at nose
        std::vector<cv::Point3f> modelPoints;
        constructWorldPoints(modelPoints);

        std::vector<cv::Point2f> subLandmarks1(6);

        double startTime = 0.0;
        // Grab and process frames until the main window is closed by the user.
        while(true) {
            startTime = (double)cv::getTickCount();

            // Grab and decode the frame
            cv::Mat raw;
            cap1.grab();
            cap1.retrieve(raw);
            // Resize
            cv::Mat tmp;
            cv::resize(raw, tmp, size);

            cv::Mat face = tmp.clone();

            bool tracked = false;
            if (startTracking) {
                tracker.reset();
                tracked = tracker.track(tmp, face);
            }
            if (tracked) {
                const std::vector<cv::Point2f> p1 = tracker.getFaceLandMarks();

                // Estimate pose
                for (int i = 0; i < 6; ++i) {
                    subLandmarks1[i] = p1[poseFaceIds[i]];
                }

                // Must be reset to an initial guess for solvePnP to work properly.
                tvec.at<double>(0, 0) = 0;
                tvec.at<double>(1, 0) = 0;
                tvec.at<double>(2, 0) = 1;
                
                cv::solvePnP(modelPoints,
                             subLandmarks1,
                             cameraMatrix,
                             distCoeffs,
                             rvec, tvec, true, CV_ITERATIVE);
                //                std::cout << "rotation: " << rvec << std::endl;
                std::cout << "translation: " << tvec << std::endl;

                // Convert rotation vector to rotation matrix
                cv::Mat rotMat(3,3,cv::DataType<double>::type);
                cv::Rodrigues(rvec, rotMat);
                std::cout << "rotation matrix: " << rotMat << std::endl;
                cv::Mat projectionMat = (cv::Mat_<double>(4, 4) << 
                                         rotMat.at<double>(0, 0), rotMat.at<double>(0, 1), rotMat.at<double>(0, 2), tvec.at<double>(0, 0),
                                         rotMat.at<double>(1, 0), rotMat.at<double>(1, 1), rotMat.at<double>(1, 2), tvec.at<double>(1, 0),
                                         rotMat.at<double>(2, 0), rotMat.at<double>(2, 1), rotMat.at<double>(2, 2), tvec.at<double>(2, 0),
                                         0, 0, 0, 1);

//                 float x, y, z;
//                 rotationMatrixToEulerAngles(rotMat, x, y, z);


                // Project 3d points back to image points
                std::vector<cv::Point2f> projectedPoints(modelPoints.size());
                cv::projectPoints(modelPoints, rvec, tvec, cameraMatrix, distCoeffs, projectedPoints);

                //Draw projected corners
                for(int i = 0; i < projectedPoints.size(); ++i) {
                    cv::circle(face, projectedPoints[i], 2, cv::Scalar(0, 255, 255), -1);
                }



                // Draw X,Y,Z axes that are reprojected back to the imaegs
                cv::projectPoints(world_axes, rvec, tvec, cameraMatrix, distCoeffs, image_axes);

                cv::line(face, p1[33], image_axes[0], cv::Scalar(0, 0, 255), 2);
                cv::line(face, p1[33], image_axes[1], cv::Scalar(0, 255, 0), 2);
                cv::line(face, p1[33], image_axes[2], cv::Scalar(255, 0, 0), 2);
                
                // Send data to server
                if (startSending) {
                    socketClient.sendModelMatrix2Avatar(projectionMat);
                }
            }

            // Flip
            cv::Mat temp;
            cv::flip(face, temp, 1);

            if (showFps) {
                overlayFps(startTime, temp);
            }

            cv::imshow("HeadPose", temp);

            char key = cv::waitKey(1);
            if (key == 'q') {
                socketClient.close();
                cv::destroyAllWindows();
                break;
            } else if (key == 'r') {
                tracker.reset();
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


