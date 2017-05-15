#ifndef POSEDATA_H
#define POSEDATA_H
#include "base/matrix4.h"
#include "BufferData.h"

namespace avt {
    class PoseData : public BufferData {
    public:
        PoseData();
        ~PoseData();
        void decodeBuffer(const char* buffer);
        const Matrix4& getPose() const;

        void setCallback(void (*func)(const Matrix4&));
    private:
        Matrix4 extrinsicMatrix;
        void (*updateFunc)(const Matrix4&);
    };
}

#endif /* POSEDATA_H */
