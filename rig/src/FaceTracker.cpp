#include "FaceTracker.h"
#include "TrackingData.h"

namespace avt {

    FaceTracker::FaceTracker(const cv::Size& img_size, const std::string& pose_model_file) {
        this->img_size = img_size;
        this->start_tracking = false;
        this->reset_neutral = false;
        this->detector = dlib::get_frontal_face_detector();
        dlib::deserialize(pose_model_file) >> this->pose_model;
    }

    FaceTracker::~FaceTracker() {

    }

    const TrackingData& FaceTracker::get_face_landmarks() {
        return current;
    }

    const TrackingData& FaceTracker::get_neutral_landmarks() {
        return neutral;
    }

    const TrackingDataf& FaceTracker::get_movement() {
        return move;
    }

    bool FaceTracker::track(const cv::Mat& frame) {
        if (!start_tracking) {
            return false;
        }
        
        dlib::cv_image<dlib::bgr_pixel> cimg(frame);

        // Detect faces
        std::vector<dlib::rectangle> faces = this->detector(cimg);

        // Consider one face only
        if (faces.size() != 1) {
            return false;
        }
        this->faceBbox.set(faces[0].left(), faces[0].top(), faces[0].right(), faces[0].bottom());
        // Detect face landmarks
        this->d = this->pose_model(cimg, faces[0]);
        
        if (reset_neutral) {
            reset(d);
            reset_neutral = false;
        } else {
            update(d);
        }

        return true;
    }

    bool FaceTracker::trackInBox(const cv::Mat& frame, const BoundingBox& box) {
        if (!start_tracking) {
            return false;
        }
        
        dlib::cv_image<dlib::bgr_pixel> cimg(frame);

        this->faceBbox.set(box);

        // Detect face landmarks
        this->d = this->pose_model(cimg, dlib::rectangle(box.minx, box.miny, box.maxx, box.maxy));
        
        if (reset_neutral) {
            reset(d);
            reset_neutral = false;
        } else {
            update(d);
        }

        return true;
    }

    void FaceTracker::reset() {
        this->reset_neutral = true;
        this->start_tracking = true;
    }

    void FaceTracker::reset(const dlib::full_object_detection& d) {
        for (int i = 0; i < d.num_parts(); i++) {
            neutral.markers[i].x = d.part(i).x();
            neutral.markers[i].y = d.part(i).y();
            current.markers[i] = neutral.markers[i];

            current.landmarks[i].x = d.part(i).x();
            current.landmarks[i].y = d.part(i).y();
        }
        setPose(0, 0, 0);
        this->start_tracking = true;
    }

    void FaceTracker::update(const dlib::full_object_detection& d) {
        for (int i = 0; i < d.num_parts(); i++) {
            current.markers[i].x = d.part(i).x();
            current.markers[i].y = d.part(i).y();

            move.markers[i] = Point2f(current.markers[i] - neutral.markers[i]);
            move.markers[i].x /= float(img_size.width / 2);
            move.markers[i].y /= float(img_size.height / 2);

            // face landmark image coordinates
            current.landmarks[i].x = d.part(i).x();
            current.landmarks[i].y = d.part(i).y();
        }
    }

    void FaceTracker::setPose(float x, float y, float z) {
        move.rotation.x = x;
        move.rotation.y = y;
        move.rotation.z = z;
    }
}

