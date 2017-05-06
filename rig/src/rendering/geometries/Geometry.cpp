#include <iostream>
#include "Geometry.hpp"
#include "../base/geometrymaker.h"

Geometry::Geometry() : indicesNum(0),
                       boundingBoxLength(0),
                       created(false)
{
}

Geometry::~Geometry()
{
}

Geometry& Geometry::operator = (const Geometry& g) {
    vertexVBO = g.vertexVBO;
    indexVBO = g.indexVBO;
    indicesNum = g.indicesNum;
    return *this;
}

void Geometry::createVBOs(std::vector<Vertex>& vtx, const std::vector<unsigned short>& idx, bool normalize) {
    if (created) {
        return;
    } else {
        created = true;
    }

    if (normalize) {
        this->normalizeVertices(vtx);
    }

    indicesNum = idx.size();
    glGenBuffers(1, &vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vtx.size(), vtx.data(), GL_STATIC_DRAW);
    
    glGenBuffers(1, &indexVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * idx.size(), idx.data(), GL_STATIC_DRAW);
}

void Geometry::draw(const GLuint aPositionLocation, const GLuint aNomralLocation, const GLuint aTexCoordLocation, const GLuint aBinormalLocation, const GLuint aTangentLocation) {
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    
    glVertexAttribPointer(aTexCoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(aTexCoordLocation);
    
    glVertexAttribPointer(aPositionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(aPositionLocation);
    
    glVertexAttribPointer(aNomralLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(aNomralLocation);
    
    glVertexAttribPointer(aBinormalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, binormal));
    glEnableVertexAttribArray(aBinormalLocation);
    
    glVertexAttribPointer(aTangentLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(aTangentLocation);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
    glDrawElements(GL_TRIANGLES, indicesNum, GL_UNSIGNED_SHORT, 0);
}

BoundingBox Geometry::calcBoundingBox(const std::vector<Vertex>& vertices) {
    BoundingBox bbox;
    std::cout << "bbox min: " << bbox.min << std::endl;
    std::cout << "bbox max: " << bbox.max << std::endl;
    for (auto it = vertices.cbegin(); it != vertices.cend(); ++it) {
        for (int j = 0; j < 3; j++) {
            if (it->position[j] < bbox.min[j]) {
                bbox.min[j] = it->position[j];
            }
            if (it->position[j] > bbox.max[j]) {
                bbox.max[j] = it->position[j];
            }
        }
    }
    this->bbox = bbox;
    return bbox;
}

void Geometry::normalizeVertices(std::vector<Vertex>& vertices) {
    BoundingBox bbox = this->calcBoundingBox(vertices);
    std::cout << "bbox min: " << bbox.min << std::endl;
    std::cout << "bbox max: " << bbox.max << std::endl;

    /** Calculate the scale and translation to recenter and normalize the geometry **/
    float abs_max = 0;
    for (int i = 0; i < 3; i++) {
        if (abs_max < std::abs(bbox.max[i])) {
            abs_max = std::abs(bbox.max[i]);
        }
        if (abs_max < std::abs(bbox.min[i])) {
            abs_max = std::abs(bbox.min[i]);
        }
    }
    std::cout << "max_abso: " << abs_max << std::endl;

    //Scale and recenter the vertices
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].position -= (bbox.max + bbox.min) * 0.5;
        vertices[i].position /= abs_max;
    }
}
