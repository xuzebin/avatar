#ifndef FACETRACKEROF_H
#define FACETRACKEROF_H

#include <dlib/opencv.h>
#include <opencv2/video/tracking.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

namespace avt {
    using namespace dlib;
    using namespace cv;


    class FaceTrackerOf {
    public:
        FaceTrackerOf(const std::string& poseModelFile) {
            flag = -1;
            startTracking = false;
            resetNeutral = false;
            detectFace = 0;
            detectLandMark = true;
            neutralTrackPts.reserve(68);
            motionTrackPts.reserve(68);
            alignedTrackPts.reserve(68);
            
            for (int i = 0; i < 68; ++i) {
                neutralTrackPts.push_back(cv::Point2f(0.0, 0.0));
                motionTrackPts.push_back(cv::Point2f(0.0, 0.0));
                alignedTrackPts.push_back(cv::Point2f(0.0, 0.0));
            }

            kalmanPoints.reserve(68);
            for (int i = 0; i < 68; ++i) {
                kalmanPoints.push_back(cv::Point2f(0.0, 0.0));
            }
            predictPoints.reserve(68);
            for (int i = 0; i < 68; ++i) {
                predictPoints.push_back(cv::Point2f(0.0, 0.0));
            }
            
            // Load face detection and pose estimation models.
            detector = dlib::get_frontal_face_detector();
            dlib::deserialize(poseModelFile) >> poseModel;

            initKalmanFilter();
            initOptialFlow();
        }

        ~FaceTrackerOf() {
            delete KF;
            delete state;
            delete processNoise;
            delete measurement;
        }
        
        void reset() {
            resetNeutral = true;
            startTracking = true;
        }

        void resetPoints() {
            for (int i = 0; i < 68; ++i) {
                neutralTrackPts[i] = nextTrackPts[i];
                motionTrackPts[i].x = 0;
                motionTrackPts[i].y = 0; 
            }
        }

        void initKalmanFilter() {
            KF = new KalmanFilter(stateNum, measureNum, 0);
            state = new cv::Mat(stateNum, 1, CV_32FC1);
            processNoise = new cv::Mat(stateNum, 1, CV_32F);//not used
            measurement = new cv::Mat(Mat::zeros(measureNum, 1, CV_32F));


            // Generate a matrix randomly
            randn(*state, Scalar::all(0), Scalar::all(0.0));

            // Generate the Measurement Matrix
            KF->transitionMatrix = Mat::zeros(stateNum, stateNum, CV_32F);
            for (int i = 0; i < stateNum; i++) {
                for (int j = 0; j < stateNum; j++) {
                    if (i == j || (j - measureNum) == i) {
                        KF->transitionMatrix.at<float>(i, j) = 1.0;
                    } else {
                        KF->transitionMatrix.at<float>(i, j) = 0.0;
                    }   
                }
            }

            //!< measurement matrix (H)
            setIdentity(KF->measurementMatrix);
  
            //!< process noise covariance matrix (Q)  
            setIdentity(KF->processNoiseCov, Scalar::all(1e-5));
          
            //!< measurement noise covariance matrix (R)  
            setIdentity(KF->measurementNoiseCov, Scalar::all(1e-1));

            //!< priori error estimate covariance matrix (P'(k)): P'(k)=A*P(k-1)*At + Q)*/  AäèF: transitionMatrix  
            setIdentity(KF->errorCovPost, Scalar::all(1));
    
            randn(KF->statePost, Scalar::all(0), Scalar::all(0.0));
        }

        void initOptialFlow() {
            prevTrackPts.reserve(68);
            nextTrackPts.reserve(68);
            for (int i = 0; i < 68; i++) {
                prevTrackPts.push_back(cv::Point2f(0, 0));
                nextTrackPts.push_back(cv::Point2f(0, 0));
            }
        }

        const std::vector<cv::Point2f>& getFaceLandMarks() {
            return prevTrackPts;
        }

        const std::vector<cv::Point2f>& getNeutralTrackPts() {
            return neutralTrackPts;
        }

        const std::vector<cv::Point2f>& getMotionTrackPts() {
            return motionTrackPts;
        }

