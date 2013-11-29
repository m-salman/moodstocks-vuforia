#include "Texture.h"

#include <stdlib.h>

Texture::Texture(JNIEnv *env, jobject jtex) :
MSVTexture()
{
  // Handle to the Texture class:
  jclass textureClass = env->GetObjectClass(jtex);

  // Get field IDs:
  jfieldID widthID = env->GetFieldID(textureClass, "width", "I");
  jfieldID heightID = env->GetFieldID(textureClass, "height", "I");
  jfieldID chanID = env->GetFieldID(textureClass, "channelCount", "I");
  jfieldID dataID = env->GetFieldID(textureClass, "data", "[B");

  // Get fields
  int width = env->GetIntField(jtex, widthID);
  int height = env->GetIntField(jtex, heightID);
  int channelCount = env->GetIntField(jtex, chanID);
  jobject data = env->GetObjectField(jtex, dataID);
  jbyteArray pixelBuffer = reinterpret_cast<jbyteArray>(data);
  jboolean isCopy;
  jbyte *pixels = env->GetByteArrayElements(pixelBuffer, &isCopy);

  set((unsigned char *) pixels, width, height, channelCount);
  env->ReleaseByteArrayElements(pixelBuffer, pixels, 0);
}

