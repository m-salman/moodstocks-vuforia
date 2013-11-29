#include "MSVCallback.h"
#include "MSVController.h"

#include <assert.h>
#include <string.h>

MSVCallback::MSVCallback() :
imgIndex(-1),
currentFrame(NULL),
needUpdate(true),
wasTracking(false),
isNew(false),
isLost(false),
lostCounter(0)
{};

MSVCallback::~MSVCallback() {}

void
MSVCallback::QCAR_onUpdate(QCAR::State &state)
{
  // Store frame
  if (needUpdate) {
    needUpdate = false;
    isNew = false;
    isLost = false;
    if (imgIndex < 0) {
      for (int i = 0; i < state.getFrame().getNumImages(); ++i) {
        if (state.getFrame().getImage(i)->getFormat() == QCAR::GRAYSCALE) {
          imgIndex = i;
          break;
        }
      }
    }
    currentFrame = state.getFrame().getImage(imgIndex);
    if (MSVController::isTracking()) {
      if (!wasTracking) {
        wasTracking = true;
        isNew = true;
        lostCounter = 0;
      }
      else {
        if (MSVController::currentTargetFound(state) < 0) {
          if (lostCounter >= 0) lostCounter++;
          if (lostCounter > LOST_FRAMES_TOL || lostCounter < 0) isLost = true;
        }
        else lostCounter = -1;
      }
    }
    else {
      if (wasTracking) wasTracking = false;
    }
    // Callback
    onStatusUpdate();
    // Reset currentFrame to avoid calling it outside of onStatusUpdate
    currentFrame = NULL;
  }
}

void
MSVCallback::requireUpdate()
{
  needUpdate = true;
}

bool
MSVCallback::getFrame() const
{
  if (currentFrame) {
    getFrame(currentFrame);
    return true;
  }
  return false;
}

bool
MSVCallback::isNewTarget() const
{
  return isNew;
}

bool
MSVCallback::isTargetLost() const
{
  return isLost;
}
