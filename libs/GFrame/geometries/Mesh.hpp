#ifndef Mesh_hpp
#define Mesh_hpp

#include <stdio.h>
#include <vector>
#include "Geometry.hpp"

class Mesh : public Geometry {
    
public:
    Mesh(std::vector<Vertex> vtx, std::vector<unsigned short> idx);
    ~Mesh();

    void createVBOs();

private:
    std::vector<Vertex> vtx;
    std::vector<unsigned short> idx;
    
    void clearDataInMemoery();
};


#endif /* Mesh_hpp */
