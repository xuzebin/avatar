#include "Entity.hpp"

int Entity::id_counter = 0;

Entity::Entity(std::shared_ptr<Geometry> geometry, std::shared_ptr<Material> material, std::string name) : name(name),
                                                                                                           geometry(geometry),
                                                                                                           material(material),
                                                                                                           parent(nullptr),
                                                                                                           clickEventListener(nullptr),
                                                                                                           visible(true),
                                                                                                           depthTest(true)

{
    setName(id_counter++);
}

Entity::~Entity() 
{
}

void Entity::createMesh()  {
    assert(("Geometry not set", geometry != nullptr));

    geometry->createVBOs();
    
    initState.transform = transform;
    initState.color = material->getColor();
}

void Entity::draw(std::shared_ptr<Camera> camera, std::shared_ptr<Shader> shader, std::shared_ptr<Light> light0, std::shared_ptr<Light> light1) {
    if (!isVisible()) {
        return;
    }
    assert(("Geometry not set", geometry != nullptr));
    assert(("Material not set", material != nullptr));

    shader->setLocationsAndDraw(*this, camera, light0, light1);
}

void Entity::acceptLight(int lightID) {
    if (lightID < 0 || lightID > 1) {
        std::cerr << "invalid lightID" << std::endl;
        return;
    }
    int mask = 1;
    mask <<= lightID;

    lightSwitch |= mask;
}

void Entity::rejectLight(int lightID) {
    if (lightID < 0 || lightID > 1) {
        std::cerr << "invalid lightID" << std::endl;
        return;
    }
    int mask = 1;
    mask <<= lightID;
    mask = !mask;

    lightSwitch &= mask;
}

bool Entity::isLightOn(int lightID) {
    if (lightID < 0 || lightID > 1) {
        std::cerr << "invalid lightID" << std::endl;
        return false;
    }
    int mask = 1;
    mask <<= lightID;

    int bit = lightSwitch & mask;
    return (bit != 0);
}

void Entity::registerClickEventListener(std::unique_ptr<ClickEventListener> listener) {
    assert(("ClickEventListener null", listener != nullptr));

    clickEventListener = std::move(listener);
}

void Entity::notify(EventType type) {
    if (clickEventListener != nullptr) {
        switch(type) {
            case EventType::CLICK:
                clickEventListener->onClick(*this);
                break;
            case EventType::HOVER:
                clickEventListener->onHover(*this);
                break;
            case EventType::IDLE:
                clickEventListener->onIdle(*this);
                break;
            default:
                std::cerr << "no matched event type" << std::endl;
                return;
        }
    }
}

void Entity::setName(int counter) {
    if (name.empty()) {
        name = "Entity" + std::to_string(counter);
    }
}
