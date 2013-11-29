#include "Callback.h"
#include "EnvStorage.h"
#include "Mesh.h"
#include "Texture.h"
#include "TextureCallback.h"

#include <jni.h>
#include <string.h>

#include <MSVCamera.h>
#include <MSVController.h>
#include <MSVTargetInfo.h>

/** JNI communication layer between the Java and C++ Controller objects */

#ifdef __cplusplus
extern "C"
{
#endif

static jclass     targetClass;
static jmethodID  ctorID;
static jfieldID   nameID;
static jfieldID   widthID;
static jfieldID   heightID;

void getJavaTarget(JNIEnv *env,
                   jobject jtarget,
                   char **name,
                   int dims[2]);

void
Java_com_moodstocks_vuforia_core_VuforiaController_initNative(JNIEnv *env,
                                                              jobject jself)
{
  MSVController::init();
  MSVCallback *cb = new Callback(env, jself);
  MSVController::registerCallback(cb);
  jclass cls = env->FindClass("com/moodstocks/vuforia/core/Target");
  targetClass = (jclass) env->NewGlobalRef(cls);
  ctorID = env->GetMethodID(cls, "<init>", "(Ljava/lang/String;[I)V");
  nameID = env->GetFieldID(cls, "name", "Ljava/lang/String;");
  widthID = env->GetFieldID(cls, "width", "I");
  heightID = env->GetFieldID(cls, "height", "I");
  EnvStorage::setJNIEnv(env);
}

void
Java_com_moodstocks_vuforia_core_VuforiaController_deInitNative(JNIEnv *env,
                                                                jobject)
{
  delete MSVController::unregisterCallback();
  MSVController::deInit();
  env->DeleteGlobalRef(targetClass);
}

bool
Java_com_moodstocks_vuforia_core_VuforiaController_startCameraNative(JNIEnv *,
                                                                     jobject)
{
  return MSVCamera::start();
}

void
Java_com_moodstocks_vuforia_core_VuforiaController_stopCameraNative(JNIEnv *,
                                                                    jobject)
{
  // Stop the tracker if needed
  MSVController::stopTracking();
  // Stop the camera
  MSVCamera::stop();
}

void
Java_com_moodstocks_vuforia_core_VuforiaController_addDatasetNative(JNIEnv *env,
                                                                    jobject,
                                                                    jstring jname) {
  const char *name = env->GetStringUTFChars(jname, NULL);
  MSVController::addDataset(name);
  env->ReleaseStringUTFChars(jname, name);
}

void
Java_com_moodstocks_vuforia_core_VuforiaController_startTracking(JNIEnv *env,
                                                                 jobject,
                                                                 jobject jtarget,
                                                                 jstring jdataset)
{
  const char *dataset = env->GetStringUTFChars(jdataset, NULL);
  char *name = NULL;
  int dims[2];
  getJavaTarget(env, jtarget, &name, dims);
  MSVController::startTracking(name, dims, dataset);
  free(name);
  env->ReleaseStringUTFChars(jdataset, dataset);
}

void
Java_com_moodstocks_vuforia_core_VuforiaController_stopTracking(JNIEnv *,
                                                                jobject)
{
  MSVController::stopTracking();
}

jboolean
Java_com_moodstocks_vuforia_core_VuforiaController_isTracking(JNIEnv *,
                                                              jobject)
{
  return MSVController::isTracking() ? JNI_TRUE : JNI_FALSE;
}

jobject
Java_com_moodstocks_vuforia_core_VuforiaController_getCurrentTarget(JNIEnv *env,
                                                                    jobject)
{
  const MSVTargetInfo *i = MSVController::getCurrentTarget();
  if (!i) return NULL;
  jstring jname = env->NewStringUTF(i->getName());
  int dims[2] = {i->getWidth(), i->getHeight()};
  jintArray jdims = env->NewIntArray(2);
  env->SetIntArrayRegion(jdims, 0, 2, dims);
  return env->NewObject(targetClass, ctorID, jname, jdims, NULL);
}

void
Java_com_moodstocks_vuforia_core_VuforiaController_setStaticModel(JNIEnv *env,
                                                                  jobject,
                                                                  jobject jmesh,
                                                                  jobject jtex,
                                                                  jfloatArray jscale)
{
  Mesh *m = NULL;
  if (!env->IsSameObject(jmesh, NULL))
    m = new Mesh(env, jmesh);
  Texture *t = NULL;
  if (!env->IsSameObject(jtex, NULL))
    t = new Texture(env, jtex);
  float *scale = env->GetFloatArrayElements(jscale, NULL);
  MSVController::setStaticModel(m, t, scale);
  env->ReleaseFloatArrayElements(jscale, scale, JNI_ABORT);
}

void
Java_com_moodstocks_vuforia_core_VuforiaController_setDynamicModel(JNIEnv *env,
                                                                   jobject,
                                                                   jobject jcb,
                                                                   jfloatArray jscale)
{
  MSVTextureCallback *cb = new TextureCallback(env, jcb);
  float *scale = env->GetFloatArrayElements(jscale, NULL);
  MSVController::setDynamicModel(cb, scale);
  env->ReleaseFloatArrayElements(jscale, scale, JNI_ABORT);
}

void
Java_com_moodstocks_vuforia_core_VuforiaController_requireUpdate(JNIEnv *env,
                                                               jobject)
{
  MSVController::getCallback()->requireUpdate();
}

bool
Java_com_moodstocks_vuforia_core_VuforiaController_getFrame(JNIEnv *env,
                                                          jobject)
{
  return MSVController::getCallback()->getFrame() ? JNI_TRUE : JNI_FALSE;
}

bool
Java_com_moodstocks_vuforia_core_VuforiaController_isNewTarget(JNIEnv *env,
                                                             jobject)
{
  return MSVController::getCallback()->isNewTarget() ? JNI_TRUE : JNI_FALSE;
}

bool
Java_com_moodstocks_vuforia_core_VuforiaController_isTargetLost(JNIEnv *env,
                                                              jobject)
{
  return MSVController::getCallback()->isTargetLost() ? JNI_TRUE : JNI_FALSE;
}

int
Java_com_moodstocks_vuforia_core_VuforiaController_obtainTextureID(JNIEnv *,
                                                                   jobject)
{
  return MSVController::obtainTextureID();
}


void
getJavaTarget(JNIEnv *env,
              jobject jtarget,
              char **name,
              int dims[2])
{
  if (env->IsSameObject(jtarget, NULL)) {
    if (name) *name = NULL;
    return;
  }
  if (name) {
    jstring jname = (jstring)env->GetObjectField(jtarget, nameID);
    const char *tmp = env->GetStringUTFChars(jname, NULL);
    *name = strdup(tmp);
    env->ReleaseStringUTFChars(jname, tmp);
  }
  if (dims) {
    dims[0] = env->GetIntField(jtarget, widthID);
    dims[1] = env->GetIntField(jtarget, heightID);
  }
}


#ifdef __cplusplus
}
#endif

