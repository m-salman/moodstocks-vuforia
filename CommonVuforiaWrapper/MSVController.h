#ifndef MSV_CONTROLLER_H
#define MSV_CONTROLLER_H

#include <stdlib.h>

#include <QCAR/State.h>

class MSVRenderer;
class MSVTracker;
class MSVTargetInfo;
class MSVCallback;
class MSVMesh;
class MSVTexture;
class MSVTextureCallback;

/** Class controlling the Vuforia SDK */
class MSVController {

  public:
    /** Initializes the non-rendering-related components */
    static void init();

    /** Sets the callback.
     * A callback registered using this method should be unregistered before
     * deInit is called, or before setting another callback.
     * @param cb the MSVCallback to use.
     * @return true if the callback could be registered, false if there is
     * already a registered callback preventing the use of this one.
     */
    static bool registerCallback(MSVCallback *cb);

    /** Unregisters the current callback, if any.
     * @return the unregistered callback, so that the caller can delete it if
     * needed.
     */
    static MSVCallback *unregisterCallback();

    /** Initializes the renderer. Must be called from GL thread */
    static void initRenderer();

    /** De-initializes all the components */
    static void deInit();

    /** Adds a bundled dataset produced with the Vuforia Target Manager to
     * the trackable datasets.
     * @param dataset the name of the dataset to add, corresponding to the
     *   `.dat` and `.xml` files generated using the Vuforia Target Manager.
     *   The dataset is loaded using STORAGE_APPRESOURCE flag, so these files
     *   should be bundled with the app.
     */
    static void addDataset(const char *dataset);

    /** Asks the Vuforia SDK to start tracking a target bundled in a dataset.
     * Should only be called if the Vuforia SDK is not already tracking a target;
     * otherwise it will stop the current tracking in order for the next call to
     * this function to work.
     * @param name the target name **as defined in the Vuforia XML file**.
     * @param dims the target dimensions.
     * @param dataset the name of the dataset to use, corresponding to the
     *   `.dat` and `.xml` files generated using the Vuforia Target Manager.
     */
    static void startTracking(const char *name,
                              const int dims[2],
                              const char *dataset);


    /** Resets the MSVController so that the Vuforia SDK does
     * nothing except providing camera frames through its
     * callback.
     */
    static void stopTracking();

    /** Ask whether the Vuforia SDK is currently in tracking mode
     * @return true if the Vuforia SDK is tracking, false otherwise.
     */
    static bool isTracking();

    /** Returns the MSVTargetInfo object corresponding to the target
     * currently being tracked, if any.
     */
    static const MSVTargetInfo *getCurrentTarget();

    /** Changes the currently displayed model to a static mesh and texture.
     * @param mesh the new MSVMesh to use, or NULL to use a plane. Its ownership
     * is transferred to the MSVController.
     * @param tex the new Texture to use. Its ownership is transferred to the
     * MSVController.
     * @param scale the scaling to apply to the mesh and texture at rendering
     *   time, formatted as {scale_x, scale_y, scale_z}.
     */
    static void setStaticModel(MSVMesh *mesh,
                               MSVTexture *tex,
                               const float scale[3]);

    /** Changes the currently displayed model to a plane with dynamic texture (for
     * example for video playback).
     * @param cb the `MSVTextureCallback` object to call for each frame. Its
     * ownership is transferred to the MSVController.
     * @param scale the scaling to apply to the mesh and texture at rendering
     *   time, formatted as {scale_x, scale_y, scale_z}.
     */
    static void setDynamicModel(MSVTextureCallback *cb,
                                const float scale[3]);

    /** Checks if the QCAR::State object corresponds to the current
     * target being tracked.
     * If yes, it will return the index of this target in the current
     * dataset (0 if the target has been built at runtime). Otherwise,
     * it will return -1.
     */
    static int currentTargetFound(QCAR::State &state);

    /** Accessors to the sub-components */
    static MSVRenderer *getRenderer();
    static MSVTracker *getTracker();
    static MSVCallback *getCallback();

    /** Tool method: get a new, valid, unused OpenGL texture ID.
     * @return an OpenGL texture ID obtained with `glGenTexture` if rendering
     * has started, 0 otherwise.
     */
    static int obtainTextureID();

  private:
    static MSVRenderer *ms_Renderer;
    static MSVTracker *ms_Tracker;
    static MSVCallback *ms_Callback;

    static bool tracking;
    static MSVTargetInfo *currentInfo;

};
#endif
