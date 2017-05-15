#include "PoseData.h"
#include <sstream>
namespace avt {
    PoseData::PoseData()
    {
    }
    
    PoseData::~PoseData()
    {
    }

    void PoseData::decodeBuffer(const char* buffer)
    {
        std::string str(buffer);
        std::stringstream ss(str);
        int frameCount;
        ss >> frameCount;
        std::cout << "Frame: " << frameCount << std::endl;

        //TODO add lock for shared variable modelMatrix
        for (int i = 0; i < 4; ++i) {
            ss >> extrinsicMatrix(i, 0);
            ss >> extrinsicMatrix(i, 1);
            ss >> extrinsicMatrix(i, 2);
            ss >> extrinsicMatrix(i, 3);
        }

        // Fix x, y, and scale z
        extrinsicMatrix(0, 3) = 0;
        extrinsicMatrix(1, 3) = 0;
        extrinsicMatrix(2, 3) /= -13;

        updateFunc(extrinsicMatrix);
    }
    
    const Matrix4& PoseData::getPose() const
    {
        return extrinsicMatrix;
    }

    void PoseData::setCallback(void (*func)(const Matrix4& m))
    {
        updateFunc = func;
    }
}

