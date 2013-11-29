#ifndef MSV_TEXTURE_CB_H
#define MSV_TEXTURE_CB_H

#include "MSVWhichOS.h"

#if (defined(__MSV_SYS_IOS__))
  #include <OpenGLES/ES2/gl.h>
  #include <OpenGLES/ES2/glext.h>
#else
  #include <GLES2/gl2.h>
  #include <GLES2/gl2ext.h>
#endif

/** Abstract class allowing to get a new texture at each rendering frame */
class MSVTextureCallback {
  public:
    MSVTextureCallback();
    virtual ~MSVTextureCallback();
    /** Will be called at each rendering frame to get the OpenGL texture ID
     * of the texture to display.
     * @param mtx the 4x4 transformation matrix to apply to the texture
     * coordinates.  This transform matrix maps 2D homogeneous texture coordinates of
     * the form (s, t, 0, 1) with s and t in the inclusive range [0, 1] to the texture
     * coordinate that should be used to sample that location from the texture.
     * By default, it is filled with the identity. If you wish to modify it, you should
     * fill it with the desired 4x4 matrix in **colum-major** order.
     * @return the OpenGL texture ID to display. On iOS, it should correspond
     * to a GL_TEXTURE_2D texture containing the current frame to display.
     * On Android, it should correspond to a GL_TEXTURE_EXTERNAL_OES texture
     * used to build the `SurfaceTexture` object displaying the video.
     */
    virtual GLuint getTexture(float mtx[16]) = 0;
    /** Will be called when tracking stops. Note that the destructor will be
     * called soon after this call.
     */
    virtual void stop() = 0;
};


#endif
