/**
 * Capture facial motion by detecting and tracking facial landmarks, and send motion data to Maya server.
 */

#define FACIAL_MOTION
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
#include "FaceTrackerOf.h"
#include "Utils.h"

using namespace dlib;
using namespace std;

bool showFps = false;
bool startSending = false;

int main(int argc, char** argv) {
    try {
        if (argc >= 5 || argc == 3) {
            cout << "Usage:" << endl;
            cout << "./avatar data/shape_predictor_68_face_landmarks.dat localhost 5056" << endl;
            return 0;
        }
        
        std::string poseModelFile = ((argc == 2 || argc == 4) ? argv[1] : "../../data/shape_predictor_68_face_landmarks.dat");
        avt::FaceTrackerOf tracker(poseModelFile);

        const char* host = (argc == 4 ? argv[2] : "localhost");
        int portNum = (argc == 4 ? std::stoi(argv[3]) : 5056);
        avt::SocketClient socketClient;
        socketClient.connect(host, portNum);

        cv::VideoCapture cap1(0);
        if (!cap1.isOpened()) {
            cerr << "Unable to connect to camera" << endl;
            return 1;
        }

        int width = cap1.get(cv::CAP_PROP_FRAME_WIDTH);
        int height = cap1.get(cv::CAP_PROP_FRAME_HEIGHT);

        std::cout << width << "x" << height << std::endl;
        cv::Size size(640, 360);
        std::cout << "resized to: " << size << std::endl;

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
            tracked = tracker.track(tmp, face);
            if (tracked) {
                const std::vector<cv::Point2f>& p1 = tracker.getFaceLandMarks();
                const std::vector<cv::Point2f>& motion = tracker.getMotionTrackPts();

                // Send motion to Maya server
                if (startSending) {
                    socketClient.send2Maya(motion);
                }
            }

            // Flip
            cv::Mat temp;
            cv::flip(face, temp, 1);

            if (showFps) {
                overlayFps(startTime, temp);
            }

            cv::imshow("Facial Motion Capture", temp);

            char key = cv::waitKey(1);
            if (key == 'q') {
                socketClient.close();
                cv::destroyAllWindows();
                break;
            } else if (key == 'r') {
                tracker.reset();
            } else if (key == 'f') {
                showFps = !showFps;
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



#endif
