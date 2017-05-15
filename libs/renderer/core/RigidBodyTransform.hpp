#ifndef RigidBodyTransform_hpp
#define RigidBodyTransform_hpp

#include "../base/quat.h"
#include "../base/cvec.h"
#include "../base/matrix4.h"

/**
 * Rigid Body Transform (RBT) excluding scaling.
 * Preserve dot products between vectors, handedness of basis and distance between points.
 */
class RigidBodyTransform {
public:
    
    RigidBodyTransform();
    ~RigidBodyTransform();

    void setRotation(const Quat& rotation);
    void setPosition(const Cvec3& position);
    void translate(const Cvec3& translation);
    void rotate(const Quat& rotation);
    
    const Cvec3& getPosition() const { return position; }
    const Quat& getRotation() const  { return rotation; }
    const Matrix4& getRigidBodyMatrix();

    RigidBodyTransform& operator = (const RigidBodyTransform& t);

protected:
    void calculateMatrix();

    Quat rotation;

    Cvec3 position;
    
    bool isMatrixDirty;
    
    Matrix4 rigidBodyMatrix;
};

#endif /* RigidBodyTransform_hpp */
