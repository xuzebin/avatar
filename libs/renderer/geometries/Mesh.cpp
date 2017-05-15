#include "Mesh.hpp"

Mesh::Mesh(std::vector<Vertex> vtx, std::vector<unsigned short> idx) : vtx(vtx),
                                                                 idx(idx)
{
}

Mesh::~Mesh()
{
}

void Mesh::createVBOs() {
    if (vtx.empty() || idx.empty()) {
        throw std::string("vertex or index array NULL");
    }
    
    Geometry::createVBOs(vtx, idx);
    
    clearDataInMemoery();
}

void Mesh::clearDataInMemoery() {
    vtx.clear();
    idx.clear();
}
