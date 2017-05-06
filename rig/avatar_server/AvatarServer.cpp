#include <GLUT/glut.h>
#include "base/glsupport.h"
#include "base/quat.h"
#include "core/Transform.hpp"
#include "materials/Material.hpp"
#include "core/Scene.hpp"
#include "core/Camera.hpp"
#include "entities/Model.hpp"
#include "core/Light.hpp"
#include "programs/ColorShader.h"
#include "programs/TextureShader.h"
#include "controls/Trackball.hpp"
#include "geometries/Geometry.hpp"
#include "geometries/Plane.h"
#include "materials/Material.hpp"
#include <Socket.h>
#include <stdio.h>
#include <thread>
#include <sstream>
#include <mutex>

#include "BlockingQueue.h"
#define LANDMARK_NUM 68

class Shader;

int screenWidth = 600;
int screenHeight = 600;

Trackball trackball(screenWidth, screenHeight);

// Shared varaible
std::vector<Cvec3f> pointCloud(LANDMARK_NUM);
std::mutex mu;

BlockingQueue bq(20);

void normPointCloud(std::vector<Cvec3f>& pc);
BoundingBox calcBoundingBox(const std::vector<Cvec3f>& vs);

// Trackball 
Matrix4 rotMatrix;
double colRotMat[16];

void message2PointCloud(const char* message) {
    std::string pcStr(message);
    std::stringstream ss(pcStr);
    int frameCount;
    ss >> frameCount;
    std::cout << "Frame: " << frameCount << std::endl;

    std::vector<Cvec3f> pc(LANDMARK_NUM);

    for (int i = 0; i < LANDMARK_NUM; ++i) {
        ss >> pc[i][0];
        ss >> pc[i][1];
        ss >> pc[i][2];
    }
    normPointCloud(pc);
    //    bq.send(pc);
    pointCloud = pc;
    std::cout << "nose: " << pointCloud[33] << std::endl;
//     std::cout << "pointcloud from client: " << std::endl;
//     for (int i = 0; i < 68; ++i) {
//         std::cout << pc[i] << std::endl;        
//     }
//     pointCloud = pc;
}

void messageToModelMatrix(const char* message, Matrix4& modelMatrix) {
    std::string str(message);
    std::stringstream ss(str);
    int frameCount;
    ss >> frameCount;
    std::cout << "Frame: " << frameCount << std::endl;

    for (int i = 0; i < 4; ++i) {
        ss >> modelMatrix(i, 0);
        ss >> modelMatrix(i, 1);
        ss >> modelMatrix(i, 2);
        ss >> modelMatrix(i, 3);
    }
}

void setupServer(int portNumber) {
    Socket socket;
    socket.bind(5055);
    socket.listen(5);

    Socket newSocket = socket.accept();//blocking
    while (1) {
        const char* buffer = newSocket.recv(4000, 0);
        if (buffer == NULL) {
            std::cout << "client disconnect." << std::endl;
            break;
        }
        //        printf("message from client:\n %s\n", buffer);
        
        // Need synchronization
        //       message2PointCloud(buffer);
#define POSE
#ifdef POSE
        Matrix4 modelMatrix;
        messageToModelMatrix(buffer, modelMatrix);
        modelMatrix(0, 3) = 0;
        modelMatrix(1, 3) = 0;
        modelMatrix(2, 3) /= -13;

         std::cout << "ModelMatrix: " << std::endl;
         for (int i = 0; i < 4; ++i) {
             for (int j = 0; j < 4; ++j) {
                 std::cout << modelMatrix(i, j) << " ";
             }
             std::cout << std::endl;
         }
#endif
//         std::string quatStr(buffer);
//         std::stringstream ss(quatStr);
//         Quat quat;
//         ss >> quat[0];
//         ss >> quat[1];
//         ss >> quat[2];
//         ss >> quat[3];

//         std::cout << quat << std::endl;
//         Matrix4 m = quatToMatrix(quat);
//         std::cout << "rotationMatrix: " << std::endl;
//         for (int i = 0; i < 16; i++) {
//             std::cout << m[i];
//             if ((i + 1) % 4 == 0) {
//                 std::cout << std::endl;
//             }
//         }
#ifdef POSE
        // Model control
        auto model = Scene::getEntity("model0");
        if (model != NULL) {
            //            Quat rotation = trackball.getRotation(x, y);
            //           model->setRotation(quat);
            model->setModelMatrix(modelMatrix);
//             modelMatrix(2, 3) *= -1;
//             Matrix4 viewMat = transpose(modelMatrix);
//             auto camera = Scene::getCamera();
//             camera->setViewMatrix(viewMat);
        }
#endif
    }
}


