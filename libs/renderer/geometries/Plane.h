#ifndef Plane_h
#define Plane_h

#include "Geometry.hpp"

class Plane : public Geometry {
public:
    Plane(float size) {
        this->size = size;

        indicesNum = 0;
        boundingBoxLength = size;
    }
    
    void createVBOs() {
        int vertexBufferLen, indexBufferLen;
        getPlaneVbIbLen(vertexBufferLen, indexBufferLen);
        
        std::vector<Vertex> vtx(vertexBufferLen);
        std::vector<unsigned short> idx(indexBufferLen);
        
        makePlane(size, vtx.begin(), idx.begin());
        
        Geometry::createVBOs(vtx, idx);
    }

protected:
    float size;
};

#endif /* Plane_h */
