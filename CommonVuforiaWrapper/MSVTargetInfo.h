#ifndef MSV_TARGETINFO_H
#define MSV_TARGETINFO_H

#include "MSVTextureCallback.h"

class MSVTexture;
class MSVMesh;

/** Class in charge of handling all the necessary information about a target. */
class MSVTargetInfo {
  public:
    MSVTargetInfo(const char *n,
                  const int *d);
    ~MSVTargetInfo();
    // General info
    const char *getName() const;
    int getWidth() const;
    int getHeight() const;
    void getScale(float s[3]) const;
    void changeScale(const float s[3]);
    MSVMesh *getMesh() const;
    bool isDynamicTarget() const;
    // Static target: displays mesh + texture
    void setStatic(MSVMesh *m, MSVTexture *t);
    MSVTexture *getStaticTexture() const;
    // Dynamic target: displays plane + dynamic texture
    void setDynamic(MSVTextureCallback *callback);
    MSVTextureCallback *getDynamicTextureCallback() const;

  private:
    /* members */
    char *name;
    int *dims;
    float *scale;
    MSVMesh *mesh;
    bool dynamicTarget;
    MSVTexture *tex;
    MSVTextureCallback *cb;
};

#endif
