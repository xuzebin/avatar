#include "Scene.hpp"
#include "Camera.hpp"
#include "../entities/Entity.hpp"
#include "../physics/Raycaster.hpp"
#include "../glObjects/FrameBufferObject.hpp"

std::unordered_map<std::string, std::shared_ptr<Entity> > Scene::entityTable;
std::vector<std::shared_ptr<Entity> > Scene::entities;

std::shared_ptr<FrameBufferObject> Scene::frameBufferObject(nullptr);
std::shared_ptr<Entity> Scene::screen(nullptr);
std::shared_ptr<Camera> Scene::camera(nullptr);

std::shared_ptr<Light> Scene::light0(nullptr);
std::shared_ptr<Light> Scene::light1(nullptr);

std::unique_ptr<Raycaster> Scene::raycaster(nullptr);

Scene::Scene()
{
}

Scene::~Scene()
{
    removeAll();
}

const std::shared_ptr<Light>& Scene::getLight(int index) {
    switch(index) {
        case 0:
            return light0;
        case 1:
            return light1;
        default:
            std::cerr << "No matched light index" << std::endl;
            return nullptr;
    }
}

void Scene::addChild(std::shared_ptr<Entity> entity) {
    entities.push_back(entity);
    entityTable[entity->getName()] = entity;
}

void Scene::createMeshes() {
    for(auto it = entities.begin(); it != entities.end(); ++it) {
        (*it)->createMesh();
    }

    if (screen != NULL) {
        screen->createMesh();
    }
}

bool Scene::testIntersect(const std::shared_ptr<Entity>& entity, int x, int y, int screenWidth, int screenHeight) {
    if (entity == NULL) {
        return false;
    }
    //lazy initialization
    if (raycaster == nullptr) {
        raycaster.reset(new Raycaster());
    }
    return raycaster->isPicked(x, y, screenWidth, screenHeight, camera->getProjectionMatrix(), camera->getViewMatrix(), camera->getPosition(), entity->getPosition(), entity->getBoundingBoxLength() / 2.0, camera->getZNear());

}

void Scene::renderLoop() {
    for(auto it = entities.begin(); it != entities.end(); ++it) {
        auto shader = (*it)->getShader();
        (*it)->draw(camera, shader, light0, light1);
    }
}

void Scene::render() {
    assert(("Camera not set", camera != nullptr));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderLoop();
    glutSwapBuffers();
}

void Scene::renderToTexture() {
    assert(("Camera not set", camera != nullptr));
    assert(("FrameBufferObject not set", frameBufferObject != nullptr));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject->getFrameBuffer());
    glViewport(0, 0, 1024, 1024);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderLoop();
}

void Scene::renderToScreen(GLsizei windowWidth, GLsizei windowHeight) {
    assert(("Screen not set", screen != nullptr));

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWidth, windowHeight);
    screen->draw(Scene::camera, screen->getShader(), Scene::light0, Scene::light1);

    glutSwapBuffers();
}

const std::shared_ptr<Entity>& Scene::getEntity(std::string name) {
    if (entityTable.find(name) == entityTable.end()) {
        return nullptr;
    }
    return entityTable[name];
}

void Scene::removeAll() {
    entities.clear();
    entityTable.clear();
}

//TODO change method name
void Scene::updateMouseEvent(int button , int state, int x, int y, int screenWidth, int screenHeight) {
    //test intersection
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        //NOTE: currently only supports ray sphere intersection.
        for(auto it = entities.begin(); it != entities.end(); ++it) {
            if ((*it)->clickEventListenerRegistered()) {
                if (testIntersect(*it, x, y, screenWidth, screenHeight)) {
                    (*it)->notify(EventType::CLICK);
                }
            }
        }
    } else {
        for(auto it = entities.begin(); it != entities.end(); ++it) {
            if ((*it)->clickEventListenerRegistered()) {
                if (testIntersect(*it, x, y, screenWidth, screenHeight)) {
                    (*it)->notify(EventType::HOVER);
                } else {
                    (*it)->notify(EventType::IDLE);
                }
            }
        }
    }
}

void Scene::updateMousePassiveMotion(int x, int y, int screenWidth, int screenHeight) {
    for(auto it = entities.begin(); it != entities.end(); ++it) {
        if ((*it)->clickEventListenerRegistered()) {
            if (testIntersect(*it, x, y, screenWidth, screenHeight)) {
                (*it)->notify(EventType::HOVER);
            } else {
                (*it)->notify(EventType::IDLE);
            }
        }
    }
}
