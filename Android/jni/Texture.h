#ifndef JNI_TEXTURE_H
#define JNI_TEXTURE_H

#include <jni.h>

#include <MSVTexture.h>

/** Android-specific implementation of the MSVTexture class. */
class Texture : public MSVTexture {
  public:
    /** Build a new MSTexture from a Java Texture object */
    Texture(JNIEnv *env, jobject jtex);
};

#endif
