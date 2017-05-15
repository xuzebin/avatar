#ifndef ScreenShader_h
#define ScreenShader_h

class Shader;

class ScreenShader : public Shader {
public:
    ~ScreenShader() {
        glDeleteProgram(programId);
    }

    void createProgram(const char* vertexShaderFileName, const char* fragmentShaderFileName) {
        Shader::createProgram(vertexShaderFileName, fragmentShaderFileName);
        getLocations(programId);
    }

    void setLocationsAndDraw(Entity& entity, std::shared_ptr<Camera> camera, std::shared_ptr<Light> light0, std::shared_ptr<Light> light1) {
        glUseProgram(programId);

        if (entity.material->hasDiffuseTexture()) {//we store the screen frame buffer texture in diffuse texture.
            glUniform1i(uScreenFrameBufferLoc, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, entity.material->getDiffuseTexture());
        }

        //fxaa shader
        glUniform2f(uResolutionLoc, 1024.0f, 1024.0f);

        //hdr tone shader
        glUniform1f(uExposureLoc, 3.0);

        //gaussian blur shader
        glUniform1f(uBlurSizeLoc, blurSize);

        //toonify filtering shader
        glUniform1f(uMouseXOffsetLoc, mouseX);

        entity.geometry->draw(aPositionLoc, -1, aTexCoordLoc, -1, -1);
    }

    void setBlurSize(float blurSize) {
        this->blurSize = blurSize;
    }

    float getBlurSize() {
        return blurSize;
    }

    void updateMouseX(float mouseX) {
        this->mouseX = mouseX;
    }


protected:
    void getLocations(int programId) {
        aPositionLoc = glGetAttribLocation(programId, "aPosition");
        aTexCoordLoc = glGetAttribLocation(programId, "aTexCoord");

        uScreenFrameBufferLoc = glGetUniformLocation(programId, "uScreenFrameBuffer");
        uResolutionLoc = glGetUniformLocation(programId, "uResolution");//used for fxaa shader
        uExposureLoc = glGetUniformLocation(programId, "uExposure");//for hdr tone shader
        uBlurSizeLoc = glGetUniformLocation(programId, "uBlurSize");//for blur shaders
        uMouseXOffsetLoc = glGetUniformLocation(programId, "uMouseXOffset");
    }

    //attributes
    GLint aPositionLoc;
    GLint aTexCoordLoc;

    //texture uniforms
    GLint uScreenFrameBufferLoc;

    //screen resolution for fxaa shader
    GLint uResolutionLoc;

    //exposure for hdr tone shader
    GLint uExposureLoc;

    //blur size for blurring shader
    GLint uBlurSizeLoc;

    //pass mouse offset in toonify filtering shader
    GLint uMouseXOffsetLoc;

    float mouseX = 0.5f;

    float blurSize = 0.01;//size of kernal for gaussian filter shader
};

#endif /* ScreenShader_h */
