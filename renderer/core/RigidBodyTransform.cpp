#include "RigidBodyTransform.hpp"

RigidBodyTransform::RigidBodyTransform() : isMatrixDirty(true) 
{
}

RigidBodyTransform::~RigidBodyTransform()
{
}

void RigidBodyTransform::setRotation(const Quat& rotation) {
    this->rotation = rotation;
    isMatrixDirty = true;
}

void RigidBodyTransform::setPosition(const Cvec3& position) {
    this->position = position;
    isMatrixDirty = true;
}
    
void RigidBodyTransform::translate(const Cvec3& translation) {
    this->position += translation;
    isMatrixDirty = true;
}

void RigidBodyTransform::rotate(const Quat& rotation) {
    this->rotation = this->rotation * rotation;
    isMatrixDirty = true;
}

const Matrix4& RigidBodyTransform::getRigidBodyMatrix() {
    if (isMatrixDirty) {
        calculateMatrix();
    }
    return rigidBodyMatrix;
}

RigidBodyTransform& RigidBodyTransform::operator = (const RigidBodyTransform& t) {
    rotation = t.rotation;
    position = t.position;
    isMatrixDirty = t.isMatrixDirty;
    rigidBodyMatrix = t.rigidBodyMatrix;
    return *this;
}

void RigidBodyTransform::calculateMatrix() {
    rigidBodyMatrix = Matrix4::makeTranslation(position) * quatToMatrix(rotation);
    isMatrixDirty = false;
}
