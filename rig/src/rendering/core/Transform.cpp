#include "Transform.hpp"

Transform::Transform() : RigidBodyTransform(),
                         scale(1.0, 1.0, 1.0), 
                         pivot(0, 0, 0)
{
}

Transform::~Transform()
{
}

void Transform::setScale(const Cvec3& scale) {
    this->scale = scale;
    isMatrixDirty = true;
}

void Transform::setModelMatrix(const Matrix4& m) {
    for (int i = 0; i < 16; i++) {
        modelMatrix[i] = m[i];
    }
}

void Transform::setPivot(float x, float y, float z) {
    pivot[0] = x;
    pivot[1] = y;
    pivot[2] = z;
    isMatrixDirty = true;
}

const Matrix4& Transform::getModelMatrix() {
    if (isMatrixDirty) {
        calculateMatrix(); //Calculate matrix only when Transform is changed.
    }
    return modelMatrix;
}

Transform& Transform::operator = (const Transform& t) {
    rotation = t.rotation;
    position = t.position;
    scale = t.scale;
    isMatrixDirty = t.isMatrixDirty;
    rigidBodyMatrix = t.rigidBodyMatrix;
    modelMatrix = t.modelMatrix;
    return *this;
}

void Transform::calculateMatrix() {
    rigidBodyMatrix = Matrix4::makeTranslation(position) * Matrix4::makeTranslation(pivot) * quatToMatrix(rotation) * Matrix4::makeTranslation(-pivot);
    modelMatrix = rigidBodyMatrix * Matrix4::makeScale(scale);
    isMatrixDirty = false;
}
