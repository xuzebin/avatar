#ifndef TextureShader_h
#define TextureShader_h

#include "Shader.h"
#include "../entities/Entity.hpp"

class TextureShader : public Shader {
public:
    
    void createProgram(const char* vertexShaderFileName, const char* fragmentShaderFileName) {
        Shader::createProgram(vertexShaderFileName, fragmentShaderFileName);
        getLocations(programId);
    }
    
    void setLocationsAndDraw(Entity& entity, std::shared_ptr<Camera> camera, std::shared_ptr<Light> light0, std::shared_ptr<Light> light1) {
        glUseProgram(programId);
        
        Matrix4 projectionMatrix = camera->getProjectionMatrix();

        //Transform hierachy, iteratively multiply parent rigidbody matrices
        //to get the ultimate modelmatrix that transform from object frame to world frame.
        Matrix4 modelMatrix;
        modelMatrix = entity.transform.getModelMatrix();
        
        auto current = entity.getParent();
        while (current != nullptr) {
            modelMatrix = current->transform.getRigidBodyMatrix() * modelMatrix;
            current = current->getParent();
        }
        
        const Matrix4 viewMatrix = camera->getViewMatrix();
        Matrix4 modelViewMatrix = inv(viewMatrix) * modelMatrix;
        Matrix4 normal = normalMatrix(modelViewMatrix);
        
        modelMatrix.writeToColumnMajorMatrix(modelMat);//TODO add switch
        modelViewMatrix.writeToColumnMajorMatrix(modelViewMat);
        projectionMatrix.writeToColumnMajorMatrix(projectionMat);
        normal.writeToColumnMajorMatrix(normalMat);
        
        glUniformMatrix4fv(uModelMatrixLoc, 1, false, modelMat);
        glUniformMatrix4fv(uModelViewMatrixLoc, 1, false, modelViewMat);
        glUniformMatrix4fv(uProjectionMatrixLoc, 1, false, projectionMat);
        glUniformMatrix4fv(uNormalMatrixLoc, 1, false, normalMat);
        
        Cvec3f color = entity.material->getColor();
        glUniform3f(uColorLoc, color[0], color[1], color[2]);
        
        if (light0 != nullptr) {
            Cvec3 lightPosEye0 = light0->getPositionInEyeSpace(viewMatrix);
            glUniform3f(uLightPositionLoc0, lightPosEye0[0], lightPosEye0[1], lightPosEye0[2]);
            Cvec3f lightColor = light0->lightColor;
            glUniform3f(uLightColorLoc0, lightColor[0], lightColor[1], lightColor[2]);

            Cvec3f specularLightColor;
            if (entity.isLightOn(0)) {
                specularLightColor = light0->specularLightColor;
            } else {
                specularLightColor = Cvec3f(0, 0, 0);
            }

            glUniform3f(uSpecularLightColorLoc0, specularLightColor[0], specularLightColor[1], specularLightColor[2]);
        } else {
            glUniform3f(uLightPositionLoc0, 0, 0, 0);
            glUniform3f(uLightColorLoc0, 1.0f, 1.0f, 1.0f);
            glUniform3f(uSpecularLightColorLoc0, 0, 0, 0);
        }
        if (light1 != nullptr) {
            Cvec3 lightPosEye1 = light1->getPositionInEyeSpace(viewMatrix);
            glUniform3f(uLightPositionLoc1, lightPosEye1[0], lightPosEye1[1], lightPosEye1[2]);
            Cvec3f lightColor = light1->lightColor;
            glUniform3f(uLightColorLoc1, lightColor[0], lightColor[1], lightColor[2]);

            Cvec3f specularLightColor;
            if (entity.isLightOn(1)) {
                specularLightColor = light1->specularLightColor;
            } else {
                specularLightColor = Cvec3f(0, 0, 0);
            }
            glUniform3f(uSpecularLightColorLoc1, specularLightColor[0], specularLightColor[1], specularLightColor[2]);
        } else {
            glUniform3f(uLightPositionLoc0, 0, 0, 0);
            glUniform3f(uLightColorLoc0, 1.0f, 1.0f, 1.0f);
            glUniform3f(uSpecularLightColorLoc0, 0, 0, 0);
        }

        if (entity.material->hasDiffuseTexture()) {
            glUniform1i(uDiffuseTextureLoc, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, entity.material->getDiffuseTexture());
        }
        
        if (entity.material->hasCubemap()) {
            glUniform1i(uEnvironmentMapLoc, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, entity.material->getCubemapTexture());
        }
        
        entity.geometry->draw(aPositionLoc, aNormalLoc, aTexCoordLoc, -1, -1);
    }

protected:
    void getLocations(int programId) {
        aPositionLoc = glGetAttribLocation(programId, "aPosition");
        aNormalLoc = glGetAttribLocation(programId, "aNormal");
        aTexCoordLoc = glGetAttribLocation(programId, "aTexCoord");
        
        uModelMatrixLoc = glGetUniformLocation(programId, "uModelMatrix");
        uModelViewMatrixLoc = glGetUniformLocation(programId, "uModelViewMatrix");
        uProjectionMatrixLoc = glGetUniformLocation(programId, "uProjectionMatrix");
        uNormalMatrixLoc = glGetUniformLocation(programId, "uNormalMatrix");
        uColorLoc = glGetUniformLocation(programId, "uColor");
        
        uLightPositionLoc0 = glGetUniformLocation(programId, "uLight[0].lightPosition");
        uLightColorLoc0 = glGetUniformLocation(programId, "uLight[0].lightColor");
        uSpecularLightColorLoc0 = glGetUniformLocation(programId, "uLight[0].specularLightColor");
        
        uLightPositionLoc1 = glGetUniformLocation(programId, "uLight[1].lightPosition");
        uLightColorLoc1 = glGetUniformLocation(programId, "uLight[1].lightColor");
        uSpecularLightColorLoc1 = glGetUniformLocation(programId, "uLight[1].specularLightColor");
        
        uDiffuseTextureLoc = glGetUniformLocation(programId, "uDiffuseTexture");

        uEnvironmentMapLoc = glGetUniformLocation(programId, "uEnvironmentMap");
    }

    //attributes
    GLint aPositionLoc;
    GLint aNormalLoc;
    GLint aTexCoordLoc;
    
    //matrix uniforms
    GLint uModelMatrixLoc;
    GLint uModelViewMatrixLoc;
    GLint uProjectionMatrixLoc;
    GLint uNormalMatrixLoc;
    
    //lights uniforms
    GLint uLightPositionLoc0;
    GLint uLightColorLoc0;
    GLint uSpecularLightColorLoc0;
    
    GLint uLightPositionLoc1;
    GLint uLightColorLoc1;
    GLint uSpecularLightColorLoc1;
    
    //texture uniforms
    GLint uDiffuseTextureLoc;
    
    //color uniform
    GLint uColorLoc;
    
    //cubemap
    GLint uEnvironmentMapLoc;
    
    GLfloat modelMat[16];
    GLfloat modelViewMat[16];
    GLfloat projectionMat[16];
    GLfloat normalMat[16];
};

#endif /* TextureShader_h */
