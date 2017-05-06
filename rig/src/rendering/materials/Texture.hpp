#ifndef Texture_hpp
#define Texture_hpp

#include <stdio.h>
#include <GLUT/glut.h>

class Texture {

public:
    Texture();
    ~Texture();

    bool hasDiffuseTexture() const  { return hasDiffuseTex; }
    bool hasSpecularTexture() const { return hasSpecularTex; }
    bool hasNormalTexture() const   { return hasNormalTex; }
    bool hasCubmapTexture() const   { return hasCubemapTex; }

    GLuint getDiffuseTexture() const { return diffuseTexture; }
    GLuint getSpecularTexture() const { return specularTexture; }
    GLuint getNormalTexture() const { return normalTexture; }
    GLuint getCubemapTexture() const { return cubemapTexture; }

    void setDiffuseTexture(GLuint diffuse);
    void setSpecularTexture(GLuint specular);
    void setNormalTexture(GLuint normal);
    void setCubemapTexture(GLuint cubemap);

private:
    bool hasDiffuseTex;
    bool hasSpecularTex;
    bool hasNormalTex;
    
    GLuint diffuseTexture;
    GLuint specularTexture;
    GLuint normalTexture;
    
    bool hasCubemapTex;
    GLuint cubemapTexture;
};

#endif /* Texture_hpp */
