#ifndef MSV_TEXTURE_H
#define MSV_TEXTURE_H

#include "MSVWhichOS.h"

#if (defined(__MSV_SYS_IOS__))
  #include <OpenGLES/ES2/gl.h>
  #include <OpenGLES/ES2/glext.h>
#else
  #include <GLES2/gl2.h>
  #include <GLES2/gl2ext.h>
#endif

/** Class representing a texture */
class MSVTexture
{
  public:
    MSVTexture(unsigned char *pixels,
              unsigned int width,
              unsigned int height,
              unsigned int channelCount = 4 /* only supported value for now! */);
    virtual ~MSVTexture();
    unsigned int getWidth() const;
    unsigned int getHeight() const;
    unsigned int getChannelCount() const;
    const unsigned char *getPixels() const;

    /** Attaches the texture to an OpenGL texture name
     * that can be reused to bind the texture to GL_TEXTURE_2D.
     */
    GLuint glTextureName();

    /** Generates a 64x64 transparent texture */
    static MSVTexture *getTransparentTexture();

  protected:
    MSVTexture();
    void set(unsigned char *pixels,
             unsigned int width,
             unsigned int height,
             unsigned int channelCount = 4 /* only supported value for now! */);

  private:
    unsigned int width;
    unsigned int height;
    unsigned int channelCount;
    unsigned char *pixels;
    GLuint glName;
    bool hasGlName;
};


#endif
