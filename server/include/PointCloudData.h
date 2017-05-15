#ifndef POINTCLOUDDATA_H
#define POINTCLOUDDATA_H
#include "BufferData.h"
#include "base/cvec.h"
#include "geometries/Geometry.hpp"

namespace avt {
    class PointCloudData : public BufferData {
    public:
        PointCloudData(int numberOfPoints);
        ~PointCloudData();
        void decodeBuffer(const char* buffer);
        const std::vector<Cvec3f>& getPointCloud() const;
    private:
        BoundingBox calcBoundingBox(const std::vector<Cvec3f>& vs);
        void normPointCloud(std::vector<Cvec3f>& pc);
        std::vector<Cvec3f> pointCloud;
    };
}

#endif /* POINTCLOUDDATA_H */

