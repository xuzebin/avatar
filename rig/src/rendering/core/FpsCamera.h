#ifndef FpsCamera_h
#define FpsCamera_h

#include "matrix4.h"
#include "cvec.h"

/** 
 * a first person camera
 * currently we here restrict rotation direction to only yaw and pitch
 */
struct FpsCamera {
public:
    FpsCamera() : yaw(0.0), 
                  pitch(0.0), 
                  position(30.0, 10.0, 30.0) 
    {
        updateView();
        projectionMatrix = Matrix4::makeProjection(fov, 1.0, Z_NEAR, Z_FAR);
    }
    
    const Matrix4& getViewMatrix() const       { return viewMatrix; }
    const Matrix4& getProjectionMatrix() const { return projectionMatrix; }
    const Cvec3& getPosition() const           { return position; }

    void translate(Cvec3 translation) { position += translation; }
    void updateView() {
        viewMatrix = Matrix4::makeTranslation(position) * Matrix4::makeXRotation(pitch) * Matrix4::makeYRotation(yaw);
    }
    
    void control(const unsigned char key) {
        float dx = 0;
        float dz = 0;
        
        switch (key) {
            case 'd':
            case 'D':
                dx = 2;
                break;
            case 's':
            case 'S':
                dz = -2;
                break;
            case 'a':
            case 'A':
                dx = -2;
                break;
            case 'w':
            case 'W':
                dz = 2;
                break;
            case 'f':
            case 'F':
                dz = 20;
            default:
                break;
        }
        
        
        Cvec3 forward = Cvec3(viewMatrix(0, 2), viewMatrix(1, 2), viewMatrix(2, 2));
        Cvec3 strafe = Cvec3(viewMatrix(0, 0), viewMatrix(1, 0), viewMatrix(2, 0));

        position += (forward * (-dz) + strafe * dx) * MovingSpeed;
//        position += (forward * (-dz)) * MovingSpeed;
        this->pitch += dz;
        this->yaw -= dx;

        updateView();
    }
    

public:
    void mouseMotion(int x, int y) {
        int dx = x - lastX;
        int dy = y - lastY;
        
        this->yaw += dx * ROTATE_SPEED;
        this->pitch += dy * ROTATE_SPEED;
        
        lastX = x;
        lastY = y;
        updateView();
    }
    void mousePressed(int state, int x, int y) {
        switch(state) {
            case GLUT_DOWN:
                lastX = x;
                lastY = y;
                pressed = true;
                break;
            case GLUT_UP:
                pressed = false;
                break;
            default:
                break;
        }
    }

private:

    const double MovingSpeed = 8.0;

    Quat rotation;
    
    double yaw;

    double pitch;
    
    Cvec3 position;
    
    Matrix4 viewMatrix;

    Matrix4 projectionMatrix;
    
    double fov = 45.0;

    const float ROTATE_SPEED = 0.2f;    

    int lastX;
    int lastY;
    bool pressed = false;
};

#endif /* FpsCamera_h */
