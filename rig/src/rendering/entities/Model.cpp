#define TINYOBJLOADER_IMPLEMENTATION
#include "../base/tiny_obj_loader.h"

#include "Model.hpp"
#include "../materials/Material.hpp"
#include "../geometries/Geometry.hpp"

Model::Model(const std::string fileName, std::string name, std::string basePath) : fileName(fileName),
                                                                                   basePath(basePath)
{
    this->name = name;

    material = std::make_shared<Material>();
        
    std::vector<Vertex> vtx;
    std::vector<unsigned short> idx;
    loadFromFile(fileName, vtx, idx);

    geometry = std::make_shared<Mesh>(vtx, idx);
}

void Model::calcFaceTangent(const Cvec3f& v1, const Cvec3f& v2, const Cvec3f& v3, const Cvec2f& texcoord1, const Cvec2f& texcoord2, const Cvec2f& texcoord3, Cvec3f& tangent, Cvec3f& binormal, Cvec3f& normal) {
        
    Cvec3f side0 = v1 - v2;
    Cvec3f side1 = v3 - v1;
    normal = cross(side1, side0);
    normal.normalize();
        
    float deltaV0 = texcoord1[1] - texcoord2[1];
    float deltaV1 = texcoord3[1] - texcoord1[1];
    tangent = side0 * deltaV1 - side1 * deltaV0;
    tangent.normalize();
        
    float deltaU0 = texcoord1[0] - texcoord2[0];
    float deltaU1 = texcoord3[0] - texcoord1[0];
    binormal = side0 * deltaU1 - side1 * deltaU0;
    binormal.normalize();

    Cvec3f tangentCross = cross(tangent, binormal);
    if (dot(tangentCross, normal) < 0.0f) {
        tangent = tangent * -1;
    }
}
    
void Model::loadFromFile(const std::string& fileName, std::vector<Vertex>& vertices, std::vector<unsigned short>& indices) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    //TODO add getBaseDir function
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileName.c_str(), basePath.c_str(), true);
    if (!err.empty()) {
        std::cerr << err << std::endl;
    }
    if (!ret) {
        std::cerr << "Failed to load " << fileName << std::endl;
    }

    std::cout << "Loading model from: " << fileName << std::endl;
    std::cout << "# of vertices = " << (int)(attrib.vertices.size() / 3) << std::endl;
    std::cout << "# of normals = " << (int)(attrib.normals.size() / 3) << std::endl;
    std::cout << "# of texcoords = " << (int)(attrib.texcoords.size() / 2) << std::endl;
    std::cout << "# of materials = " << (int)materials.size() << std::endl;
    std::cout << "# of shapes = " << (int)shapes.size() << std::endl;

    if (materials.size() > 0) {
        if (materials[0].diffuse_texname != "") {
            material->setDiffuseTexture(basePath + materials[0].diffuse_texname);
        }
        if (materials[0].specular_texname != "") {
            material->setSpecularTexture(basePath + materials[0].specular_texname);
        }
        if (materials[0].normal_texname != "") {
            material->setNormalTexture(basePath + materials[0].normal_texname);
        }
    }

    for (int i = 0; i < shapes.size(); i++) {
        for (int j = 0; j < shapes[i].mesh.indices.size(); j++) {
            unsigned int vertexOffset = shapes[i].mesh.indices[j].vertex_index * 3;
            unsigned int normalOffset = shapes[i].mesh.indices[j].normal_index * 3;
            unsigned int texCoordOffset = shapes[i].mesh.indices[j].texcoord_index * 2;
                    
            Vertex v;
            v.position[0] = attrib.vertices[vertexOffset];
            v.position[1] = attrib.vertices[vertexOffset + 1];
            v.position[2] = attrib.vertices[vertexOffset + 2];

            if (attrib.normals.size() > 0) {
                v.normal[0] = attrib.normals[normalOffset];
                v.normal[1] = attrib.normals[normalOffset + 1];
                v.normal[2] = attrib.normals[normalOffset + 2];
            }

            if (attrib.texcoords.size() > 0) {
                v.texCoord[0] = attrib.texcoords[texCoordOffset];
                v.texCoord[1] = 1.0 - attrib.texcoords[texCoordOffset + 1];
            }
                    
            vertices.push_back(v);
            indices.push_back(vertices.size() - 1);
        }
    }

    for (int i = 0; i < vertices.size(); i += 3) {
        Cvec3f tangent;
        Cvec3f binormal;
        Cvec3f normal;
        calcFaceTangent(vertices[i].position, vertices[i + 1].position, vertices[i + 2].position,
                        vertices[i].texCoord, vertices[i + 1].texCoord, vertices[i + 2].texCoord, tangent, binormal, normal);
                
        vertices[i].tangent = tangent;
        vertices[i + 1].tangent = tangent;
        vertices[i + 2].tangent = tangent;
                
        vertices[i].binormal = binormal;
        vertices[i + 1].binormal = binormal;
        vertices[i + 2].binormal = binormal;
            
        // Use the calculated normals if the model does not have normals
        if (attrib.normals.size() == 0) {
            vertices[i].normal = normal;
            vertices[i + 1].normal = normal;
            vertices[i + 2].normal = normal;
        }
    }
}

