#ifndef JNI_TEXTURECALLBACK_H
#define JNI_TEXTURECALLBACK_H

#include <jni.h>

#include <MSVTextureCallback.h>

class TextureCallback : public MSVTextureCallback
{
  public:
    TextureCallback(JNIEnv *env, jobject obj);
    ~TextureCallback();
    GLuint getTexture(float mtx[16]);
    void stop();
  private:
    jobject jcb;
    jmethodID getTexID;
    jmethodID stopID;
    jfloatArray transform;
};

#endif
