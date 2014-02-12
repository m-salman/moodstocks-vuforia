package com.moodstocks.vuforia.core;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.os.AsyncTask;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.RelativeLayout;

import com.moodstocks.vuforia.AbstractModel;
import com.moodstocks.vuforia.StaticModel;
import com.moodstocks.vuforia.DynamicModel;
import com.moodstocks.vuforia.Mesh;
import com.moodstocks.vuforia.Texture;
import com.qualcomm.QCAR.QCAR;

/** Class wrapping the Vuforia SDK */
public class VuforiaController {

  public static final String TAG = "VuforiaController";

  /** The parent {@link Activity} */
  private Activity parent;
  /** The {@link RelativeLayout} used to display the camera preview. */
  private RelativeLayout preview;
  /** The {@link Callback} to notify */
  private Listener listener;

  /** Camera frame pixels */
  public byte[] pixels = null;
  /** Camera frame width */
  public int width = -1;
  /** Camera frame height */
  public int height = -1;
  /** Camera frame stride, in bytes */
  public int stride = -1;

  /** List of datasets names to load after initialization */
  private static List<String> datasets = new ArrayList<String>();

  /** Store the current state of the camera */
  private boolean cameraRunning = false;

  /** Listener interface to be notified of Vuforia SDK status updates */
  public static interface Listener {
    /** Informs the listener that a new frame has been processed by
     * the Vuforia SDK.
     */
    public void onStatusUpdate();
  }

  /** Constructor.
   * @param parent the parent {@link Activity}
   * @param preview the {@link RelativeLayout} into which the camera preview
   * and AR objects will be inflated. Should be placed in the background of
   * the UI.
   * @param listener the {@link Listener} to notify.
   */
  public VuforiaController(Activity parent, RelativeLayout preview, Listener listener) {
    this.parent = parent;
    this.preview = preview;
    this.listener = listener;
  }

  /**
   * Adds a bundled Vuforia dataset to the tracker. <b>Must be called
   * before {@link #init()} is called.</b>
   * @param name the name of the `.dat` and `.xml` files generated
   * using the Vuforia Target Manager, and bundled with the app in
   * the `assets` directory.
   * @return true if the dataset exists and has been loaded, false
   * otherwise.
   */
  public static boolean addDataset(Context ctx, String name) {
    if (!datasetExists(ctx, name)) return false;
    datasets.add(name);
    return true;
  }

  /** Initializes the VuforiaController.
   * @return the error message if an error occurred,
   * null otherwise.
   */
  public String init() {
    initializer = new Initializer();
    return initializer.init();
  }

  /** Starts the camera.
   * @return false if the camera could not be started because
   * the frames delivered by the Vuforia SDK won't fit the
   * Moodstocks SDK requirements, true otherwise.
   */
  public boolean start() {
    if (!cameraRunning) {
      if (mGlView == null) return false;
      if (!startCameraNative()) return false;
      mGlView.setVisibility(View.VISIBLE);
      mGlView.onResume();
      cameraRunning = true;
    }
    return true;
  }

  /** Stops the camera */
  public void stop() {
    stopTracking();
    if (cameraRunning) {
      stopCameraNative();
      if (mGlView != null)
      {
        mGlView.setVisibility(View.INVISIBLE);
        mGlView.onPause();
      }
      cameraRunning = false;
    }
  }

  /** Closes and deinitializes the Vuforia SDK */
  public void deInit() {
    datasets.clear();
    initializer.stopInit();
    initializer.deInit();
  }

  /** Asks the Vuforia SDK to start tracking a target bundled in a dataset.
   * Should be called only when not already tracking a target. Otherwise it will
   * automatically reset this Controller in order for a second call
   * to this function to have the desired effect.
   * @param t the {@link Target} to track. Its name must fit the one used
   * when indexing the target on the Vuforia Target Manager.
   * @param dataset the name of the dataset to use, corresponding to the
   *   `.dat` and `.xml` files generated using the Vuforia Target Manager.
   */
  public native void startTracking(Target t, String dataset);

  /** Stop tracking, so that the Vuforia SDK does nothing
   * except providing camera frames through its callback.
   */
  public native void stopTracking();

  /**
   * Check whether this Controller is currently tracking
   * a target.
   * @return true if tracking, false otherwise.
   */
  public native boolean isTracking();

  /**
   * Get information about the target currently being tracked or built.
   * @return the current {@link com.moodstocks.vuforia.core.Target}.
   */
  public native Target getCurrentTarget();

  /** Replaces the currently displayed augmented reality model (if any)
   * by the given {@link AbstractModel} object.
   * @param model the model to display.
   */
  public void changeCurrentModel(AbstractModel model) {
    if (model.isDynamic()) {
      DynamicModel m = (DynamicModel)model;
      this.setDynamicModel(m.getCallback(), m.getScale());
    }
    else {
      StaticModel m = (StaticModel)model;
      this.setStaticModel(m.getMesh(), m.getTexture(), m.getScale());
    }
  }

