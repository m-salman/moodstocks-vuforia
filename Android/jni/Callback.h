#ifndef JNI_CALLBACK_H
#define JNI_CALLBACK_H

#include <jni.h>

#include <MSVCallback.h>

/** Android-specific implementation of the MSCallback class */
class Callback : public MSVCallback
{
  public:
    Callback(JNIEnv *env, jobject obj);
    ~Callback();

  protected:
    void onStatusUpdate();
    void getFrame(const QCAR::Image *frame) const;

  private:
    jobject jcb;
    jmethodID onStatusUpdateID;
    jfieldID pixelsID;
    jfieldID widthID;
    jfieldID heightID;
    jfieldID strideID;
};

#endif
