#include "MSVCallback.h"
#include "MSVController.h"
#include "MSVRenderer.h"
#include "MSVState.h"
#include "MSVTargetInfo.h"
#include "MSVTracker.h"

#include <math.h>
#include <string.h>

#include <QCAR/QCAR.h>
#include <QCAR/TrackableResult.h>

// Initialize static variables
MSVRenderer *MSVController::ms_Renderer = NULL;
MSVTracker *MSVController::ms_Tracker = NULL;
MSVCallback *MSVController::ms_Callback = NULL;

bool MSVController::tracking = false;
MSVTargetInfo *MSVController::currentInfo = NULL;

void
MSVController::init()
{
  MSVController::ms_Tracker  = new MSVTracker();
}

bool
MSVController::registerCallback(MSVCallback *cb)
{
  if (MSVController::ms_Callback) return false;
  MSVController::ms_Callback = cb;
  QCAR::registerCallback(cb);
  return true;
}

MSVCallback *
MSVController::unregisterCallback()
{
  if (!MSVController::ms_Callback) return NULL;
  MSVCallback *cb = MSVController::ms_Callback;
  MSVController::ms_Callback = NULL;
  QCAR::registerCallback(NULL);
  return cb;
}

void
MSVController::initRenderer()
{
  MSVController::ms_Renderer = new MSVRenderer();
}

void
MSVController::deInit()
{
  delete MSVController::ms_Renderer;
  MSVController::ms_Renderer = NULL;
  delete MSVController::ms_Tracker;
  MSVController::ms_Tracker = NULL;
}

MSVRenderer *
MSVController::getRenderer()
{
  return MSVController::ms_Renderer;
}

MSVTracker *
MSVController::getTracker()
{
  return MSVController::ms_Tracker;
}

MSVCallback *
MSVController::getCallback()
{
  return MSVController::ms_Callback;
}

void
MSVController::addDataset(const char *dataset)
{
  ms_Tracker->addDataset(dataset);
}

void
MSVController::startTracking(const char *name,
                             const int dims[2],
                             const char *dataset)
{
  if (tracking)
    goto fail;
  if (!ms_Tracker->has(name, dataset))
    goto fail;
  tracking = true;
  if (currentInfo) delete currentInfo;
  currentInfo = new MSVTargetInfo(name, dims);
  ms_Tracker->start(dataset);
  return;
fail:
  stopTracking();
}

void
MSVController::stopTracking()
{
  tracking = false;
  if (currentInfo) {
    if (currentInfo->isDynamicTarget()) {
      currentInfo->getDynamicTextureCallback()->stop();
    }
    delete currentInfo;
  }
  currentInfo = NULL;
  ms_Tracker->stop();
}

void
MSVController::setStaticModel(MSVMesh *mesh,
                              MSVTexture *tex,
                              const float scale[3])
{
  if (!tracking) return;
  if (!currentInfo) return;
  currentInfo->setStatic(mesh, tex);
  currentInfo->changeScale(scale);
}

void
MSVController::setDynamicModel(MSVTextureCallback *cb,
                               const float scale[3])
{
  if (!tracking) return;
  if (!currentInfo) return;
  currentInfo->setDynamic(cb);
  currentInfo->changeScale(scale);
}

const MSVTargetInfo *
MSVController::getCurrentTarget()
{
  return currentInfo;
}

int
MSVController::currentTargetFound(QCAR::State &state)
{
  int result = -1;
  if (!currentInfo) return result;
  for (int i = 0; i < state.getNumTrackableResults(); ++i) {
    if ( !strcmp(state.getTrackableResult(0)->getTrackable().getName(),
                 currentInfo->getName()) ) {
      result = i;
      break;
    }
  }
  return result;
}

bool
MSVController::isTracking()
{
  return tracking;
}

int
MSVController::obtainTextureID()
{
  if (!ms_Renderer) return 0;
  return ms_Renderer->obtainTextureID();
}
