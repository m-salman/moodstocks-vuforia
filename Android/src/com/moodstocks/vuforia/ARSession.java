package com.moodstocks.vuforia;

import android.app.Activity;
import android.content.Context;
import android.os.AsyncTask;
import android.widget.RelativeLayout;

import com.moodstocks.android.MoodstocksError;
import com.moodstocks.android.Result;
import com.moodstocks.vuforia.core.MoodstocksController;
import com.moodstocks.vuforia.core.Target;
import com.moodstocks.vuforia.core.VuforiaController;
import com.qualcomm.QCAR.QCAR;

/**
 * Main class, controlling the Vuforia SDK, the Moodstocks SDK and their interactions.
 */
public class ARSession implements MoodstocksController.Listener, VuforiaController.Listener {

  /** Interface implemented to be notified of ARSession events.*/
  public static interface Listener {
    /**
     * Called when the Moodstocks Scanner returns a new non-null result.
     * @param result the incoming result.
     * @return the name of the dataset to use for tracking if you wish this target
     * to be tracked, null otherwise.
     */
    public String onResultFound(Result result);

    /**
     * Method called asynchronously to build the {@link AbstractModel} to display
     * from the name and dimensions of the found result.
     *<p>
     * IMPORTANT:
     * The returned object will be displayed in the scene using the conventions
     * on coordinate systems used in this project, i.e for a given target:
     *<p>
     * - the origin is located at the center of the target, the X axis points
     * to its right, the Y axis to its top, and the Z axis follows the right
     * hand rule
     *<p>
     * - this coordinate system is orthonormal, with the norm given by half the
     * <b>smallest</b> dimension of the target. For example, if the indexed target
     * is 450px wide and 600px long, the X axis will go from the center to the right
     * edge of the target, while the Y axis will go from the center to 0.75% of the
     * top of the image.
     * <p>
     * Implications on examples:
     * <p>
     * - Displaying a 2x2 plane on a square target will exactly fit the target.
     * <p>
     * - Displaying a 3x2 plane on a rectangular 300x200px (or any 3:2 ratio)
     *    target will exactly fit the target, so will a 2x3 plane on a rectangular
     *    200x300px target.
     * <p>
     * - If displaying a more complex 3D model, you should probably ensure that
     *   its x and y coordinates are in the [-1..1] range.
     *
     * @param name the tracked target name
     * @param width its width
     * @param height its height
     * @return the Model
     */
    public AbstractModel buildModel(String name, int width, int height);
  }

  private VuforiaController vuforia;
  private MoodstocksController moodstocks;
  private String errMsg = null;

  private Activity parent;
  private Listener listener;
  private RelativeLayout preview;

  private modelBuildingTask modelTask = null;
  private AbstractModel builtModel = null;

  private boolean paused = false;

  private long lastFound = 0;
  private static final long UNTRACK_DELAY = 500 /*ms*/;

  /**
   * Adds a bundled Vuforia dataset to the tracker. <b>Must be called
   * before {@link #init()} method!</b>
   * @param name the name of the `.dat` and `.xml` files generated
   * using the Vuforia Target Manager (without file extension), and
   * bundled with the app in the `assets` directory.
   * @return true if the dataset exists and has been loaded, false
   * otherwise.
   */
  public static boolean addDataset(Context ctx, String name) {
    return VuforiaController.addDataset(ctx, name);
  }

  /**
   * Constructor
   * @param parent the parent Activity
   * @param listener the {@link Listener} to notify
   * @param preview a {@link RelativeLayout} into which the camera preview
   * and augmented reality objects will be inflated. Should be placed
   * in the background of your scanner UI.
   */
  public ARSession(Activity parent, Listener listener, RelativeLayout preview) {
    this.parent = parent;
    this.listener = listener;
    this.preview = preview;
  }

  /**
   * Initializes the ARSession.
   * Should be called in the `onCreate` method of your activity.
   * @return false if an error occurred, true otherwise. If an error
   * occurred, the ARSession object won't be usable and the user should
   * be informed. You can get the corresponding error message by calling
   * {@link #getErrorMessage()}.
   */
  public boolean init() {
    vuforia = new VuforiaController(parent, preview, this);
    errMsg = vuforia.init();
    if (errMsg == null) {
      moodstocks = new MoodstocksController(parent, this);
      errMsg = moodstocks.init();
    }
    if (errMsg != null) return false;
    return true;
  }

  /**
   * Get the error message if the call to {@link #init()} failed.
   * @return the error message that caused the initialization to fail.
   */
  public String getErrorMessage() {
    return errMsg;
  }

  /**
   * Call this method from your Activity {@link android.app.Activity#onConfigurationChanged(android.content.res.Configuration)}
   * method if you wish the ARSession to properly respond to UI orientation changes.
   */
  public void onConfigurationChanged() {
    moodstocks.onConfigurationChanged();
  }

