#ifndef FrameBufferObject_hpp
#define FrameBufferObject_hpp

#include <GLUT/glut.h>

class FrameBufferObject {

public:

    FrameBufferObject(GLsizei width = 1024, GLsizei height = 1024, bool highPrecision = false);
    ~FrameBufferObject();

    GLuint getFrameBuffer() const        { return frameBuffer; }
    GLuint getFrameBufferTexture() const { return frameBufferTexture; }
    GLuint getDepthBufferTexture() const { return depthBufferTexture; }

private:

    void init(GLsizei fboWidth, GLsizei fboHeight);

    GLuint frameBuffer;
    GLuint frameBufferTexture;
    GLuint depthBufferTexture;

    GLsizei width;
    GLsizei height;
    bool highPrecision;
};

#endif /* FrameBufferObject_hpp */
