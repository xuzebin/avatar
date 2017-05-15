
#ifndef Model_hpp
#define Model_hpp

#include "../geometries/Mesh.hpp"
#include "../entities/Entity.hpp"

class Texture;
class Geometry;


class Model : public Entity {
public:
    Model(const std::string fileName, std::string name, std::string basePath = "");

    void calcFaceTangent(const Cvec3f& v1, const Cvec3f& v2, const Cvec3f& v3, 
                         const Cvec2f& texcoord1, const Cvec2f& texcoord2, const Cvec2f& texcoord3,
                         Cvec3f& tangent, Cvec3f& binormal, Cvec3f& normal);

    std::string getFileName() const { return fileName; }
    std::string getBasePath() const { return basePath; }
    const BoundingBox& getBoundingBox() const { return geometry->getBoundingBox(); }

private:
    void loadFromFile(const std::string& fileName, std::vector<Vertex>& vertices, std::vector<unsigned short>& indices);

    std::string fileName;
    std::string basePath;
};

#endif /* Model_hpp */
