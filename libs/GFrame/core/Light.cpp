#include "Light.hpp"

Light::Light() : isPositionDirty(true), 
                 lightColor(Cvec3f(1, 1, 1)),
                 specularLightColor(Cvec3f(1, 1, 1)) 
{
}

Light::~Light()
{
}

void Light::setPosition(double x, double y, double z) {
    position[0] = x;
    position[1] = y;
    position[2] = z;
    isPositionDirty = true;
}

void Light::setPositionX(double x) {
    position[0] = x;
    isPositionDirty = true;
}

void Light::setPositionY(double y) {
    position[1] = y;
    isPositionDirty = true;
}

void Light::setPositionZ(double z) {
    position[2] = z;
    isPositionDirty = true;
}

void Light::moveX(double x) { setPositionX(position[0] + x); }
void Light::moveY(double y) { setPositionY(position[1] + y); }
void Light::moveZ(double z) { setPositionZ(position[2] + z); }

void Light::updatePositionInEyeSpace(const Matrix4& viewMatrix) {
    Cvec4 p = normalMatrix(viewMatrix) * Cvec4(position[0], position[1], position[2], 1);
    positionInEyeSpace[0] = p[0];
    positionInEyeSpace[1] = p[1];
    positionInEyeSpace[2] = p[2];
    isPositionDirty = false;
}

const Cvec3& Light::getPositionInEyeSpace(const Matrix4& viewMatrix) {
    if (isPositionDirty) {
        updatePositionInEyeSpace(viewMatrix);
    }
    return positionInEyeSpace;
}
