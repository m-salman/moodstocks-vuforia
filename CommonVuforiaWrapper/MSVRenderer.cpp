#include "MSVShaders.h"
#include "MSVController.h"
#include "MSVMesh.h"
#include "MSVRenderer.h"
#include "MSVState.h"
#include "MSVTargetInfo.h"
#include "MSVTexture.h"
#include "MSVTextureCallback.h"

#include <QCAR/CameraDevice.h>
#include <QCAR/Renderer.h>
#include <QCAR/TrackableResult.h>
#include <QCAR/VideoBackgroundConfig.h>

// Contructor
MSVRenderer::MSVRenderer() :
#if(!defined(__MSV_SYS_IOS__)) // Android specific OpenGL data for dynamic models
dynamicShaderProgramID(0),
dynamicVertexHandle(0),
dynamicNormalHandle(0),
dynamicTextureCoordHandle(0),
dynamicMvpMatrixHandle(0),
dynamicTexCoordTransformHandle(0),
dynamicTexSamplerOESHandle(0),
#endif
shaderProgramID(0),
vertexHandle(0),
normalHandle(0),
textureCoordHandle(0),
mvpMatrixHandle(0),
texCoordTransformHandle(0),
texSampler2DHandle(0),
nextTextureID(0)
{
  // Define clear color
  glClearColor(0.0f, 0.0f, 0.0f, QCAR::requiresAlpha() ? 0.0f : 1.0f);

  // Initialize OpenGL: shaders, attributes.
  shaderProgramID = MSVRenderer::createProgramFromBuffer(vertexShader,
                                                         fragmentShader);
  vertexHandle = glGetAttribLocation(shaderProgramID, "vertexPosition");
  normalHandle = glGetAttribLocation(shaderProgramID, "vertexNormal");
  textureCoordHandle = glGetAttribLocation(shaderProgramID, "vertexTexCoord");
  mvpMatrixHandle = glGetUniformLocation(shaderProgramID, "modelViewProjectionMatrix");
  texCoordTransformHandle = glGetUniformLocation(shaderProgramID, "texCoordTransformMatrix");
  texSampler2DHandle  = glGetUniformLocation(shaderProgramID, "texSampler2D");
#if (!defined(__MSV_SYS_IOS__))
  dynamicShaderProgramID = MSVRenderer::createProgramFromBuffer(vertexShader,
                                                                dynamicFragmentShader);
  dynamicVertexHandle = glGetAttribLocation(dynamicShaderProgramID, "vertexPosition");
  dynamicNormalHandle = glGetAttribLocation(dynamicShaderProgramID, "vertexNormal");
  dynamicTextureCoordHandle = glGetAttribLocation(dynamicShaderProgramID, "vertexTexCoord");
  dynamicMvpMatrixHandle = glGetUniformLocation(dynamicShaderProgramID, "modelViewProjectionMatrix");
  dynamicTexCoordTransformHandle = glGetUniformLocation(dynamicShaderProgramID, "texCoordTransformMatrix");
  dynamicTexSamplerOESHandle  = glGetUniformLocation(dynamicShaderProgramID, "texSamplerOES");
#endif
}

GLuint
MSVRenderer::obtainTextureID() {
  GLuint texID = nextTextureID;
  nextTextureID = 0;
  return texID;
}

