/**
 * A viewer for a face model. The face model will change its rotation/translation given the pose data sent fromt the client.
 *
 * Usage: ./FaceViewer [obj file] [shaders directory] [port number]
 * Examples: ./FaceViewer face.obj shaders 5055
 * Press q to quit.
 */
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
#include "BufferData.h"
#include "PoseData.h"
#include "SocketServer.h"
#define LANDMARK_NUM 68

using namespace avt;

class Shader;

int screenWidth = 600;
int screenHeight = 600;

Trackball trackball(screenWidth, screenHeight);

// Callback for receiving a new matrix from the client.
void updateFacePose(const Matrix4& modelMatrix) {
    auto model = Scene::getEntity("model0");
    if (model != NULL) {
        model->setModelMatrix(modelMatrix);
    }
}

// This runs in a separate thread for updating face pose.
void setupServer(int portNumber) {
    auto data = std::make_shared<PoseData>();
    data->setCallback(updateFacePose);
    SocketServer server(data);
    server.start(portNumber);
}

void display(void) {
    Scene::render();
}

void init(std::string modelFile, std::string shadersDir) {
    glClearColor(0.7, 0.7, 0.7, 1.0);
    glClearDepth(0.0);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);
    glReadBuffer(GL_BACK);
    
    std::string vertexShader = shadersDir + "/vertex_shader_simple.glsl";
    std::string fragmentShader = shadersDir + "/fragment_shader_color.glsl";

    auto colorShader = std::make_shared<ColorShader>();
    colorShader->createProgram(vertexShader.c_str(), fragmentShader.c_str());

    auto camera = make_shared<Camera>(Cvec3(0, 0, 0), Quat::makeXRotation(0));
    Scene::setCamera(camera);

    auto light0 = std::make_shared<Light>();
    light0->setPosition(0, 2, 5);
    light0->lightColor = Cvec3f(1, 1, 1);

    Scene::setLight0(light0);
    
    auto model0 = std::make_shared<Model>(modelFile, "model0");
    model0->material->setColor(0.8, 0.8, 0.8);
    model0->translate(Cvec3(0, 0, -4));
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
        auto model = Scene::getEntity("model0");
        if (model != NULL) {
            Quat rotation = trackball.getRotation(x, y);
            model->setRotation(rotation);

#ifdef DEBUG
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
#endif
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 1 && argc != 4) {
        std::cout << "Usage: ./FaceViewer [obj file] [shaders directory] [port number]" << std::endl;
        std::cout << "Examples: ./FaceViewer face.obj shaders 5055" << std::endl;
        return 1;
    }

    std::string modelFile = "face.obj";
    std::string shadersDirectory = "shaders";
    if (argc == 4) {
        modelFile = argv[1];
        shadersDirectory = argv[2];
    }
    int portNumber = (argc == 4) ? std::stoi(argv[3]) : 5055;

    std::cout << "Press q to quit" << std::endl;
    
    // setup server listening to port 5055 in another thread
    std::thread t(setupServer, portNumber);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Face Viewer");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);

    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    init(modelFile, shadersDirectory);
    glutMainLoop();

    t.join();
    
    return 0;
}