        bool track(const cv::Mat& img, cv::Mat& frame) {
            if (!startTracking) {
                return false;
            }

/*             cv::Mat downsizedImg; */
/*             cv::resize(img, downsizedImg, cv::Size(), 0.5, 0.5); */

            cv_image<bgr_pixel> cimg(img);
/*             cv_image<bgr_pixel> cimg2(img); */

            if (detectFace == 0) {
                // Detect faces, load the vertexes as vector 
                std::vector<dlib::rectangle> faces = detector(cimg);
                if (faces.size() == 1) {
                    detectedFace.set_left(faces[0].left());
                    detectedFace.set_top(faces[0].top());
                    detectedFace.set_right(faces[0].right());
                    detectedFace.set_bottom(faces[0].bottom());
                }
            }
            detectFace++;
/*             int frequncey = std::abs(direction) > 200 ? 1 : 2; */
            // Set face detection frequency
            if (detectFace == 1) {
                detectFace = 0;
            }

            if (detectLandMark) {
/*                 this->d = poseModel(cimg, faces[0]); */
                dlib::rectangle rect(detectedFace.left(), detectedFace.top(), detectedFace.right(), detectedFace.bottom());
                this->d = poseModel(cimg, rect);
/*                 cv::rectangle(frame, cv::Point2f(rect.left(), rect.top()), cv::Point2f(rect.right(), rect.bottom()), cv::Scalar(0, 255, 0)); */
/*                 cv::rectangle(frame, cv::Point2f(faces[0].left(), faces[0].top()), cv::Point2f(faces[0].right(), faces[0].bottom()), cv::Scalar(0, 255, 0)); */
            }


            // Find the pose of face.
            //            const full_object_detection& d = poseModel(cimg, faces[0]);


            // This function is to combine the optical flow + kalman filter + dlib to deteck and track the facial landmark
            if (flag == -1) {// First frame
                cvtColor(frame, prevgray, CV_BGR2GRAY);
                for (int i = 0; i < d.num_parts(); i++) {
                    prevTrackPts[i].x = d.part(i).x();
                    prevTrackPts[i].y = d.part(i).y();
                }
                flag = 1; 
            }

            // Update Kalman Filter Points
            for (int i = 0; i < d.num_parts(); i++) {
                kalmanPoints[i].x = d.part(i).x();
                kalmanPoints[i].y = d.part(i).y();
            }

            // Kalman Prediction
            Mat prediction = KF->predict();
            for (int i = 0; i < 68; i++) {
                predictPoints[i].x = prediction.at<float>(i * 2);
                predictPoints[i].y = prediction.at<float>(i * 2 + 1);
            }

            cvtColor(frame, gray, CV_BGR2GRAY);
            if (prevgray.data) {
                std::vector<uchar> status;
                std::vector<float> err;

                // Calculate Optical flow
                calcOpticalFlowPyrLK(prevgray, gray, prevTrackPts, nextTrackPts, status, err);


                this->direction = calcHorizontalGlobalDirection(prevTrackPts, nextTrackPts);

                // if the face is moving so fast, use dlib to detect the face
                double diff = calDistanceDiff(prevTrackPts, nextTrackPts);
                this->variance = diff;
                //                std::cout << "variance: " << diff << std::endl;
                if (diff > 1.0) {
                    //                        std::cout<< "DLIB" << std::endl;
                    for (int i = 0; i < d.num_parts(); i++) {
                        cv::circle(frame, cv::Point2f(d.part(i).x(), d.part(i).y()), 2, cv::Scalar(0, 0, 255), -1);
                        nextTrackPts[i].x = d.part(i).x();
                        nextTrackPts[i].y = d.part(i).y();
                    }
                } else if (diff <= 1.0 && diff > 0.005) {//0.005) {//0.005){
                    // In this case, use Optical Flow
                    //                        std::cout<< "Optical Flow" << std::endl;
                    for (int i = 0; i < nextTrackPts.size(); i++) {
                        cv::circle(frame, nextTrackPts[i], 2, cv::Scalar(255, 0, 0), -1);
                    }
                } else {
                    // In this case, use Kalman Filter
                    //                        std::cout<< "Kalman Filter" << std::endl;
                    for (int i = 0; i < predictPoints.size(); i++) {
                        cv::circle(frame, predictPoints[i], 2, cv::Scalar(0, 255, 0), -1);
                        nextTrackPts[i].x = predictPoints[i].x;
                        nextTrackPts[i].y = predictPoints[i].y;
                    }
                }
            }

            // Swap current tracked points and previous tracked points
            std::swap(prevTrackPts, nextTrackPts);
            std::swap(prevgray, gray);

            // Update Measurement
            for (int i = 0; i < 136; i++) {
                if (i % 2 == 0) {
                    measurement->at<float>(i) = (float)kalmanPoints[i / 2].x;
                } else {
                    measurement->at<float>(i) = (float)kalmanPoints[(i - 1) / 2].y;
                }
            }

            // Update the Measurement Matrix
            (*measurement) += KF->measurementMatrix * (*state);
            KF->correct(*measurement);


            if (resetNeutral) {
                resetPoints();
                resetNeutral = false;
            }

            // Update Motion
            for (int i = 0; i < 68; ++i) {
/*                 motionTrackPts[i].x = (prevTrackPts[i].x - predictPoints[i].x) / 320.0;//160.0; */
/*                 motionTrackPts[i].y = (prevTrackPts[i].y - predictPoints[i].y) / 180.0;//120.0; */
                motionTrackPts[i].x = (prevTrackPts[i].x - neutralTrackPts[i].x) / 320.0;//160.0;
                motionTrackPts[i].y = (prevTrackPts[i].y - neutralTrackPts[i].y) / 180.0;//120.0;
                alignedTrackPts[i].x = prevTrackPts[i].x;
                alignedTrackPts[i].y = prevTrackPts[i].y;
            }


            return true;
        }

