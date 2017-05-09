#ifndef Trackball_cpp
#define Trackball_cpp

#include "Trackball.hpp"

Trackball::Trackball() : radius(0.0f),
                         screenWidth(0), 
                         screenHeight(0),
                         speed(4.0f),
                         preX(0),
                         preY(0)
{
}

Trackball::Trackball(int screenWidth, int screenHeight) {
    this->radius = (screenWidth < screenHeight) ? screenWidth / 2 : screenHeight / 2;
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->speed = 4.0f;
    this->preX = 0;
    this->preY = 0;
}

Trackball::Trackball(float radius, int screenWidth, int screenHeight) : radius(radius),
                                                                        screenWidth(screenWidth),
                                                                        screenHeight(screenHeight),
                                                                        speed(4.0f),
                                                                        preX(0),
                                                                        preY(0)
{
}

Trackball::~Trackball() {
}

void Trackball::setScreenSize(int screenWidth, int screenHeight) {
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
}

void Trackball::setRadiusAndScreenSize(float radius, int screenWidth, int screenHeight) {
    setRadius(radius);
    setScreenSize(screenWidth, screenHeight);
}

void Trackball::updateScreenSize(int screenWidth, int screenHeight) {
    this->radius = (screenWidth < screenHeight) ? screenWidth / 2 : screenHeight / 2;
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
}

void Trackball::setInitRotation(const Quat& rotation) { 
    this->preRotation = rotation;
    this->curRotation = rotation;
}

Quat Trackball::getQuatBetween(const Cvec3& v1, const Cvec3& v2) {
    if (same(v1, v2, CS175_EPS)) {
        return Quat::makeRotationAroundAxis(v1, 0);
    } else if (same(v1, -v2, CS175_EPS)) {
        Cvec3 v;
        if (v1[0] > -CS175_EPS && v1[0] < CS175_EPS) {
            v = Cvec3(1, 0, 0);
        } else if (v1[1] > -CS175_EPS && v1[1] < CS175_EPS) {
            v = Cvec3(0, 1, 0);
        } else {
            v = Cvec3(0, 0, 1);
        }
        return Quat::makeRotationAroundAxis(v, 180);
    } else {
        Cvec3 nv1 = v1;
        Cvec3 nv2 = v2;
        nv1.normalize();
        nv2.normalize();
        Cvec3 v = cross(nv1, nv2);
        float radian = acosf(dot(nv1, nv2)); //rotation angle (radian)
        
        return Quat::makeRotationAroundAxis(v, speed * radian * 180 / CS175_PI);
    }
}

/**
 * To calculate the length of the arc path between previous and current points on the sphere,
 * we use the distance between screen center and mouse position to approximate.
 * x = r * sin(a) * cos(b)
 * y = r * sin(a) * sin(b)
 * z = r * cos(a)
 * (a is the angle on x-z plane, b is the angle on x-y plane)
 */
Cvec3 Trackball::getVector(int x, int y) const {
    /** convert coordiantes from (0,0)~(w,h) to (-w/2, -h/2)~(w/2, h/2) **/
    float nx = x - screenWidth / 2;
    float ny = screenHeight / 2 - y;

    float distance = sqrtf(nx * nx + ny * ny); // distance between screen center and mouse position
    float xz = distance / radius;              // angle on x-z plane (radian)
    float xy = atan2f(ny, nx);                 // angle on x-y plane (radian)
    float tmp  = radius * sinf(xz);
    
    Cvec3 vector;
    vector[0] = tmp * cosf(xy);
    vector[1] = tmp * sinf(xy);
    vector[2] = radius * cosf(xz);

    return vector;
}

Cvec3 Trackball::getUnitVector(int x, int y) const {
    Cvec3 vector = getVector(x, y);
    vector.normalize();
    return vector;
}

void Trackball::recordMousePosition(int x, int y) {
    preX = x;
    preY = y;
}

void Trackball::record(int x, int y) {
    recordMousePosition(x, y);
    this->preRotation = this->curRotation;
}

const Quat& Trackball::getRotation(int x, int y) {
    Cvec3 v1 = getUnitVector(preX, preY);
    Cvec3 v2 = getUnitVector(x, y);
    curRotation = getQuatBetween(v1, v2) * preRotation;
    return curRotation;
}


#endif /* Trackball_cpp */
