#ifndef MSV_SHADERS_H
#define MSV_SHADERS_H

#include "MSVWhichOS.h"

/** Standard vertex and fragment shaders */

static const char* vertexShader = "\
\
attribute vec4 vertexPosition; \n\
attribute vec4 vertexNormal; \n\
attribute vec4 vertexTexCoord; \n\
\n\
varying vec4 texCoord; \n\
varying vec4 normal; \n\
\n\
uniform mat4 modelViewProjectionMatrix; \n\
uniform mat4 texCoordTransformMatrix; \n\
\n\
void main() \n\
{ \n\
   gl_Position = modelViewProjectionMatrix * vertexPosition; \n\
   normal = vertexNormal; \n\
   texCoord = texCoordTransformMatrix * vertexTexCoord; \n\
} \
";


static const char* fragmentShader = "\
\
precision mediump float; \n\
\n\
varying vec4 texCoord; \n\
varying vec4 normal; \n\
\n\
uniform sampler2D texSampler2D; \n\
\n\
void main() \n\
{ \n\
   gl_FragColor = texture2DProj(texSampler2D, texCoord); \n\
} \n\
";

#if (!defined(__MSV_SYS_IOS__))

static const char* dynamicFragmentShader = "\
\
#extension GL_OES_EGL_image_external : require \n\
\n\
precision mediump float; \n\
\n\
varying vec4 texCoord; \n\
\n\
uniform samplerExternalOES texSamplerOES; \n\
\n\
void main() \n\
{ \n\
   gl_FragColor = texture2DProj(texSamplerOES, texCoord); \n\
} \
";

#endif

#endif
