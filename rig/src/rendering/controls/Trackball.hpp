#ifndef Trackball_hpp
#define Trackball_hpp

#include "../base/cvec.h"
#include "../base/quat.h"

/**
 * Usage: 
 * 1. setInitRotation(Quat): set the object's initial rotation.
 * 1. record(int, int): record the firstly pressed mouse position & object's rotation in the mouse callback.
 * 2. getRotation(int, int): get current rotation in the motion callback function.
 * 3. updateScreenSize(int, int): update screen size in the reshape callback.
 * Optional functions:
 * 4. setSpeed(float): set the rotation speed (default: 4.0f).
 * 5. setRadius(float): the rotation circle (default: half of the smaller side of the screen).
 * 
 * A brief sample:
 * Trackball trackball(width, height);
 * void init() {
 *     //GL initialization
 *     //...
 *     //Initialize an Entity object..
 *     auto entity = ....
 *
 *     trackball.setInitRotation(entity->getRotation());
 * }
 * void reshape(int w, int h) {
 *     //update screensize
 *     //...
 *     trackball.updateScreenSize(screenWidth, screenHeight);
 * }
 * bool mouseLeftDown = false;
 * void mouse(int button, int state, int x, int y) {
 *     if (buton == GLUT_LEFT_BUTTON) {
 *         if (state == GLUT_DOWN) {
 *             trackball.record(x, y);
 *             mouseLeftDown = true;
 *         } else if (state == GLUT_UP) {
 *             mouseLeftDown = false;
 *         }
 *     }
 * }
 * void moition(int x, int y) {
 *    if (mouseLeftDown) {
 *        Quat rotation = trackball.getRotation(x, y);
 *        //rotate the entity by setting the rotation and we are all done!
 *        auto entity = Scene::getEntity("modelName");
 *        entity->setRotation(rotation);
 *    }
 * }
 * 
 *
 */
class Trackball {

public:
    Trackball();
    Trackball(int screenWidth, int screenHeight);
    Trackball(float radius, int screenWidth, int screenHeight);
    ~Trackball();

    void setRadius(float radius) { this->radius = radius; }
    void setScreenSize(int screenWidth, int screenHeight);
    void setRadiusAndScreenSize(float radius, int screenWidth, int screenHeight);
    void updateScreenSize(int screenWidth, int screenHeight);
    void setSpeed(float speed)   { this->speed = speed; }

    /** Set initial rotation of the target **/
    void setInitRotation(const Quat& rotation);
    
    float getRadius() const      { return radius; }
    int getScreenWidth() const   { return screenWidth; }
    int getScreenHeight() const  { return screenHeight; }
    float getSpeed() const       { return speed; }

    Quat getQuatBetween(const Cvec3& v1, const Cvec3& v2);
    
    Cvec3 getVector(int x, int y) const;
    Cvec3 getUnitVector(int x, int y) const;
    
    /** record the mouse position and rotation of the previous state **/
    void record(int x, int y);

    /** get current rotation based on previous and current mouse position **/
    const Quat& getRotation(int x, int y);

    int getPreX() { return preX;}
    int getPreY() { return preY;}
    
private:
    void recordMousePosition(int x, int y);

    inline bool same(const Cvec3& v1, const Cvec3& v2, float epsilon) {
        return fabs(v1[0] - v2[0]) < epsilon && fabs(v1[1] - v2[1]) < epsilon && fabs(v1[2] - v2[2]) < epsilon;
    }

    float radius;
    int screenWidth;
    int screenHeight;
    float speed;

    int preX, preY;
    Quat curRotation, preRotation;
};

#endif /* Trackball_hpp */
