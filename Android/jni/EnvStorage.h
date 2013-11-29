#ifndef JNI_CONTROLLER_H
#define JNI_CONTROLLER_H

#include <jni.h>

/** Android specific class in charge of storing the JVM from which
 * a JNIEnv instance can be obtained from anywhere.
 */
class EnvStorage
{
  public:
    static void setJNIEnv(JNIEnv *env);
    static JNIEnv *getJNIEnv();

  private:
    static JavaVM *jvm;
};


#endif
