#include "EnvStorage.h"

#include <stdlib.h>

JavaVM *EnvStorage::jvm = NULL;

void
EnvStorage::setJNIEnv(JNIEnv *env)
{
  env->GetJavaVM(&EnvStorage::jvm);
}

JNIEnv *
EnvStorage::getJNIEnv()
{
  if (EnvStorage::jvm == NULL) return NULL;
  JNIEnv* env;
  if (EnvStorage::jvm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK)
  {
    return NULL;
  }
  return env;
}
