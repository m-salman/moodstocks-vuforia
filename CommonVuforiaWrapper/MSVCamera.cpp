#include "MSVCamera.h"
#include "MSVState.h"

#include <QCAR/CameraDevice.h>

bool
MSVCamera::start()
{
  // Initialize the camera:
  if (!QCAR::CameraDevice::getInstance().init()) return false;
  // Set video mode
  if (!QCAR::CameraDevice::getInstance().selectVideoMode(CAM_QUALITY)) return false;
  // Check that the frame resolution fits the Moodstocks SDK requirements, i.e.
  // its largest dimensions is >= 480 pixels.
  QCAR::VideoMode mode = QCAR::CameraDevice::getInstance().getVideoMode(CAM_QUALITY);
  if (!(mode.mWidth >= 480 || mode.mHeight >= 480)) return false;
  // Start the camera:
  if (!QCAR::CameraDevice::getInstance().start()) return false;
  if (!QCAR::CameraDevice::getInstance().setFocusMode(QCAR::CameraDevice::FOCUS_MODE_CONTINUOUSAUTO)) {
    QCAR::CameraDevice::getInstance().setFocusMode(QCAR::CameraDevice::FOCUS_MODE_NORMAL);
  }
  return true;
}

void
MSVCamera::stop()
{
  QCAR::CameraDevice::getInstance().stop();
  QCAR::CameraDevice::getInstance().deinit();
}
