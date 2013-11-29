#ifndef MSV_STATE_H
#define MSV_STATE_H

#define MODEL_SIZE      2.0
#define CAM_QUALITY     QCAR::CameraDevice::MODE_OPTIMIZE_QUALITY
#define HOMOG_THRES     0.1

/** Class storing application state values */
class MSVState {
  public:
    // Getters and setters for screen dimensions and device orientation
    static void setGLViewSize(int w, int h);
    static void getGLViewSize(int *w, int *h);
    static void setPortrait(bool p);
    static bool isPortrait();

    private:
      static int glWidth;
      static int glHeight;
      static bool portrait;
};

#endif
