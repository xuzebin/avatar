#include "FrameBufferObject.hpp"
#include <stddef.h>

FrameBufferObject::FrameBufferObject(GLsizei width, GLsizei height, bool highPrecision) : width(width),
                                                                                          height(height),
                                                                                          highPrecision(highPrecision)
{
    init(width, height);
}

FrameBufferObject::~FrameBufferObject() {
    glDeleteTextures(1, &frameBufferTexture);
    glDeleteTextures(1, &depthBufferTexture);
    glDeleteFramebuffers(1, &frameBuffer);
}

void FrameBufferObject::init(GLsizei fboWidth, GLsizei fboHeight) {
    //1. generate and bind a frame buffer
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    //2. create an empty texture to render to
    glGenTextures(1, &frameBufferTexture);
    glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
    if (highPrecision) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, fboWidth, fboHeight, 0, GL_RGB, GL_FLOAT, NULL);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fboWidth, fboHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //3. bind the texture to the FBO as a color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture, 0);

    //4. create a depth buffer texture
    glGenTextures(1, &depthBufferTexture);
    glBindTexture(GL_TEXTURE_2D, depthBufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, fboWidth, fboHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //bind the depth buffer texture to the FBO as a depth buffer attachment
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, fboWidth, fboHeight);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBufferTexture, 0);
        
    //5. unbind the created frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
