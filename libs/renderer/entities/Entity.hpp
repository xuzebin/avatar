#ifndef Entity_hpp
#define Entity_hpp

#include <stdio.h>
#include "../core/Transform.hpp"
#include "../geometries/Geometry.hpp"
#include "../materials/Material.hpp"
#include "../programs/Shader.h"
#include "../core/Light.hpp"

class Camera;
class Entity;

enum EventType {
    CLICK = 0,
    HOVER = 1,
    IDLE = 2,
    SCROLL = 3
};

class ClickEventListener {
public:
    ClickEventListener() {}
    virtual ~ClickEventListener(){}

    virtual void onClick(Entity& entity) = 0;
    virtual void onHover(Entity& entity) = 0;
    virtual void onIdle(Entity& entity) = 0;
};


struct InitState {
    Transform transform;
    Cvec3f color;
};


/**
 * The object/Entity to be rendered.
 */
class Entity {
public:
    Entity(std::shared_ptr<Geometry> geometry = nullptr, std::shared_ptr<Material> material = nullptr, std::string name = "");
    ~Entity();

    void createMesh();
    void draw(std::shared_ptr<Camera> camera, std::shared_ptr<Shader> shader, std::shared_ptr<Light> light0, std::shared_ptr<Light> light1);

    void acceptLight(int lightID);
    void rejectLight(int lightID);
    void rejectAllLights() { lightSwitch = 0; }
    bool isLightOn(int lightID);

    void registerClickEventListener(std::unique_ptr<ClickEventListener> listener);
    void removeClickEventListener()     { clickEventListener.reset(nullptr); }
    bool clickEventListenerRegistered() { return clickEventListener != nullptr; }
    
    void notify(EventType type);

    void setVisible(bool visible)                         { this->visible = visible; }
    void setDepthTest(bool enable)                        { this->depthTest = enable; }
    void setPosition(const Cvec3& position)               { transform.setPosition(position); }
    void translate(const Cvec3& translation)              { transform.translate(translation); }
    void setRotation(const Quat& rotation)                { transform.setRotation(rotation); }
    void rotate(const Quat& rotation)                     { transform.rotate(rotation); }
    void setScale(const Cvec3& scale)                     { transform.setScale(scale); }
    void setModelMatrix(const Matrix4& m)                 { transform.setModelMatrix(m); }
    void setShader(const std::shared_ptr<Shader>& shader) { this->shader = shader; }
    void setParent(std::shared_ptr<Entity> parent)        { this->parent = parent; }

    int getProgram() const                           { return shader->getProgramId(); }
    std::string getName() const                      { return this->name; }
    bool isVisible() const                           { return this->visible; }
    bool depthTestEnabled() const                    { return this->depthTest; }
    const Cvec3& getPosition() const                 { return transform.getPosition(); }
    const Quat& getRotation() const                  { return transform.getRotation(); }
    const Cvec3& getScale() const                    { return transform.getScale(); }
    const Matrix4& getModelMatrix()                  { return transform.getModelMatrix(); }
    const std::shared_ptr<Shader>& getShader() const { return shader; }
    const std::shared_ptr<Entity>& getParent() const { return parent; }
    float getBoundingBoxLength()                     { return geometry->getBoundingBoxLength() * getScale()[0]; }


public:
    Transform transform;
    std::shared_ptr<Geometry> geometry;
    std::shared_ptr<Material> material;
    
    InitState initState;

protected:
    void setName(int counter);

    static int id_counter;

    std::string name;
    bool visible;
    bool depthTest;
    
    int programId;

    std::shared_ptr<Entity> parent;
    
    std::unique_ptr<ClickEventListener> clickEventListener;//currently only one event listener for each entity.

    //by default: open 2 lights(..00011)
    int lightSwitch = 3;//0001 indicates light0 opens, 0010 indicates light1 opens, 0011 indicates light0 and light1 open, and so on.

    std::shared_ptr<Shader> shader;


};

#endif /* Entity_hpp */
