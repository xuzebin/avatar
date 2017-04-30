#ifndef FACETRACKER_H
#define FACETRACKER_H

#include <string>
#include <opencv2/core/core.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include "TrackingData.h"

namespace avt {

    struct BoundingBox {
        int minx;
        int miny;
        int maxx;
        int maxy;

        BoundingBox() {
            minx = 0;
            miny = 0;
            maxx = 0;
            maxy = 0;
        }

        BoundingBox(int minx, int miny, int maxx, int maxy) {
            set(minx, miny, maxx, maxy);
        }

        void set(const BoundingBox& box) {
            set(box.minx, box.miny, box.maxx, box.maxy);
        }

        void set(int minx, int miny, int maxx, int maxy) {
            this->minx = minx;
            this->miny = miny;
            this->maxx = maxx;
            this->maxy = maxy;
        }
            
    };

    class FaceTracker {
    public:
        FaceTracker(const cv::Size& img_size, const std::string& pose_model_file);
        virtual ~FaceTracker();

        const TrackingData& get_face_landmarks();
        const TrackingData& get_neutral_landmarks();
        const TrackingDataf& get_movement();
        const BoundingBox& getFaceBoundingBox() { return faceBbox; }

        const dlib::full_object_detection& get_all_landmarks() { return d;}

        // Track the frame from camera or video.
        bool track(const cv::Mat& frame);
        bool trackInBox(const cv::Mat& frame, const BoundingBox& box);
        
        // Call this to reset a neutral face position so track() will return true.
        void reset();

        void setPose(float x, float y, float z);

        // Refine current landmark using another view's landmark
        void refineLandmark(const TrackingData& t) {
            for (int i = 0; i < t.landmarks.size(); ++i) {
                current.markers[i].y = t.markers[i].y;
                current.landmarks[i].y = t.landmarks[i].y;
            }
        }

    
    private:
        void update(const dlib::full_object_detection& d);
        void reset(const dlib::full_object_detection& d);

        dlib::frontal_face_detector detector;
        dlib::shape_predictor pose_model;

        dlib::full_object_detection d;

        bool start_tracking;
        bool reset_neutral;

        TrackingData current;
        TrackingData neutral;

        TrackingDataf move;

        cv::Size img_size;

        BoundingBox faceBbox;
    };
}
#endif /* FACETRACKER_H */
