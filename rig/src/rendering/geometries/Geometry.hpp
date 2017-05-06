#ifndef Geometry_hpp
#define Geometry_hpp

#include <GLUT/glut.h>
#include <stdio.h>
#include <vector>
#include "../base/Vertex.h"
#include "../base/cvec.h"

struct BoundingBox {
    Cvec3f min;
    Cvec3f max;
};

/**
 * An abstract class that genereates and saves vbo/ibo information.
 * we use this type for polymorphism in Entity class.
 */
class Geometry {
public:
    virtual void createVBOs() = 0;
    virtual ~Geometry();
    
    Geometry& operator = (const Geometry& g);
    void createVBOs(std::vector<Vertex>& vtx, const std::vector<unsigned short>& idx, bool normalize = true);
    virtual void draw(const GLuint aPositionLocation, const GLuint aNomralLocation, const GLuint aTexCoordLocation, const GLuint aBinormalLocation, const GLuint aTangentLocation);
    
    float getBoundingBoxLength() { return boundingBoxLength; }

    const BoundingBox& getBoundingBox() const { return bbox; }

    BoundingBox calcBoundingBox(const std::vector<Vertex>& vertices);

protected:
    Geometry();
    void normalizeVertices(std::vector<Vertex>& vertices);

    unsigned short indicesNum;
    float boundingBoxLength;//currently all objects are considered sphere when testing intersection

private:
    GLuint vertexVBO;
    GLuint indexVBO;
    
    bool created;

    BoundingBox bbox;
};




#endif /* Geometry_hpp */
