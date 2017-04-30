#ifndef TRACKINGDATA_H
#define TRACKINGDATA_H

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

namespace avt {

    const int FACE_LANDMARK_COUNT = 68;

    enum Marker {
/*         FOREHEAD = 0, */
        LEFTOUTERBROW,
        LEFTINNERBROW,
        RIGHTINNERBROW,
        RIGHTOUTERBROW,
/*         LEFTCHEEK, */
/*         RIGHTCHEEK, */
        LEFTNOSE,
/*         NOSE, */
        RIGHTNOSE,
        UPPERLIP,
        LEFTMOUTH,
        RIGHTMOUTH,
        LOWERLIP,
        MARKER_COUNT
    };
    
    template <typename T>
    struct Point2d {
        T x;
        T y;

        Point2d() {
            this->x = 0;
            this->y = 0;
        }
        
        Point2d(T x, T y) {
            this->x = x;
            this->y = y;
        }

        Point2d(const Point2d<int>& p) {
            this->x = p.x;
            this->y = p.y;
        }

        Point2d operator - (const Point2d& p) {
            return Point2d(x - p.x, y - p.y);
        }

        Point2d<T>& operator = (const Point2d<T>& p) {
            x = p.x;
            y = p.y;
            return *this;
        }
    };

/*     template <typename T> */
/*     struct Point3d : Point2d<T> { */
/*         T z; */
/*         Point3d& operator - (const Point3d& p) { */
/*             Point3d res; */
/*             res.x = x - p.x; */
/*             res.y = y - p.y; */
/*             res.z = z - p.z; */
/*             return res; */
/*         } */
/*     }; */

    template <typename T>
    struct TrackData {

        TrackData() {
            landmarks.reserve(FACE_LANDMARK_COUNT);
            for (int i = 0; i < FACE_LANDMARK_COUNT; ++i) {
                landmarks.push_back(cv::Point2i(0,0));
            }
        }

        /*     Point2f markers[MARKER_COUNT]; */
/*         cv::Point markers[MARKER_COUNT]; */
        /* Point2d<T> markers[MARKER_COUNT]; */

        Point2d<T> markers[FACE_LANDMARK_COUNT];//moving data

        std::vector<cv::Point2f> landmarks;//pixel coordinate in image

        cv::Point3f rotation;

        friend std::ostream& operator << (std::ostream& os, const TrackData<T>& data) {
            for (int i = 0; i < FACE_LANDMARK_COUNT; ++i) {
                os << "landmarks motion:" << std::endl;
                os << i << ": (" << data.markers[i].x << ", " << data.markers[i].y << ")" << std::endl;
            }
            return os;
        }
    };

    typedef TrackData<int> TrackingData;
    typedef TrackData<float> TrackingDataf;
    typedef Point2d<int> Point2i;
    typedef Point2d<float> Point2f;

}
#endif 	/* TRACKINGDATA_H */
