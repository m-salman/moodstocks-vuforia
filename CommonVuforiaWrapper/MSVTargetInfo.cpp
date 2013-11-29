#include "MSVMesh.h"
#include "MSVTargetInfo.h"
#include "MSVTexture.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

static const float noScale[3] = {1, 1, 1};

MSVTargetInfo::MSVTargetInfo(const char *n,
                             const int *d) :
dynamicTarget(false),
cb(NULL)
{
  name = strdup(n);

  dims = new int[2];
  memcpy(dims, d, 2*sizeof(int));

  tex = MSVTexture::getTransparentTexture();

  mesh = MSVMesh::getNormalizedPlane();

  scale = new float[3];
  memcpy(scale, noScale, 3*sizeof(float));
}

MSVTargetInfo::~MSVTargetInfo()
{
  if (name) free(name);
  if (dims) delete dims;
  if (scale) delete scale;
  if (tex) delete tex;
  if (mesh) delete mesh;
  if (cb) delete cb;
}

const char *
MSVTargetInfo::getName() const
{
  return name;
}

int
MSVTargetInfo::getWidth() const
{
  return dims[0];
}

int
MSVTargetInfo::getHeight() const
{
  return dims[1];
}

void
MSVTargetInfo::getScale(float s[3]) const
{
  memcpy(s, scale, 3*sizeof(float));
}

void
MSVTargetInfo::changeScale(const float s[3])
{
  if (!scale) scale = new float[3]();
  memcpy(scale, s, 3*sizeof(float));
}

MSVMesh *
MSVTargetInfo::getMesh() const
{
  return mesh;
}

bool
MSVTargetInfo::isDynamicTarget() const {
  return dynamicTarget;
}

void
MSVTargetInfo::setStatic(MSVMesh *m, MSVTexture *t)
{
  if (cb)
    cb = NULL;
  if (mesh)
    delete mesh;
  if (m)
    mesh = m;
  else
    mesh = MSVMesh::getNormalizedPlane();
  if (tex)
    delete tex;
  if (t)
    tex = t;
  else
    tex = MSVTexture::getTransparentTexture();
  dynamicTarget = false;
}

MSVTexture *
MSVTargetInfo::getStaticTexture() const
{
  return tex;
}

void
MSVTargetInfo::setDynamic(MSVTextureCallback *callback)
{
  cb = callback;
  if (mesh)
    delete mesh;
  mesh = MSVMesh::getNormalizedPlane();
  if (tex)
    delete tex;
  tex = NULL;
  dynamicTarget = true;
}

MSVTextureCallback *
MSVTargetInfo::getDynamicTextureCallback() const
{
  return cb;
}