        // This function is to calculate the variance 
        inline double calDistanceDiff(const std::vector<cv::Point2f>& curPoints, const std::vector<cv::Point2f>& lastPoints) {
            double variance = 0.0;
            double sum = 0.0;
            if (curPoints.size() == lastPoints.size()) {
                std::vector<double> diffs(lastPoints.size());
                for (int i = 0; i < curPoints.size(); i++) {
                    double diff = std::sqrt(std::pow(curPoints[i].x - lastPoints[i].x, 2.0) + std::pow(curPoints[i].y - lastPoints[i].y, 2.0));
                    sum += diff;
                    diffs[i] = diff;
                }
                double mean = sum / diffs.size();
                for (int i = 0; i < curPoints.size(); i++) {
                    variance += std::pow(diffs[i] - mean, 2);
                }
                return variance / diffs.size();
            }
            return variance;
        }

        inline double calcHorizontalGlobalDirection(const std::vector<cv::Point2f>& curPoints, const std::vector<cv::Point2f>& lastPoints) {
            if (curPoints.size() != lastPoints.size()) {
                return 0.0;
            }
            double direction = 0.0;
            for (int i = 0; i < curPoints.size(); i++) {
                direction += curPoints[i].x - lastPoints[i].x;
            }
            return direction;
        }

        double getDirection() {
            return direction;
        }

        const std::vector<cv::Point2f>& getAlignedLandMarks(const std::vector<cv::Point2f>& pts) {
            assert(pts.size() == 68);
            assert(alignedTrackPts.size() == 68);
            for (int i = 0; i < 68; ++i) {
                alignedTrackPts[i].y = pts[i].y;
            }
            return alignedTrackPts;
        }

    private:
        int flag;

        bool startTracking;
        bool resetNeutral;
        int detectFace;
        bool detectLandMark;

        std::vector<cv::Point2f> kalmanPoints;
        std::vector<cv::Point2f> predictPoints;
        frontal_face_detector detector;
        shape_predictor poseModel;

        dlib::full_object_detection d;
        dlib::rectangle detectedFace;
        

        //Optical flow
        cv::Mat prevgray, gray;
        std::vector<cv::Point2f> prevTrackPts;
        std::vector<cv::Point2f> nextTrackPts;

        std::vector<cv::Point2f> neutralTrackPts;
        std::vector<cv::Point2f> motionTrackPts;

        //Kalman filter
        cv::KalmanFilter* KF;
        cv::Mat* state;
        cv::Mat* processNoise;
        cv::Mat* measurement;

        // Kalman Filter Setup (68 Points Test)
        static const int stateNum = 272;
        static const int measureNum = 136;

        double variance = 0.0;
        double direction = 0.0;


        std::vector<cv::Point2f> alignedTrackPts;
    };
}

#endif

