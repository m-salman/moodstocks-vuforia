#ifndef JNI_MESH_H
#define JNI_MESH_H

#include <jni.h>

#include <MSVMesh.h>

/** Scaffold of a class in charge of transforming a Java mesh object
 * to a C++ one. Not implemented yet, as there is no out-of-the-box
 * 3D model gestion in Vuforia or Android.
 */
class Mesh : public MSVMesh {
  public:
    Mesh(JNIEnv *env, jobject jmesh);
};

#endif