void display(void) {
    Scene::render();
}

void init(void) {
    glClearColor(0.7, 0.7, 0.7, 1.0);
    glClearDepth(0.0);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);
    glReadBuffer(GL_BACK);
    
    std::string vertexShader = "shaders/vertex_shader_simple.glsl";
    std::string fragmentShader = "shaders/fragment_shader_color.glsl";

    auto colorShader = std::make_shared<ColorShader>();
    colorShader->createProgram(vertexShader.c_str(), fragmentShader.c_str());

    //auto camera = make_shared<Camera>(Cvec3(1.5, 0.5, 2), Quat::makeXRotation(-10) * Quat::makeYRotation(10));
    auto camera = make_shared<Camera>(Cvec3(0, 0, 0), Quat::makeXRotation(0));
    Scene::setCamera(camera);

    auto light0 = std::make_shared<Light>();
    light0->setPosition(0, 2, 5);
    light0->lightColor = Cvec3f(1, 1, 1);

    Scene::setLight0(light0);

//     auto textureShader = std::make_shared<TextureShader>();
//     textureShader->createProgram("shaders/vertex_shader_simple.glsl", "shaders/fragment_shader_texture.glsl");
//     auto planar = std::make_shared<Plane>(4);
//     //auto material = std::make_shared<Material>("assets/checkerboard.jpg");
//     auto material = std::make_shared<Material>(Cvec3f(0, 0, 1));
//     auto ground = std::make_shared<Entity>(planar, material, "ground");
//     ground->setPosition(Cvec3(0.5, -2, -4));
//     ground->setScale(Cvec3(2, 2, 2));
//     ground->setShader(colorShader);
//     Scene::addChild(ground);

    
    auto model0 = std::make_shared<Model>("assets/models/face/face.obj", "model0", "assets/models/face/");
    //    auto model0 = std::make_shared<Model>("assets/models/monkey.obj", "model0", "assets/models/");
    model0->material->setColor(0.8, 0.8, 0.8);
    model0->translate(Cvec3(0, 0, -4));

//     model0->setRotation(Quat::makeYRotation(30) * Quat::makeXRotation(-30));
    model0->setShader(colorShader);
    Scene::addChild(model0);

    //set trackball params
    trackball.setInitRotation(model0->getRotation());

    // genereate vbo/ibo for the geometry of each Entity.
    Scene::createMeshes();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    screenWidth = w;
    screenHeight = h;
    trackball.updateScreenSize(w, h);
}

void idle(void) {
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    auto model = Scene::getEntity("model0");
    switch (key) {
        case 'q':
        case 'Q':
        {
            exit(0);
            break;
        }
        case 'w':
            model->translate(Cvec3(0, 0.2, 0));
            break;
        case 's':
            model->translate(Cvec3(0, -0.2, 0));
            break;
        case 'a':
            model->translate(Cvec3(-0.2, 0, 0));
            break;
        case 'd':
            model->translate(Cvec3(0.2, 0, 0));
            break;
        case 'z':
            model->translate(Cvec3(0, 0, -0.2));
            break;
        case 'x':
            model->translate(Cvec3(0, 0, 0.2));
            break;
        default:
            break;
    }
}

bool mouseLeftDown = false;

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            trackball.record(x, y);
            mouseLeftDown = true;
        } else if (state == GLUT_UP) {
            mouseLeftDown = false;
        }
    }
}

void motion(int x, int y) {
    if (mouseLeftDown) {
        //Face control
        Quat rotation = trackball.getRotation(x, y);
        rotMatrix = quatToMatrix(rotation);

        auto model = Scene::getEntity("model0");
        if (model != NULL) {
            Quat rotation = trackball.getRotation(x, y);
            model->setRotation(rotation);
            std::cout << "position: " << model->getPosition() << std::endl;
            std::cout << "rotation: " << model->getRotation() << std::endl;


            std::cout << "modelMatrix: " << std::endl;
            Matrix4 mm = model->getModelMatrix();
            for (int i = 0; i < 16; ++i) {
                std::cout << mm[i] << " ";
                if ((i + 1) % 4 == 0) {
                    std::cout << std::endl;
                }
            }
        }
    }
}

