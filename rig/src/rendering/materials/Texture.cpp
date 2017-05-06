#include "Texture.hpp"

Texture::Texture() : diffuseTexture(0), specularTexture(0), normalTexture(0), cubemapTexture(0),
            hasDiffuseTex(false), hasSpecularTex(false), hasNormalTex(false), hasCubemapTex(false)
{
}

Texture::~Texture()
{
}

void Texture::setDiffuseTexture(GLuint diffuse) {
    diffuseTexture = diffuse;
    hasDiffuseTex = true;
}
void Texture::setSpecularTexture(GLuint specular) {
    specularTexture = specular;
    hasSpecularTex = true;
}
void Texture::setNormalTexture(GLuint normal) {
    normalTexture = normal;
    hasNormalTex = true;
}
void Texture::setCubemapTexture(GLuint cubemap) {
    cubemapTexture = cubemap;
    hasCubemapTex = true;
}
