#ifndef Shader_h
#define Shader_h

class Entity;
class Light;
class Camera;

class Shader {
public:
    
    Shader() {}
    virtual ~Shader()        { glDeleteProgram(programId); }

    void use()               { glUseProgram(programId); }
    int getProgramId() const { return programId; }

    virtual void createProgram(const char* vertexShaderFileName, const char* fragmentShaderFileName) {
        programId = glCreateProgram();
        readAndCompileShader(programId, vertexShaderFileName, fragmentShaderFileName);
    }
    virtual void setLocationsAndDraw(Entity& entity, std::shared_ptr<Camera> camera, std::shared_ptr<Light> light0, std::shared_ptr<Light> light1) = 0;

protected:
    GLuint programId;
};

#endif /* Shader_h */
