#ifndef Scene_hpp
#define Scene_hpp

#include <GLUT/glut.h>
#include <unordered_map>
#include <vector>
#include <string>

class Camera;
class Entity;
class Light;
class Shader;
class ScreenShader;
class Raycaster;
class FrameBufferObject;

/**
 * A simple scene containing entities to be rendered.
 */
class Scene {

public:
    ~Scene();

    static void setCamera(std::shared_ptr<Camera> camera) { Scene::camera = camera; }
    static void setLight0(std::shared_ptr<Light> light)   { light0 = light; }
    static void setLight1(std::shared_ptr<Light> light)   { light1 = light; }
    static void addChild(std::shared_ptr<Entity> entity);

    static const std::shared_ptr<Camera>& getCamera()     { return camera; }
    static const std::shared_ptr<Light>& getLight(int index);
    static const std::shared_ptr<Entity>& getEntity(std::string name);

    //must be called and called once before rendering
    static void createMeshes();
    
    static bool testIntersect(const std::shared_ptr<Entity>& entity, int x, int y, int screenWidth, int screenHeight);

    static void renderLoop();
    static void render();
    static void renderToTexture();
    static void renderToScreen(GLsizei windowWidth, GLsizei windowHeight);

    static void removeAll();

    static void updateMouseEvent(int button , int state, int x, int y, int screenWidth, int screenHeight);
    static void updateMousePassiveMotion(int x, int y, int screenWidth, int screenHeight);

    //currently only support one fbo
    static void setFrameBufferObject(std::shared_ptr<FrameBufferObject> fbo) { frameBufferObject = fbo; }
    static void removeFrameBufferObject()                                    { frameBufferObject = NULL; }

    //set the target that the texture in FBO is rendered to, which will be rendered to the window.
    static void setScreen(std::shared_ptr<Entity> screen)                    { Scene::screen = screen; }

private:
    Scene();

    static std::shared_ptr<Camera> camera;

    static std::vector<std::shared_ptr<Entity> > entities;
    static std::unordered_map<std::string, std::shared_ptr<Entity> > entityTable;
    
    static std::shared_ptr<FrameBufferObject> frameBufferObject;

    static std::shared_ptr<Entity> screen;
    static std::shared_ptr<Light> light0;
    static std::shared_ptr<Light> light1;

    static std::unique_ptr<Raycaster> raycaster;
};


#endif /* Scene_hpp */
