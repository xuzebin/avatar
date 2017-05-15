#include "Material.hpp"

Material::Material()
{
}

Material::Material(const Cvec3f& color) : color(color)
{
}
    
Material::Material(const char* textureFileName)
{
    setDiffuseTexture(textureFileName);
}

Material::~Material()
{
}

void Material::setColor(const Cvec3f& color) { this->color = color; }

void Material::setColor(float r, float g, float b) {
    this->color[0] = r;
    this->color[1] = g;
    this->color[2] = b;
}


void Material::setDiffuseTexture(std::string diffuseTexName) {
    if (diffuseTexName != "") {
        texture.setDiffuseTexture(loadGLTexture(diffuseTexName.c_str(), true, false));
        std::cout << "diffuse texture set: " << diffuseTexName << std::endl;
    } else {
        std::cerr << "diffuse texture name empty" << std::endl;
    }
}
    
void Material::setDiffuseTextureId(GLuint textureId) {
    texture.setDiffuseTexture(textureId);
}
    
void Material::setSpecularTexture(std::string specularTexName) {
    if (specularTexName != "") {
        texture.setSpecularTexture(loadGLTexture(specularTexName.c_str(), true, false));
        std::cout << "specular texture set: " << specularTexName << std::endl;
    } else {
        std::cerr << "specular texture name empty" << std::endl;
    }
}
    
void Material::setNormalTexture(std::string normalTexName) {
    if (normalTexName != "") {
        texture.setNormalTexture(loadGLTexture(normalTexName.c_str(), true, false));
        std::cout << "normal texture set: " << normalTexName << std::endl;
    } else {
        std::cerr << "normal texture name empty" << std::endl;
    }
}
    
void Material::setCubemap(GLuint cubemapTexture) {
    texture.setCubemapTexture(cubemapTexture);
}
