#ifndef Screen_h
#define Screen_h

#include "Geometry.hpp"

class Screen : public Geometry {
public:
    Screen() {
        this->size = 2;
        
        indicesNum = 0;
        this->boundingBoxLength = size;
    }

    void createVBOs() {
        GLfloat screenPositions[] = {
            1.0f, 1.0f,
            1.0f, -1.0f,
            -1.0f, -1.0f,

            -1.0f, -1.0f,
            -1.0f, 1.0f,
            1.0f, 1.0f
        };

        GLfloat screenUVs[] = {
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,

            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f
        };


        glGenBuffers(1, &positionVBO);
        glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
        glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), screenPositions, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &texCoordVBO);
        glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
        glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), screenUVs, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }

    void draw(const GLuint aPositionLocation, const GLuint aNomralLocation, const GLuint aTexCoordLocation, const GLuint aBinormalLocation, const GLuint aTangentLocation) {

        glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
        glVertexAttribPointer(aPositionLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(aPositionLocation);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
        glVertexAttribPointer(aTexCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(aTexCoordLocation);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableVertexAttribArray(aPositionLocation);
        glDisableVertexAttribArray(aTexCoordLocation);
    }

protected:
    float size;

    GLuint positionVBO;
    GLuint texCoordVBO;
};

#endif /* Screen_h */
