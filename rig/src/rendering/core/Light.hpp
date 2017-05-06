#ifndef Light_hpp
#define Light_hpp

#include "../base/cvec.h"
#include "../base/matrix4.h"
class Light {
public:
    Light();
    ~Light();
    
    void setPosition(double x, double y, double z);
    void setPositionX(double x);
    void setPositionY(double y);
    void setPositionZ(double z);

    void moveX(double x);
    void moveY(double y);
    void moveZ(double z);

    void updatePositionInEyeSpace(const Matrix4& viewMatrix);
    
    const Cvec3& getPosition() const { return position; }
    const Cvec3& getPositionInEyeSpace(const Matrix4& viewMatrix);
    
public:
    Cvec3f lightColor;
    Cvec3f specularLightColor;

private:
    bool isPositionDirty;
    Cvec3 position;
    Cvec3 positionInEyeSpace;
    //TODO add attenuation coefficient in c code.
};

#endif /* Light_hpp */
