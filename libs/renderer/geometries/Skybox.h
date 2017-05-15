#ifndef Skybox_h
#define Skybox_h

#include "Cube.h"

////The difference between SkyBox and Cube here is that SkyBox flips normals to point inside the box.
class SkyBox : public Cube {

public:
    SkyBox(int size) : Cube(size) 
    {
    }

    void createVBOs() {
        int vertexBufferLen, indexBufferLen;
        getCubeVbIbLen(vertexBufferLen, indexBufferLen);
        
        std::vector<Vertex> vtx(vertexBufferLen);
        std::vector<unsigned short> idx(indexBufferLen);
        
        makeCube(size, vtx.begin(), idx.begin());
        
        //flip normals of cube to point inside the box.
        for(std::vector<Vertex>::iterator it = vtx.begin(); it != vtx.end(); ++it) {
            it->normal *= -1;
        }
        
        Geometry::createVBOs(vtx, idx);
    }
};

#endif /* Skybox_h */
