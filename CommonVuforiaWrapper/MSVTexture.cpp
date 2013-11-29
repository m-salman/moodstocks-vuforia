#include "MSVTexture.h"

#include <string.h>

MSVTexture::MSVTexture() :
width(0),
height(0),
channelCount(0),
pixels(NULL),
glName(0),
hasGlName(false)
{}

MSVTexture::MSVTexture(unsigned char *pixels,
                     unsigned int width,
                     unsigned int height,
                     unsigned int channelCount /* 4 */) :
glName(0),
hasGlName(false)
{
  set(pixels, width, height, channelCount);
}

void
MSVTexture::set(unsigned char *pixels,
               unsigned int width,
               unsigned int height,
               unsigned int channelCount)
{
  this->width = width;
  this->height = height;
  this->channelCount = channelCount;
  this->pixels = new unsigned char[width*height*channelCount];
  int rowSize = width * channelCount;
  for (int r = 0; r < height; ++r)
  {
    memcpy(this->pixels + rowSize * r, pixels + rowSize * (height - 1 - r), rowSize);
  }
}

MSVTexture::~MSVTexture()
{
  if (pixels) delete [] pixels;
}

unsigned int
MSVTexture::getWidth() const
{
  return width;
}

unsigned int
MSVTexture::getHeight() const
{
  return height;
}

unsigned int
MSVTexture::getChannelCount() const
{
  return channelCount;
}

const unsigned char *
MSVTexture::getPixels() const
{
  return pixels;
}

MSVTexture *
MSVTexture::getTransparentTexture()
{
  MSVTexture *tex = new MSVTexture();
  tex->width = 64;
  tex->height = 64;
  tex->channelCount = 4;
  tex->pixels = new unsigned char[64*64*4]();
  return tex;
}

GLuint
MSVTexture::glTextureName()
{
  if (!hasGlName) {
    glGenTextures(1, &glName);
    glBindTexture(GL_TEXTURE_2D, glName);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width,
                 height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 (GLvoid*) pixels);
    hasGlName = true;
  }
  return glName;
}



