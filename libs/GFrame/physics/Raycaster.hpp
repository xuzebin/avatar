#ifndef Raycaster_hpp
#define Raycaster_hpp

#include "../base/cvec.h"
#include "../base/matrix4.h"
class Camera;

/**
 * Currently only support ray sphere intersection.
 */
class Raycaster {

public:
    Raycaster();
    ~Raycaster();

    bool isPicked(int x, int y, int screenWidth, int screenHeight, const Matrix4& projectionMatrix, const Matrix4& viewMatrix, const Cvec3& cameraPosition, const Cvec3& center, double radius, double zNear);

private:
    //get ray direction
    const Cvec3& castRay(int x, int y, int windowWidth, int windowHeight, const Matrix4& projectionMatrix, const Matrix4& viewMatrix);
    bool raySphereIntersect(const Cvec3& center, double radius, const Cvec3& cameraPosition, const Cvec3& rayDirection, double zNear);
};


#endif /* Raycaster_hpp */
