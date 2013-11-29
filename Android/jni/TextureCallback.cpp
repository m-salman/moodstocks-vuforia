#include "TextureCallback.h"
#include "EnvStorage.h"
#include <stdlib.h>

TextureCallback::TextureCallback(JNIEnv *env,
                                 jobject obj)
{
  jclass cls = env->GetObjectClass(obj);
  this->jcb = env->NewWeakGlobalRef(obj);
  this->getTexID = env->GetMethodID(cls, "getTexture", "([F)I");
  this->stopID = env->GetMethodID(cls, "stop", "()V");
  jfloatArray t = env->NewFloatArray(16);
  this->transform = (jfloatArray) env->NewGlobalRef(t);
  env->DeleteLocalRef(t);
}

TextureCallback::~TextureCallback()
{
  JNIEnv *env = EnvStorage::getJNIEnv();
  env->DeleteWeakGlobalRef(this->jcb);
  env->DeleteGlobalRef(this->transform);
}

GLuint
TextureCallback::getTexture(float mtx[16])
{
  GLuint texID = 0;
  JNIEnv *env = EnvStorage::getJNIEnv();
  jobject local = env->NewLocalRef(jcb);
  if (!env->IsSameObject(local, NULL)) {
    env->SetFloatArrayRegion(transform, 0, 16, mtx);
    texID = env->CallIntMethod(local, getTexID, this->transform);
    env->GetFloatArrayRegion(transform, 0, 16, mtx);
  }
  env->DeleteLocalRef(local);
  return texID;
}

void
TextureCallback::stop()
{
  JNIEnv *env = EnvStorage::getJNIEnv();
  jobject local = env->NewLocalRef(jcb);
  if (!env->IsSameObject(local, NULL)) {
    env->CallVoidMethod(local, stopID);
  }
  env->DeleteLocalRef(local);
}
