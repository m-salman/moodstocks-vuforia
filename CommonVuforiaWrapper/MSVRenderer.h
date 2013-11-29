#ifndef MSV_RENDERER_H
#define MSV_RENDERER_H

#include "MSVWhichOS.h"

#if (defined(__MSV_SYS_IOS__))
  #include <OpenGLES/ES2/gl.h>
  #include <OpenGLES/ES2/glext.h>
#else
  #include <GLES2/gl2.h>
  #include <GLES2/gl2ext.h>
#endif

#include <QCAR/Tool.h>

/** Class in charge of rendering the camera background and the potential
 * tracked targets.
 */
class MSVRenderer {

  public:
    MSVRenderer();
    /** Tool method: get a new, valid, unused OpenGL texture ID.
     * @return an OpenGL texture ID obtained with `glGenTexture` if rendering
     * has started, 0 otherwise.
     */
    GLuint obtainTextureID();
    /** Render */
    void renderFrame();
    /** Updates to latest changes in MSVState */
    void updateState();

  private:
#if(!defined(__MSV_SYS_IOS__)) // Android specific OpenGL data for dynamic models
    unsigned int dynamicShaderProgramID;
    GLint dynamicVertexHandle;
    GLint dynamicNormalHandle;
    GLint dynamicTextureCoordHandle;
    GLint dynamicMvpMatrixHandle;
    GLint dynamicTexCoordTransformHandle;
    GLint dynamicTexSamplerOESHandle;
#endif
    unsigned int shaderProgramID;
    GLint vertexHandle;
    GLint normalHandle;
    GLint textureCoordHandle;
    GLint mvpMatrixHandle;
    GLint texCoordTransformHandle;
    GLint texSampler2DHandle;
    QCAR::Matrix44F projectionMatrix;
    GLuint nextTextureID;
    void setProjectionMatrix();
    void configureVideoBackground();
    static void scalePoseMatrix(float x, float y, float z, float* nMatrix = NULL);
    static void multiplyMatrix(float *matrixA, float *matrixB, float *matrixC);
    static unsigned int initShader(unsigned int shaderType, const char* source);
    static unsigned int createProgramFromBuffer(const char* vertexShaderBuffer,
                                                const char* fragmentShaderBuffer);
};

#endif
