#ifndef MSV_CALLBACK_H
#define MSV_CALLBACK_H

#include <QCAR/UpdateCallback.h>
#include <QCAR/Image.h>

#define LOST_FRAMES_TOL 15

class MSVTargetInfo;

/** Implementation of QCAR::UpdateCallback.
 *
 * It is used to notify the caller of the current state of the
 * associated MSController and send it camera frames if required.
 */
class MSVCallback : public QCAR::UpdateCallback
{
  public:
    MSVCallback();
    virtual ~MSVCallback();

    /** Implementation of QCAR::UpdateCallback */
    void QCAR_onUpdate(QCAR::State& state);

    /** Requires a new call to `onStatusUpdate` as soon as possible */
    void requireUpdate();

    /** Calls virtual `getFrame(frame)` method with the current
     * camera frame.
     * Should only be called from inside `onStatusUpdate` method!
     */
    bool getFrame() const;

    /** Returns true if tracking of a new target has just started.
     * Useful to trigger the creation of a new mesh/texture pair to display
     * Should only be called from inside `onStatusUpdate` method!
     */
    bool isNewTarget() const;

    /** Returns true if the currently tracked target has been lost.
     * Useful to automatically stop tracking if the target can't be
     * tracked (for example) for some time.
     * Should only be called from inside `onStatusUpdate` method!
     */
    bool isTargetLost() const;

  protected:
    /** Virtual method that will get called each time `requireUpdate()`
     * is called. It should be used to take actions depending on the
     * current state of the MSController.
     */
    virtual void onStatusUpdate() = 0;

    /** Virtual method that will be called each time `getFrame()` is
     * called. it should be used to dispatch the camera frames to the
     * caller.
     */
    virtual void getFrame(const QCAR::Image *frame) const = 0;

  private:
    int imgIndex;
    const QCAR::Image *currentFrame;
    bool needUpdate;
    bool wasTracking;
    bool isNew;
    bool isLost;

    /* Through this counter, we give a tolerance of LOST_FRAMES_TOL frames
     * immediately after a call to `MSVController::startTracking()`, during
     * which the target won't be considered as lost even if it is not found
     * in the current frame. This compensates the fact that the MSVTracker may
     * take a few frames to initialize (up to 10 on slow devices) and actually
     * be able to find the target.
     */
    int lostCounter;
};

#endif
