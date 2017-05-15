#include "PointCloudData.h"
#include <sstream>
#define LANDMARK_NUM 68
namespace avt {
    PointCloudData::PointCloudData(int numberOfPoints)
    {
        pointCloud.reserve(numberOfPoints);
        for (int i = 0; i < numberOfPoints; ++i) {
            pointCloud.push_back(Cvec3f(0.0f, 0.0f, 0.0f));
        }
    }

    PointCloudData::~PointCloudData()
    {
    }

    void PointCloudData::decodeBuffer(const char* buffer)
    {
        std::string pcStr(buffer);
        std::stringstream ss(pcStr);
        int frameCount;
        ss >> frameCount;
        std::cout << "Frame: " << frameCount << std::endl;

//         std::vector<Cvec3f> pc(LANDMARK_NUM);

        // Add lock for shared variable.
        for (int i = 0; i < LANDMARK_NUM; ++i) {
            ss >> this->pointCloud[i][0];
            ss >> this->pointCloud[i][1];
            ss >> this->pointCloud[i][2];
        }
        normPointCloud(this->pointCloud);
    }

    const std::vector<Cvec3f>& PointCloudData::getPointCloud() const {
        return pointCloud;
    }

    BoundingBox PointCloudData::calcBoundingBox(const std::vector<Cvec3f>& vs)
    {
        BoundingBox bbox;
        for (int i = 0; i < vs.size(); ++i) {
            for (int j = 0; j < 3; j++) {
                if (vs[i][j] < bbox.min[j]) {
                    bbox.min[j] = vs[i][j];
                }
                if (vs[i][j] > bbox.max[j]) {
                    bbox.max[j] = vs[i][j];
                }
            }
        }
        return bbox;
    }
    
    void PointCloudData::normPointCloud(std::vector<Cvec3f>& pc)
    {
        BoundingBox bbox = calcBoundingBox(pc);
        std::cout << "bbox min: " << bbox.min << std::endl;
        std::cout << "bbox max: " << bbox.max << std::endl;

        /** Calculate the scale and translation to recenter and normalize the geometry **/
        float abs_max = 0;
        for (int i = 0; i < 3; i++) {
            if (abs_max < std::abs(bbox.max[i])) {
                abs_max = std::abs(bbox.max[i]);
            }
            if (abs_max < std::abs(bbox.min[i])) {
                abs_max = std::abs(bbox.min[i]);
            }
        }
        std::cout << "max_abso: " << abs_max << std::endl;

        //Scale and recenter 
        for (int i = 0; i < pc.size(); i++) {
            pc[i] -= (bbox.max + bbox.min) * 0.5;
            pc[i] /= abs_max;
        }
    }
}
