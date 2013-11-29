package com.moodstocks.vuforia.core;

import java.lang.ref.WeakReference;

import com.moodstocks.android.Image;
import com.moodstocks.android.MoodstocksError;
import com.moodstocks.android.Scanner.Flags;
import com.moodstocks.android.Result;
import com.moodstocks.android.Scanner;
import com.moodstocks.android.core.OrientationListener;
import com.moodstocks.android.core.OrientationListener.Orientation;

import android.app.Activity;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;

/**
 * This class implements the same logic as
 * {@link com.moodstocks.android.ScannerSession}, but
 * does not manage the camera itself, as we expect the
 * Vuforia SDK to provide us the frames.
 */
public class MoodstocksController extends Handler {
  private Scanner scanner = null;
  private WeakReference<Listener> listener;
  private WorkerThread worker;

  private byte[] imgData;
  private int imgWidth;
  private int imgHeight;
  private int imgStride;

  private int orientation = Orientation.NONE;

  /** This project needs these flags to work */
  protected int extras = Result.Extra.DIMENSIONS;

  /**
   * Interface that must be implemented to receive callbacks from the ScannerSession.
   */
  public static interface Listener {
    /**
     * Notifies the listener that a scan has ended.
     * @param result the {@link com.moodstocks.android.Result} if any, null otherwise.
     */
    public void onScanComplete(Result result);
    /**
     * Notified the listener that a scan has failed.
     * @param error the {@link com.moodstocks.android.MoodstocksError} that caused the
     * scan to fail.
     */
    public void onScanFailed(MoodstocksError error);
  }

  /**
   * Constructor.
   * @param parent the caller {@link Activity}
   * @param listener the {@link Listener} to notify
   * to notify if an error occurs.
   */
  public MoodstocksController(Activity parent, Listener listener) {
      this.listener = new WeakReference<Listener>(listener);
      this.worker = new WorkerThread(this);
      worker.start();
      OrientationListener.init(parent);
      OrientationListener.get().enable();
  }

  /** Initializes the MoodstocksController
   * @return the error message if the initialization failed, null
   * otherwise.
   */
  public String init() {
    orientation = OrientationListener.get().getOrientation();
    try {
      this.scanner = Scanner.get();
    }
    catch (MoodstocksError e) {
      return "Moodstocks SDK Error "+e.getErrorCode()+": "+e.getMessage();
    }
    return null;
  }

  /**
   * Closes the ScannerManager
   * <p>
   * Must be called before exiting the enclosing {@link Activity}.
   */
  public void deInit() {
    OrientationListener.get().disable();
    finishWorker(500L);
  }

  public void onConfigurationChanged() {
    orientation = OrientationListener.get().getOrientation();
  }

  /**
   * Closes the worker thread.
   * @param t the time in milliseconds allowed for the thread to end.
   */
  private void finishWorker(long t) {
    worker.getHandler().obtainMessage(MsgCode.QUIT).sendToTarget();
    try {
      worker.join(t);
    } catch (InterruptedException e) {

    }
  }

  /**
   * Search for the frame in the local database.
   * @param data the frame data
   * @param w the frame width
   * @param h the frame height
   * @param stride the frame stride, in bytes per row
   */
  public void scan(byte[] data, int w, int h, int stride) {
    this.imgData = data;
    this.imgWidth = w;
    this.imgHeight = h;
    this.imgStride = stride;
    worker.getHandler().obtainMessage(MsgCode.SCAN).sendToTarget();
  }

  /**
   * <i>Internal message passing utility</i>
   */
  @Override
  public void handleMessage(Message msg) {
    Listener l = listener.get();

    switch(msg.what) {

      case MsgCode.SUCCESS:
        if (l != null)
          l.onScanComplete((Result)msg.obj);
        break;

      case MsgCode.FAILED:
        if (l != null)
          l.onScanFailed((MoodstocksError)msg.obj);
        break;

      default:
        break;
    }

  }

  /**
   * <i>Background thread processing the camera frames.</i>
   */
  private class WorkerThread extends Thread {

    private Handler handler;
    private MoodstocksController manager;

    private WorkerThread(MoodstocksController manager) {
      super();
      this.manager = manager;
    }

    @Override
    public void run() {
      Looper.prepare();
      handler = new WorkerHandler(this);
      Looper.loop();
    }

    private Handler getHandler() {
      return handler;
    }

    private void quit() {
      Looper.myLooper().quit();
    }

    private void scan() {
      Result result = null;
      MoodstocksError error = null;
      try {
        result = scan(new Image(imgData, imgWidth, imgHeight, imgStride, orientation));
      } catch (MoodstocksError e) {
        error = e;
      }
      if (error != null) {
        manager.obtainMessage(MsgCode.FAILED, error).sendToTarget();
      }
      else {
        manager.obtainMessage(MsgCode.SUCCESS, result).sendToTarget();
      }
    }

    private Result scan(Image qry)
        throws MoodstocksError {

      qry.retain();
      Result result = null;

      try {
        result = scanner.search2(qry, extras, Flags.SMALLTARGET);
      } catch (MoodstocksError e) {
        if (e.getErrorCode() != MoodstocksError.Code.EMPTY)
          throw e;
      }

      qry.release();
      return result;
    }
  }

  /**
   * <i>Internal message passing utility</i>
   */
  private static class WorkerHandler extends Handler {

    private final WeakReference<WorkerThread> worker;

    private WorkerHandler(WorkerThread worker) {
      super();
      this.worker = new WeakReference<WorkerThread>(worker);
    }

    @Override
    public void handleMessage(Message msg) {

      WorkerThread w = worker.get();

      if (w != null) {
        switch(msg.what) {

          case MsgCode.SCAN:
            w.scan();
            break;

          case MsgCode.QUIT:
            w.quit();
            break;

          default:
            break;

        }
      }
    }
  }

  /**
   * <i>Internal message passing codes</i>
   */
  protected static final class MsgCode {
    public static final int SCAN = 0;
    public static final int QUIT = 1;
    public static final int SUCCESS = 2;
    public static final int FAILED = 3;
  }

}
