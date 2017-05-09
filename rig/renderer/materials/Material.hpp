#ifndef Material_hpp
#define Material_hpp

#include "../base/cvec.h"
#include "../base/stb_image.h"
#include "../base/glsupport.h"
#include "Texture.hpp"
#include "Cubemap.hpp"

/**
 * Currently wrapping texture and color uniform.
 * More to add in the future.
 */
class Material {

public:
    Material();
    Material(const Cvec3f& color_);
    Material(const char* textureFileName);
    ~Material();
    
    void setColor(const Cvec3f& color);
    void setColor(float r, float g, float b);
    
    const Cvec3f& getColor() const   { return color; }
    
    bool hasDiffuseTexture() const   { return texture.hasDiffuseTexture(); }
    bool hasSpecularTexture() const  { return texture.hasSpecularTexture(); }
    bool hasNormalTexture() const    { return texture.hasNormalTexture(); }
    bool hasCubemap() const          { return texture.hasCubmapTexture(); }
    
    GLint getDiffuseTexture() const  { return texture.getDiffuseTexture(); }
    GLint getSpecularTexture() const { return texture.getSpecularTexture(); }
    GLint getNormalTexture() const   { return texture.getNormalTexture(); }
    GLint getCubemapTexture() const  { return texture.getCubemapTexture(); }
    
    void setDiffuseTexture(std::string diffuseTexName);
    void setDiffuseTextureId(GLuint textureId);
    void setSpecularTexture(std::string specularTexName);
    void setNormalTexture(std::string normalTexName);
    void setCubemap(GLuint cubemapTexture);

private:
    Cvec3f color;
    Texture texture;
};

#endif /* Material_hpp */
