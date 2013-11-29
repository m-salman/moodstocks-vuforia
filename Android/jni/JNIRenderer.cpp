#include <jni.h>
#include <string.h>

#include <MSVController.h>
#include <MSVRenderer.h>
#include <MSVState.h>

/** JNI communication layer between the Java and C++ Renderer objects */

#ifdef __cplusplus
extern "C"
{
#endif

JNIEXPORT void JNICALL
Java_com_moodstocks_vuforia_core_Renderer_renderFrame(JNIEnv *,
                                                      jobject)
{
  MSVController::getRenderer()->renderFrame();
}

JNIEXPORT void JNICALL
Java_com_moodstocks_vuforia_core_Renderer_initRendering(JNIEnv*,
                                                        jobject)
{
  MSVController::initRenderer();
}

JNIEXPORT void JNICALL
Java_com_moodstocks_vuforia_core_Renderer_updateRendering(JNIEnv*,
                                                          jobject,
                                                          jint width,
                                                          jint height,
                                                          jboolean portrait)
{
  // Update screen dimensions
  MSVState::setGLViewSize(width, height);
  MSVState::setPortrait(portrait);
  // Update the renderer
  MSVController::getRenderer()->updateState();
}

#ifdef __cplusplus
}
#endif
