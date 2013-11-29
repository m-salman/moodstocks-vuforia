#include "Callback.h"
#include "EnvStorage.h"

/** Initializes the field and method IDs required to communicate
 * with the Java code.
 */
Callback::Callback(JNIEnv *env,
                   jobject obj)
{
  jclass cls = env->GetObjectClass(obj);
  this->jcb = env->NewWeakGlobalRef(obj);
  this->onStatusUpdateID = env->GetMethodID(cls, "onStatusUpdate", "()V");
  this->pixelsID = env->GetFieldID(cls, "pixels", "[B");
  this->widthID = env->GetFieldID(cls, "width", "I");
  this->heightID = env->GetFieldID(cls, "height", "I");
  this->strideID = env->GetFieldID(cls, "stride", "I");
}

Callback::~Callback()
{
  JNIEnv *env = EnvStorage::getJNIEnv();
  env->DeleteWeakGlobalRef(this->jcb);
}

/** Calls Java VuforiaCallback.onStatusUpdate() method */
void
Callback::onStatusUpdate()
{
  JNIEnv *env = EnvStorage::getJNIEnv();
  jobject local = env->NewLocalRef(jcb);
  if (!env->IsSameObject(local, NULL)) {
    env->CallVoidMethod(local, onStatusUpdateID);
  }
  env->DeleteLocalRef(local);
}

/** Transfers the camera frame data to the Java part of the code */
void
Callback::getFrame(const QCAR::Image *frame) const
{
  JNIEnv *env = EnvStorage::getJNIEnv();
  jobject local = env->NewLocalRef(jcb);
  if (!env->IsSameObject(local, NULL)) {
    const void *pixels = frame->getPixels();
    int width = frame->getWidth();
    int height = frame->getHeight();
    int stride = frame->getStride();
    jbyteArray jpixels = env->NewByteArray(height*stride);
    env->SetByteArrayRegion(jpixels, 0, height*stride, (const jbyte*)pixels);
    env->SetObjectField(local, pixelsID, jpixels);
    env->SetIntField(local, widthID, width);
    env->SetIntField(local, heightID, height);
    env->SetIntField(local, strideID, stride);
  }
  env->DeleteLocalRef(local);
}