  /**
   * Require an update of the {@link VuforiaController} status through
   * the {@link Listener#onStatusUpdate()} method.
   */
  public native void requireUpdate();

  /**
   * Require the camera frame through the {@link #pixels},
   * {@link #width}, {@link #height} and {@link #stride} fields.
   * <p>
   * Should be called <b>only</b> from {@link Listener#onStatusUpdate()}.
   * @return true if the frame could be fetched, false otherwise
   * (for example if not called from {@link Listener#onStatusUpdate()}.
   */
  public native boolean getFrame();

  /**
   * Call this method to know if a new target has just started
   * being tracked by the Vuforia SDK.
   * <p>
   * This is useful for example to build a new AR model for display.
   * <p>
   * Should be called <b>only</b> from {@link Listener#onStatusUpdate()}.
   * @return true if a new target is tracked, false otherwise.
   */
  public native boolean isNewTarget();

  /**
   * Call this method to know if the target that was being tracked
   * has been lost in this frame.
   * <p>
   * Useful for example to deactivate the tracking if the target has
   * been lost for too long.
   * <p>
   * Should be called <b>only</b> from {@link Listener#onStatusUpdate()}.
   * @return true if the target is lost, false otherwise.
   */
  public native boolean isTargetLost();

  /**
   * Get a new, valid, unused OpenGL texture ID.
   * @return an OpenGL texture ID obtained with `glGenTexture` if rendering
   * has started, 0 otherwise.
   */
  public native int obtainTextureID();

  /** Called from native shortly after a call to {@link #requireUpdate()} */
  private void onStatusUpdate() {
    if (listener != null) listener.onStatusUpdate();
  }

  /** Static block to load native libraries */
  static
  {
    System.loadLibrary("Vuforia");
    System.loadLibrary("VuforiaWrapper");
    System.loadLibrary("MoodstocksVuforia");
  }

  /** The GLView to use for display */
  private GLView mGlView;
  /** The {@link Renderer} to use for display */
  private Renderer mRenderer;

  /** The {@link Initializer} to use */
  private Initializer initializer;
  /** The initializer parameter: request OpenGLES 2 */
  private int mQCARFlags = QCAR.GL_20;

  /** Native method to start the camera */
  private native boolean startCameraNative();
  /** Native method to stop the camera */
  private native void stopCameraNative();

  /** Cache the list of available assets */
  private static List<String> assets = null;

  /** Inner class to manage the initialization of all components asynchronously */
  private class Initializer extends AsyncTask<Void, Void, Integer>
  {
    private int mProgressValue = -1;
    private Object mShutdownLock = null;

    public Initializer() {
      this.mShutdownLock = new Object();
    }

    public String init() {
      synchronized (mShutdownLock)
      {
        QCAR.setInitParameters(parent, mQCARFlags);

        do
        {
          // Must be called repeatedly to advance step-by-step.
          // Return value is either -1 if an error occurred, or a percentage
          // of progression.
          mProgressValue = QCAR.init();

          // Check that: the task has not been cancelled, that no error occurred,
          // and that the initialization is not finished.
        } while (!isCancelled() && mProgressValue >= 0 && mProgressValue < 100);
      }
      if (mProgressValue < 0) {
        return (mProgressValue == QCAR.INIT_DEVICE_NOT_SUPPORTED) ?
               "Vuforia SDK Error: this device is not supported." :
               "Vuforia SDK Error: initialization failed.";
      }
      initNative();
      initGL();
      this.execute();
      return null;
    }

    public void stopInit() {
      if (this.getStatus() != Status.FINISHED) {
        this.cancel(true);
      }
    }

    public void deInit() {
      synchronized (mShutdownLock) {
        deInitNative();
        QCAR.deinit();
      }
    }

    protected Integer doInBackground(Void... params)
    {
      for (String dataset : datasets) {
        addDatasetNative (dataset);
      }
      return Integer.valueOf(0);
    }
  }

  /** Initializes the native part of the code */
  private native void initNative();
  /** Deinitializes the native part of the code */
  private native void deInitNative();

  /**
   * Method to initialize the {@link Renderer} and the GLView.
   * Must be called from GL thread!
   */
  private void initGL() {
    mGlView = new GLView(parent);
    mGlView.init(QCAR.requiresAlpha());

    mRenderer = new Renderer(parent);
    mGlView.setRenderer(mRenderer);

    mRenderer.mIsActive = true;
    preview.addView(mGlView, new LayoutParams(LayoutParams.MATCH_PARENT,
                                              LayoutParams.MATCH_PARENT));
  }

  /** Checks that a Vuforia dataset exists */
  private static boolean datasetExists(Context ctx, String name) {
    try {
      assets = Arrays.asList(ctx.getResources().getAssets().list(""));
    } catch (IOException e) {
      return false;
    }
    return (assets.contains(name+".xml") && assets.contains(name+".dat"));
  }

  private native void addDatasetNative(String name);

  private native void setStaticModel(Mesh mesh, Texture tex, float[] scale);

  private native void setDynamicModel(DynamicModel.Callback cb, float[] scale);

}