void
MSVRenderer::renderFrame()
{
  // Clear color and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // If needed, generate a new texture ID
  if (!nextTextureID) glGenTextures(1, &nextTextureID);

  // Get the state from QCAR and mark the beginning of a rendering section
  QCAR::State state = QCAR::Renderer::getInstance().begin();

  // Explicitly render the Video Background
  QCAR::Renderer::getInstance().drawVideoBackground();

  int tIdx = MSVController::currentTargetFound(state);
  if (tIdx >= 0) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Get the trackable:
    const QCAR::TrackableResult* trackableResult = state.getTrackableResult(tIdx);
    QCAR::Matrix44F modelViewMatrix =
      QCAR::Tool::convertPose2GLMatrix(trackableResult->getPose());

    // get the target info
    const MSVTargetInfo *info = MSVController::getCurrentTarget();
    MSVMesh *mesh = info->getMesh();
    float scale[3] = {0};
    info->getScale(scale);

    // Dynamic/Static Model specific choices:
    GLuint texID;
    GLenum texTarget;
    GLint vertexH, normalH, textureCoordH, mvpMatrixH, texCoordTransformH, texSamplerH;
    float texCoordTransform[16] = {1, 0, 0, 0,
                                   0, 1, 0, 0,
                                   0, 0, 1, 0,
                                   0, 0, 0, 1};
    unsigned int programID;
    if (info->isDynamicTarget()) {
      texID = info->getDynamicTextureCallback()->getTexture(texCoordTransform);
#if (defined(__MSV_SYS_IOS__))
      // on iOS, use GL_TEXTURE_2D
      texTarget = GL_TEXTURE_2D;
      programID = shaderProgramID;
      vertexH = vertexHandle;
      normalH = normalHandle;
      textureCoordH = textureCoordHandle;
      mvpMatrixH = mvpMatrixHandle;
      texCoordTransformH = texCoordTransformHandle;
      texSamplerH = texSampler2DHandle;
#else
      // on Android, use GL_TEXTURE_EXTERNAL_OES extension
      texTarget = GL_TEXTURE_EXTERNAL_OES;
      programID = dynamicShaderProgramID;
      vertexH = dynamicVertexHandle;
      normalH = dynamicNormalHandle;
      textureCoordH = dynamicTextureCoordHandle;
      mvpMatrixH = dynamicMvpMatrixHandle;
      texCoordTransformH = dynamicTexCoordTransformHandle;
      texSamplerH = dynamicTexSamplerOESHandle;
#endif
    }
    else {
      // static texture: use GL_TEXTURE_2D
      texID = info->getStaticTexture()->glTextureName();
      texTarget = GL_TEXTURE_2D;
      programID = shaderProgramID;
      vertexH = vertexHandle;
      normalH = normalHandle;
      textureCoordH = textureCoordHandle;
      mvpMatrixH = mvpMatrixHandle;
      texCoordTransformH = texCoordTransformHandle;
      texSamplerH = texSampler2DHandle;
    }

    MSVRenderer::scalePoseMatrix(scale[0],
                                 scale[1],
                                 scale[2],
                                 &modelViewMatrix.data[0]);

    QCAR::Matrix44F modelViewProjection;

    MSVRenderer::multiplyMatrix(&projectionMatrix.data[0],
                               &modelViewMatrix.data[0] ,
                               &modelViewProjection.data[0]);

    glUseProgram(programID);

    glVertexAttribPointer(vertexH,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          mesh->getVertices());
    glVertexAttribPointer(normalH,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          mesh->getNormals());
    glVertexAttribPointer(textureCoordH,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          mesh->getTexCoords());

    glEnableVertexAttribArray(vertexH);
    glEnableVertexAttribArray(normalH);
    glEnableVertexAttribArray(textureCoordH);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(texTarget, texID);
    // Allow non-power-of-two textures
    glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glUniformMatrix4fv(mvpMatrixH,
                       1,
                       GL_FALSE,
                       (GLfloat *)modelViewProjection.data);
    glUniformMatrix4fv(texCoordTransformH,
                       1,
                       GL_FALSE,
                       (GLfloat *)texCoordTransform);
    glUniform1i(texSamplerH, 0);
    glDrawElements(GL_TRIANGLES,
                   3*mesh->getFacesCount(),
                   GL_UNSIGNED_SHORT,
                   mesh->getFaces());

    glDisable(GL_DEPTH_TEST);

    glDisableVertexAttribArray(vertexH);
    glDisableVertexAttribArray(normalH);
    glDisableVertexAttribArray(textureCoordH);

    glDisable(GL_BLEND);
  }
  QCAR::Renderer::getInstance().end();
}

void
MSVRenderer::updateState()
{
  setProjectionMatrix();
  configureVideoBackground();
}

void
MSVRenderer::setProjectionMatrix()
{
  // Cache the projection matrix:
  const QCAR::CameraCalibration& cameraCalibration =
  QCAR::CameraDevice::getInstance().getCameraCalibration();
  projectionMatrix = QCAR::Tool::getProjectionGL(cameraCalibration, 0.04f, 50.0f);
}

