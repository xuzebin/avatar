#include "Raycaster.hpp"
#include "../core/Camera.hpp"

Raycaster::Raycaster()
{
}

Raycaster::~Raycaster()
{
}

bool Raycaster::isPicked(int x, int y, int screenWidth, int screenHeight, const Matrix4& projectionMatrix, const Matrix4& viewMatrix, const Cvec3& cameraPosition, const Cvec3& center, double radius, double zNear) {
    const Cvec3& rayDirection = castRay(x, y, screenWidth, screenHeight, projectionMatrix, viewMatrix);
    return raySphereIntersect(center, radius, cameraPosition, rayDirection, zNear);
}
    
//get ray direction
const Cvec3& Raycaster::castRay(int x, int y, int windowWidth, int windowHeight, const Matrix4& projectionMatrix, const Matrix4& viewMatrix) {
    //normalize
    float nx = (float)x / ((float)windowWidth / 2.0f) - 1.0f;
    float ny = 1.0f - (float)y / ((float)windowHeight / 2.0f);

    //convert to homogeneous clip space
    Cvec4 rayClip = Cvec4(nx, ny, -1.0, 1.0);//point toward -z axis.
        
    //convert to eye space
    Matrix4 proj = projectionMatrix;
    proj[12] = 0.0; proj[13] = 0.0; proj[14] = 0.0; proj[15] = 1.0;
    Matrix4 projectionInverse = inv(proj);
    Cvec4 rayEye = projectionInverse * rayClip;
    rayEye[2] = -1.0;//forward
    rayEye[3] = 0.0;//vector
        
    //convert to world space
    Matrix4 viewInverse = inv(viewMatrix);
    Cvec4 rayWorld = viewInverse * rayEye;
        
    return Cvec3(rayWorld[0], rayWorld[1], rayWorld[2]).normalize();//rayDirection
}

//ray sphere intersection
bool Raycaster::raySphereIntersect(const Cvec3& center, double radius, const Cvec3& cameraPosition, const Cvec3& rayDirection, double zNear) {
    Cvec3 nearPos = cameraPosition;
    nearPos += Cvec3(0, 0, zNear);
    Cvec3 toCenterDirection = center - nearPos;
    double t = dot(toCenterDirection, rayDirection);
        
    Cvec3 intersect = nearPos + rayDirection * t;
    //calculate distance
    double distance = norm(intersect - center);

    return (distance <= radius);
}
