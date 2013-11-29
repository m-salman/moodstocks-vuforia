#ifndef MSV_CAMERA_H
#define MSV_CAMERA_H

/** Helper class around QCAR::CameraDevice */
class MSVCamera {
  public:
    static bool start();
    static void stop();
};

#endif