  /**
   * Starts the ARSession.
   * Once called, the camera preview will start displaying and the
   * frames will start being processed.
   * Should be called in the `onResume` method of your activity.
   * @return true if the camera should be started, false if it could not
   * be started because the resolution of the frames passed by the Vuforia SDK
   * is too low for the Moodstocks SDK.
   */
  public boolean start() {
    QCAR.onResume();
    return vuforia.start();
  }

  /**
   * Pauses the ARSession.
   * This only stop the frames processing: the camera preview will keep on
   * being displayed.
   */
  public void pause() {
    paused = true;
  }

  /** Resumes the ARSession after it has been paused. */
  public void resume() {
    if (paused) {
      paused = false;
      vuforia.requireUpdate();
    }
  }

  /**
   * Stops the ARSession. This removes the camera preview and
   * stops the frame processing.
   * Should be called in the `onPause` method of your activity.
   */
  public void stop() {
    QCAR.onPause();
    vuforia.stop();
  }

  /**
   * Deinitializes the ARSession.
   * Should be called in the `onDestroy` method of your activity.
   */
  public void deInit() {
    vuforia.deInit();
    moodstocks.deInit();
  }

  /** Tool method: get a new, valid, unused OpenGL texture ID.
   * @return an OpenGL texture ID obtained with `glGenTexture` if rendering
   * has started, 0 otherwise.
   */
  public int obtainTextureID() {
    return vuforia.obtainTextureID();
  }

  /** {@link com.moodstocks.vuforia.core.VuforiaController.Listener#onStatusUpdate()} implementation.
   * This function handles the logic of the Vuforia SDK, the Moodstocks SDK and their interactions.
   */
  @Override
  public void onStatusUpdate() {
    if (paused) return;
    if (vuforia.isTracking()) {
      /* Vuforia is currently trying to track a target */
      if (!vuforia.isTargetLost()) {
        /* The target is found */
        if (vuforia.isNewTarget()) {
          /* it is a new target: ask for the corresponding
           * model to be built.
           */
          buildModel();
        }
        else if (builtModel != null) {
          /* a new model is ready for display, either because it has
           * just been built, or because it has been updated by the caller:
           * ask the Vuforia SDK to use it.
           */
          vuforia.changeCurrentModel(builtModel);
          builtModel = null;
        }
        /* Mark the timestamp of the last time the tracked target has been found */
        lastFound = System.currentTimeMillis();
      }
      else if (System.currentTimeMillis() - lastFound > UNTRACK_DELAY) {
        /* target is lost for more than UNTRACK_DELAY ms:
         * exit tracking mode.
         */
        vuforia.stopTracking();
      }
      vuforia.requireUpdate();
    }
    else {
      /* Vuforia is inactive: send the frame to the Moodstocks SDK
       * for image recognition. See `onScanComplete` for the result
       * handling.
       * `requireUpdate` is not called, as we're waiting for the
       * scan to complete.
       */
      vuforia.getFrame();
      moodstocks.scan(vuforia.pixels, vuforia.width, vuforia.height, vuforia.stride);
    }
  }

  /** {@link com.moodstocks.vuforia.core.MoodstocksController.Listener#onScanComplete(Result)} implementation */
  @Override
  public void onScanComplete(Result result) {
    if (paused) return;
    if (vuforia.isTracking()) return;
    askCallerAction(result);
    vuforia.requireUpdate();
  }

  /**
   * Invokes the {@link Listener#onResultFound(Result)} callback and
   * processes its result.
   * @param result the result with which the caller should decide of
   * an action to take.
   */
  private void askCallerAction(Result result) {
    if (result == null) return;
    String dataset = listener.onResultFound(result);
    if (dataset != null)
      vuforia.startTracking(Target.fromResult(result), dataset);
  }

  /** {@link com.moodstocks.vuforia.core.MoodstocksController.Listener#onScanFailed(MoodstocksError)} implementation */
  @Override
  public void onScanFailed(MoodstocksError e) {
    if (paused) return;
    e.log();
    vuforia.requireUpdate();
  }


  /**
   * Asynchronously calls the {@link Listener#buildModel(String, int, int)} method
   * to ask for a {@link AbstractModel} to display.
   * This method is called automatically by the ARSession whenever a new target
   * starts being tracked, but can also be called manually to trigger a call to
   * {@link Listener#buildModel(String, int, int)} and update the displayed model.
   */
  public void buildModel() {
    if (vuforia.getCurrentTarget() == null) return;
    if (modelTask != null)
      modelTask.abort();
    modelTask = new modelBuildingTask(vuforia.getCurrentTarget());
    modelTask.execute();
  }

  /** AsyncTask extension to build AR Models */
  private class modelBuildingTask extends AsyncTask<Void,Void,AbstractModel> {

    private Target target;

    private modelBuildingTask(Target t) {
      target = t;
    }

    @Override
    protected AbstractModel doInBackground(Void... params) {
      return listener.buildModel(target.getName(), target.getWidth(), target.getHeight());
    }

    @Override
    protected void onPostExecute(AbstractModel m) {
      builtModel = m;
      modelTask = null;
    }

    private void abort() {
      cancel(true);
      modelTask = null;
    }

  }

}