BoundingBox calcBoundingBox(const std::vector<Cvec3f>& vs) {
    BoundingBox bbox;
    for (int i = 0; i < vs.size(); ++i) {
        for (int j = 0; j < 3; j++) {
            if (vs[i][j] < bbox.min[j]) {
                bbox.min[j] = vs[i][j];
            }
            if (vs[i][j] > bbox.max[j]) {
                bbox.max[j] = vs[i][j];
            }
        }
    }
    return bbox;
}

void normPointCloud(std::vector<Cvec3f>& pc) {
    BoundingBox bbox = calcBoundingBox(pc);
    std::cout << "bbox min: " << bbox.min << std::endl;
    std::cout << "bbox max: " << bbox.max << std::endl;

    /** Calculate the scale and translation to recenter and normalize the geometry **/
    float abs_max = 0;
    for (int i = 0; i < 3; i++) {
        if (abs_max < std::abs(bbox.max[i])) {
            abs_max = std::abs(bbox.max[i]);
        }
        if (abs_max < std::abs(bbox.min[i])) {
            abs_max = std::abs(bbox.min[i]);
        }
    }
    std::cout << "max_abso: " << abs_max << std::endl;

    //Scale and recenter 
    for (int i = 0; i < pc.size(); i++) {
        pc[i] -= (bbox.max + bbox.min) * 0.5;
        pc[i] /= abs_max;
    }
}
void drawAxes(float length) {
    glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT) ;

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) ;
    glDisable(GL_LIGHTING) ;

    glLineWidth((GLfloat)3);
    glBegin(GL_LINES) ;
    glColor3f(1,0,0) ;
    glVertex3f(0,0,0) ;
    glVertex3f(length,0,0);

    glColor3f(0,1,0) ;
    glVertex3f(0,0,0) ;
    glVertex3f(0,length,0);

    glColor3f(0,0,1) ;
    glVertex3f(0,0,0) ;
    glVertex3f(0,0,length);
    glEnd() ;


    glPopAttrib() ;
}
float lastAngle = 0;
void drawPoints(void) {
    //    std::vector<Cvec3f> pc = bq.consume();
    //    std::vector<Cvec3f> pc = pointCloud;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, screenWidth, screenHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, screenWidth * 1.0 / screenHeight, 0.01, 200);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 2, 0, 0, 0, 0, 1, 0);

    glPushMatrix();

    rotMatrix.writeToColumnMajorMatrix(colRotMat);
    glMultMatrixd(colRotMat);
    glScalef(2, 2, 2);
    //    glRotatef(lastAngle++, 0, 1, 0);
    glPointSize(3.0);
    glBegin(GL_POINTS);
    //    glColor3f( 0.95f, 0.207, 0.031f );
    glColor3f(1, 0, 0);
    
    //    mu.lock();
    for (int i = 0; i < LANDMARK_NUM; ++i) {
        glVertex3f(pointCloud[i][0], pointCloud[i][1], pointCloud[i][2]);
    }
    //    mu.unlock();
    glEnd();

    glPopMatrix();

//     glPushMatrix();
//     glRotatef(20, 0, 1, 0);
//     glTranslatef(-2, -1, -2);
//     drawAxes(1.0);
//     glPopMatrix();

    glutSwapBuffers();
    glutPostRedisplay();
}

void initDrawPoints(void) {
//     for (int i = 0; i < pointCloud.size(); ++i) {
//         pointCloud = Cvec3f(0.1 * i, 0.2 * i, -0.3 * i);
//     }
    glClearColor(0.7, 0.7, 0.7, 1.0);
    glViewport(0, 0, screenWidth, screenHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, screenWidth * 1.0 / screenHeight, 0.1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}
void consumer() {
    while (1) {
        std::vector<Cvec3f> pc = bq.consume();
        mu.lock();
        pointCloud = pc;
        mu.unlock();
    }
}

int main(int argc, char **argv) {
    std::thread t(setupServer, 5055);
    //    std::thread t2(consumer);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Point Cloud");

    glutDisplayFunc(display);
    //glutDisplayFunc(drawPoints);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);

    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    //    initDrawPoints();
    init();
    glutMainLoop();

    t.join();
    //    t2.join();
    
    return 0;
}


#ifdef TEST_BUFFER
void producer() {
    for (int i = 0; i < 100; ++i) {
        bq.send(pointCloud);
    }
}
void consumer() {
    for (int i = 0; i < 100; ++i) {
        bq.consume();
    }
}

int main(int argc, char **argv) {
    initDrawPoints();
    std::thread t1(producer);
    std::thread t2(consumer);

    t1.join();
    t2.join();
    std::cout << "end" << std::endl;

    return 0;
}
#endif

