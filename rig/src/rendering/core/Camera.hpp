#ifndef Camera_hpp
#define Camera_hpp

#include "../base/quat.h"
#include "../base/cvec.h"

/**
 * The camera object holds the view matrix as well as projection matrix.
 */
class Camera {

public:
    static const Cvec3 UP_VEC;

    Camera();
    Camera(const Cvec3& position, const Quat& rotation, double fov = 45.0);
    ~Camera();

    const Matrix4& getViewMatrix() const       { return viewMatrix; }
    const Matrix4& getProjectionMatrix() const { return projectionMatrix; }
    const Cvec3& getPosition() const           { return position; }
    const Quat& getRotation() const            { return rotation; }
    double getFov() const                      { return fov; }
    double getZNear() const                    { return zNear; }
    double getZFar() const                     { return zFar; }

    void updateView(const Cvec3& target)       { viewMatrix = setLookat(position, target, UP_VEC); }
    void rotate(const Quat& rotation);
    void setRotation(const Quat& rotation);
    void setPosition(const Cvec3& position);
    void setFov(double fov);
    void setZNear(double zNear);
    void setZFar(double zFar);
    void setViewMatrix(const Cvec3& position, const Quat& rotation);
    void setViewMatrix(const Matrix4& viewMatrix) { this->viewMatrix = viewMatrix; }

private:

    void updateViewMatrix();
    void setProjectionMatrix(double fov,
                             double aspectRatio,
                             double zNear,
                             double zFar);

    Cvec3 position;
    Quat rotation;
    
    Matrix4 viewMatrix;
    Matrix4 projectionMatrix;

    double fov;
    double zNear;
    double zFar;
};

#endif /* Camera_hpp */
