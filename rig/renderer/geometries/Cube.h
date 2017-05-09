#ifndef Cube_h
#define Cube_h

#include "Geometry.hpp"

class Cube : public Geometry {
public:
    
    Cube(float size) {
        this->size = size;

        indicesNum = 0;
        boundingBoxLength = size;
    }
    
    void createVBOs() {
        int vertexBufferLen, indexBufferLen;
        getCubeVbIbLen(vertexBufferLen, indexBufferLen);
        
        std::vector<Vertex> vtx(vertexBufferLen);
        std::vector<unsigned short> idx(indexBufferLen);
        
        makeCube(size, vtx.begin(), idx.begin());
        
        Geometry::createVBOs(vtx, idx);
    }

protected:
    float size;
};

#endif /* Cube_h */