void
MSVRenderer::configureVideoBackground()
{
  // Get the default video mode:
  QCAR::CameraDevice& cameraDevice = QCAR::CameraDevice::getInstance();
  QCAR::VideoMode videoMode = cameraDevice.getVideoMode(CAM_QUALITY);

  // Configure the video background
  QCAR::VideoBackgroundConfig config;
  config.mEnabled = true;
  config.mSynchronous = true;
  config.mPosition.data[0] = 0.0f;
  config.mPosition.data[1] = 0.0f;

  int glWidth, glHeight;
  MSVState::getGLViewSize(&glWidth, &glHeight);

  if (MSVState::isPortrait())
  {
    config.mSize.data[0] = glHeight * (videoMode.mHeight / (float)videoMode.mWidth);
    config.mSize.data[1] = glHeight;

    if (config.mSize.data[0] < glWidth)
    {
      config.mSize.data[0] = glWidth;
      config.mSize.data[1] = glWidth * (videoMode.mWidth  / (float)videoMode.mHeight);
    }
  }
  else
  {
    config.mSize.data[0] = glWidth;
    config.mSize.data[1] = glWidth * (videoMode.mHeight / (float)videoMode.mWidth);

    if (config.mSize.data[1] < glHeight)
    {
      config.mSize.data[0] = glHeight * (videoMode.mWidth / (float)videoMode.mHeight);
      config.mSize.data[1] = glHeight;
    }
  }

  // Set the config:
  QCAR::Renderer::getInstance().setVideoBackgroundConfig(config);

}

void
MSVRenderer::scalePoseMatrix(float x, float y, float z, float* matrix)
{
  // Sanity check
  if (!matrix)
    return;

  // matrix * scale_matrix
  matrix[0]  *= x;
  matrix[1]  *= x;
  matrix[2]  *= x;
  matrix[3]  *= x;

  matrix[4]  *= y;
  matrix[5]  *= y;
  matrix[6]  *= y;
  matrix[7]  *= y;

  matrix[8]  *= z;
  matrix[9]  *= z;
  matrix[10] *= z;
  matrix[11] *= z;
}


void
MSVRenderer::multiplyMatrix(float *matrixA, float *matrixB, float *matrixC)
{
  int i, j, k;
  float aTmp[16];

  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      aTmp[j * 4 + i] = 0.0;

      for (k = 0; k < 4; k++)
        aTmp[j * 4 + i] += matrixA[k * 4 + i] * matrixB[j * 4 + k];
    }
  }

  for (i = 0; i < 16; i++)
    matrixC[i] = aTmp[i];
}

unsigned int
MSVRenderer::initShader(unsigned int shaderType, const char* source)
{
  GLuint shader = glCreateShader((GLenum)shaderType);
  if (shader)
  {
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
      GLint infoLen = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
      if (infoLen)
      {
        char* buf = (char*) malloc(infoLen);
        if (buf)
        {
          glGetShaderInfoLog(shader, infoLen, NULL, buf);
          free(buf);
        }
        glDeleteShader(shader);
        shader = 0;
      }
    }
  }
  return shader;
}


unsigned int
MSVRenderer::createProgramFromBuffer(const char* vertexShaderBuffer,
                   const char* fragmentShaderBuffer)
{
  GLuint vertexShader = initShader(GL_VERTEX_SHADER, vertexShaderBuffer);
  if (!vertexShader)
    return 0;

  GLuint fragmentShader = initShader(GL_FRAGMENT_SHADER,
                    fragmentShaderBuffer);
  if (!fragmentShader)
    return 0;

  GLuint program = glCreateProgram();
  if (program)
  {
    glAttachShader(program, vertexShader);

    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

    if (linkStatus != GL_TRUE)
    {
      GLint bufLength = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
      if (bufLength)
      {
        char* buf = (char*) malloc(bufLength);
        if (buf)
        {
          glGetProgramInfoLog(program, bufLength, NULL, buf);
          free(buf);
        }
      }
      glDeleteProgram(program);
      program = 0;
    }
  }
  return program;
}
