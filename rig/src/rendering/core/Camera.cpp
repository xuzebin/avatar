#include "Camera.hpp"

const Cvec3 Camera::UP_VEC = Cvec3(0, 1, 0);
// const double Camera::Z_NEAR = -0.1;
// const double Camera::Z_FAR = -800;

Camera::Camera() : position(0, 0, 0),
                   fov(45.0),
                   zNear(-0.1),
                   zFar(-800)
{
    updateView(Cvec3(0, 0, 0));
    setProjectionMatrix(fov, 1.0, zNear, zFar);
}

Camera::Camera(const Cvec3& position, const Quat& rotation, double fov) : position(position),
                                                                          rotation(rotation),
                                                                          fov(fov),
                                                                          zNear(-0.1),
                                                                          zFar(-800)
{
    setViewMatrix(position, rotation);
    setProjectionMatrix(fov, 1.0, zNear, zFar);
}

Camera::~Camera()
{
}

void Camera::rotate(const Quat& rotation) {
    this->rotation = this->rotation * rotation;
    updateViewMatrix();
}

void Camera::setRotation(const Quat& rotation) {
    this->rotation = rotation;
    updateViewMatrix();
}

void Camera::setPosition(const Cvec3& position) {
    this->position = position;
    updateViewMatrix();
}

void Camera::setViewMatrix(const Cvec3& position, const Quat& rotation) {
    viewMatrix = Matrix4::makeTranslation(position) * quatToMatrix(rotation);
}

void Camera::updateViewMatrix() {
    setViewMatrix(position, rotation);
}

void Camera::setProjectionMatrix(double fov,
                                 double aspectRatio,
                                 double zNear,
                                 double zFar) {
    projectionMatrix = Matrix4::makeProjection(fov, aspectRatio, zNear, zFar);
}

void Camera::setFov(double fov) {
    this->fov = fov;
    setProjectionMatrix(fov, 1.0, zNear, zFar);
}

void Camera::setZNear(double zNear) {
    this->zNear = zNear;
    setProjectionMatrix(fov, 1.0, zNear, zFar);
}

void Camera::setZFar(double zFar) {
    this->zFar = zFar;
    setProjectionMatrix(fov, 1.0, zNear, zFar);
}
