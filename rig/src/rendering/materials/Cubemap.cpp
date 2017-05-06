#include "Cubemap.hpp"
#include "../base/glsupport.h"

Cubemap::Cubemap() : texture(0)
{
}

Cubemap::~Cubemap()
{
}

void Cubemap::loadTextures(std::string posX, std::string negX,
                           std::string posY, std::string negY,
                           std::string posZ, std::string negZ) {
    
    std::vector<std::string> cubemapFiles;
    cubemapFiles.push_back(posX);
    cubemapFiles.push_back(negX);
    cubemapFiles.push_back(posY);
    cubemapFiles.push_back(negY);
    cubemapFiles.push_back(posZ);
    cubemapFiles.push_back(negZ);
    
    texture = loadGLCubemap(cubemapFiles);
}
