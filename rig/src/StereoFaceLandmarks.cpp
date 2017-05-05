#ifdef STEREO_FACE
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
#include "Buffer.h"
//#include "PoseEstimator.h"
#include "StereoCamera.h"

using namespace dlib;
using namespace std;

void overlayFps(double t, cv::Mat& img);
void overlayHorizontalLines(cv::Mat& img);
bool showLines = false;
bool showFps = false;
bool startTracking = false;

int main(int argc, char** argv) {
    try {
        if (argc > 2) {
            cout << "Usage:" << endl;
            cout << "./avatar data/shape_predictor_68_face_landmarks.dat" << endl;
            return 0;
        }

        cv::VideoCapture cap1(1);
        cv::VideoCapture cap2(2);
        if (!cap1.isOpened() || !cap2.isOpened()) {
            cerr << "Unable to connect to camera1 or camera2" << endl;
            return 1;
        }

        int width = cap1.get(cv::CAP_PROP_FRAME_WIDTH);
        int height = cap1.get(cv::CAP_PROP_FRAME_HEIGHT);

        std::cout << width << "x" << height << std::endl;
        cv::Size size(320, 240);
        std::cout << "resized to: " << size << std::endl;

        std::string pose_model_file = (argc == 2 ? argv[1] : "../data/shape_predictor_68_face_landmarks.dat");

        avt::SocketClient socketClient;
        avt::FaceTracker tracker(size, pose_model_file);
        avt::FaceTracker tracker2(size, pose_model_file);
//         avt::PoseEstimator pose_estimator(size);
//         int pose_markers[] = {30, 8, 36, 45, 48, 54};
//         std::vector<cv::Point2d> image_points(6);
        avt::StereoCamera stereoCamera(cv::Size(32, 32), size);
        std::cout << stereoCamera << std::endl;

        double startTime = 0.0;
        // Grab and process frames until the main window is closed by the user.
        while(true) {
            startTime = (double)cv::getTickCount();

            // Grab and decode left right frames
            cv::Mat raw, raw2;
            cap1.grab();
            cap2.grab();
            cap1.retrieve(raw);
            cap2.retrieve(raw2);
            
            // Resize
            cv::Mat tmp, tmp2;
            cv::resize(raw, tmp, size);
            cv::resize(raw2, tmp2, size);

            // Undistort & Rectify
            cv::Mat img1, img2;
            stereoCamera.undistortRectify(tmp, tmp2, img1, img2);

            // Flip
            cv::Mat temp, temp2;
            cv::flip(img1, temp, 1);
            cv::flip(img2, temp2, 1);

            cv::Mat face = temp.clone();
            cv::Mat face2 = temp2.clone();

            bool tracked = false, tracked2 = false;
            if (startTracking) {
                tracker.reset();
                tracker2.reset();
                tracked = tracker.track(temp);
                tracked2 = tracker2.track(temp2);
            }
            if (tracked || tracked2) {
                const avt::TrackingData& data = tracker.get_face_landmarks();
                const avt::TrackingData& data2 = tracker2.get_face_landmarks();

                const avt::TrackingDataf& move = tracker.get_movement();
                //                std::cout << move << std::endl;

                const dlib::full_object_detection& d = tracker.get_all_landmarks();
                const dlib::full_object_detection& d2 = tracker2.get_all_landmarks();
                
                for (int i = 0; i < d.num_parts(); ++i) {
                    cv::circle(face, cv::Point(d.part(i).x(), d.part(i).y()), 2, cv::Scalar(0, 0, 255), -1);
                    cv::circle(face2, cv::Point(d2.part(i).x(), d2.part(i).y()), 2, cv::Scalar(0, 0, 255), -1);
                }


//                 for (int i = 0; i < image_points.size(); ++i) {
//                     image_points[i] = cv::Point2d(d.part(pose_markers[i]).x(), d.part(pose_markers[i]).y());
//                 }
//                 pose_estimator.estimate(image_points);
//                 cv::Mat rotation_vec = pose_estimator.get_rotation_vec();
//                 cv::Mat translation_vec = pose_estimator.get_translation_vec();
//                 std::cout << "rotation: " << rotation_vec << std::endl;
//                 tracker.set_pose(rotation_vec);
                
                // Send data to server
                //                socketClient.send2Avatar(move);
            }

            if (showLines) {
                overlayHorizontalLines(face);
                overlayHorizontalLines(face2);
            }
            if (showFps) {
                overlayFps(startTime, face2);
            }

            cv::Mat hmat;
            cv::hconcat(face2, face, hmat);

            cv::imshow("stereo", hmat);

            char key = cv::waitKey(1);
            if (key == 'q') {
                socketClient.close();
                cv::destroyAllWindows();
                break;
            } else if (key == 'r') {
                tracker.reset();
                tracker2.reset();
            } else if (key == 'h') {
                showLines = !showLines;
            } else if (key == 'f') {
                showFps = !showFps;
            } else if (key == 't') {
                startTracking = !startTracking;
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

#endif
