/*==============================================================================
Copyright (c) 2012-2013 QUALCOMM Austria Research Center GmbH.
All Rights Reserved.

This  Vuforia(TM) sample application in source code form ("Sample Code") for the
Vuforia Software Development Kit and/or Vuforia Extension for Unity
(collectively, the "Vuforia SDK") may in all cases only be used in conjunction
with use of the Vuforia SDK, and is subject in all respects to all of the terms
and conditions of the Vuforia SDK License Agreement, which may be found at
https://developer.vuforia.com/legal/license.

By retaining or using the Sample Code in any manner, you confirm your agreement
to all the terms and conditions of the Vuforia SDK License Agreement.  If you do
not agree to all the terms and conditions of the Vuforia SDK License Agreement,
then you may not retain or use any of the Sample Code in any manner.


@file
    VideoPlayerHelper.java

@brief
    Sample for VideoPlayerHelper

==============================================================================*/

/* This file is a modified version of the VideoPlayerHelper packaged with the
 * official Vuforia `VideoPlayback` sample.
 *
 * Major modification:
 * The `load` method has been modified to use `MediaPlayer.prepare()` method
 * instead of `MediaPlayer.prepareAsync()`. This is a quick workaround in order
 * to be able to get information about the video (resolution, length, etc) easily
 * and immediately after `load` has been called.
 *
 * Other modifications:
 * - only support local files,
 * - remove full screen support
 */


package com.qualcomm.QCARSamples;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.util.Log;
import android.view.Surface;
import android.content.Intent;
import android.content.res.AssetFileDescriptor;
import android.graphics.SurfaceTexture;
import android.os.Build;

import java.util.concurrent.locks.ReentrantLock;

/** Helper class for video playback functionality */
public class VideoPlayerHelper implements OnCompletionListener,
                                          OnErrorListener
{
  public static final String TAG = "VideoPlayerHelper";

  public static final int CURRENT_POSITION            = -1;
  private MediaPlayer     mMediaPlayer                = null;
  private SurfaceTexture  mSurfaceTexture             = null;
  private int             mTextureID                  = 0;
  Intent                  mPlayerHelperActivityIntent = null;
  private Activity        mParentActivity             = null;
  private MEDIA_STATE     mCurrentState               = MEDIA_STATE.NOT_READY;
  private boolean         mShouldPlayImmediately      = false;
  private int             mSeekPosition               = CURRENT_POSITION;
  private ReentrantLock   mMediaPlayerLock            = null;
  private ReentrantLock   mSurfaceTextureLock         = null;

  // This enum declares the possible states a media can have:
  public enum MEDIA_STATE
  {
    REACHED_END     (0),
    PAUSED          (1),
    STOPPED         (2),
    PLAYING         (3),
    READY           (4),
    NOT_READY       (5),
    ERROR           (6);

    private int type;
    MEDIA_STATE (int i)
    {
      this.type = i;
    }
    public int getNumericType()
    {
      return type;
    }
  }

  /** Initializes the VideoPlayerHelper object. */
  public boolean init()
  {
    mMediaPlayerLock = new ReentrantLock();
    mSurfaceTextureLock = new ReentrantLock();

    return true;
  }

  /** Deinitializes the VideoPlayerHelper object. */
  public boolean deinit()
  {
    unload();

    mSurfaceTextureLock.lock();
    mSurfaceTexture = null;
    mSurfaceTextureLock.unlock();

    return true;
  }

  /** Loads a movie from a file in the assets folder */
  @SuppressLint("NewApi")
  public boolean load(String filename, boolean playOnTextureImmediately, int seekPosition)
  {
    boolean result = false;
    mMediaPlayerLock.lock();
    mSurfaceTextureLock.lock();

    // If the media has already been loaded then exit.
    // The client must first call unload() before calling load again:
    if ((mCurrentState == MEDIA_STATE.READY) || (mMediaPlayer != null))
    {
      Log.d(TAG, "Already loaded");
    }
    else
    {
      if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.ICE_CREAM_SANDWICH)  // if this is an ICS device
      {
        if (mSurfaceTexture == null)
        {
          Log.d(TAG, "Can't load file to ON_TEXTURE because the Surface Texture is not ready");
        }
        else
        {
          try
          {
            mMediaPlayer = new MediaPlayer();

            AssetFileDescriptor afd = mParentActivity.getAssets().openFd(filename);
            mMediaPlayer.setDataSource(afd.getFileDescriptor(),afd.getStartOffset(),afd.getLength());
            afd.close();

            mMediaPlayer.prepare();
            mMediaPlayer.setOnCompletionListener(this);
            mMediaPlayer.setOnErrorListener(this);
            mMediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
            mMediaPlayer.setSurface(new Surface(mSurfaceTexture));
            mShouldPlayImmediately = playOnTextureImmediately;
            mCurrentState = MEDIA_STATE.READY;
            mSeekPosition = 0;
            // If requested an immediate play
            if (mShouldPlayImmediately) play( mSeekPosition);
          }
          catch (Exception e)
          {
            Log.e(TAG, "Error while creating the MediaPlayer: " + e.toString());

            mCurrentState = MEDIA_STATE.ERROR;
            mMediaPlayerLock.unlock();
            mSurfaceTextureLock.unlock();
            return false;
          }
        }
      }
      else
      {
        try
        {
          // We need to verify that the file exists
          AssetFileDescriptor afd = mParentActivity.getAssets().openFd(filename);
          afd.close();
        }
        catch (Exception e)
        {
          Log.d(TAG, "File does not exist");
          mCurrentState = MEDIA_STATE.ERROR;
          mMediaPlayerLock.unlock();
          mSurfaceTextureLock.unlock();
          return false;
        }
      }

      // We store the parameters for further use
      mSeekPosition = seekPosition;

      result = true;
    }

    mSurfaceTextureLock.unlock();
    mMediaPlayerLock.unlock();

    return result;
  }

  /** Unloads the currently loaded movie
        After this is called a new load() has to be invoked */
  public boolean unload()
  {
    mMediaPlayerLock.lock();
    if (mMediaPlayer != null) {
      try
      {
        mMediaPlayer.stop();
      }
      catch (Exception e)
      {
        mMediaPlayerLock.unlock();
        Log.e(TAG, "Could not start playback");
      }

      mMediaPlayer.release();
      mMediaPlayer = null;
    }
    mMediaPlayerLock.unlock();

    mCurrentState = MEDIA_STATE.NOT_READY;
    return true;
  }

  /** Return the current status of the movie such as Playing, Paused or Not Ready */
  MEDIA_STATE getStatus()
  {
    return mCurrentState;
  }

  /** Returns the width of the video frame */
  public int getVideoWidth()
  {
    if ((mCurrentState == MEDIA_STATE.NOT_READY) || (mCurrentState == MEDIA_STATE.ERROR))
    {
      // Log.d(TAG, "Cannot get the video width if it is not ready");
      return -1;
    }

    int result=-1;
    mMediaPlayerLock.lock();
    if (mMediaPlayer != null)
      result = mMediaPlayer.getVideoWidth();
    mMediaPlayerLock.unlock();

    return result;
  }

  /** Returns the height of the video frame */
  public int getVideoHeight()
  {
    if ((mCurrentState == MEDIA_STATE.NOT_READY) || (mCurrentState == MEDIA_STATE.ERROR))
    {
      // Log.d(TAG, "Cannot get the video height if it is not ready");
      return -1;
    }

    int result=-1;
    mMediaPlayerLock.lock();
    if (mMediaPlayer != null)
      result = mMediaPlayer.getVideoHeight();
    mMediaPlayerLock.unlock();

    return result;
  }

  /** Returns the length of the current movie */
  public float getLength()
  {
    if ((mCurrentState == MEDIA_STATE.NOT_READY) || (mCurrentState == MEDIA_STATE.ERROR))
    {
      // Log.d(TAG, "Cannot get the video length if it is not ready");
      return -1;
    }

    int result=-1;
    mMediaPlayerLock.lock();
    if (mMediaPlayer != null)
      result = mMediaPlayer.getDuration()/1000;
    mMediaPlayerLock.unlock();

    return result;
  }


  /** Request a movie to be played either full screen or on texture and at a given position */
  public boolean play(int seekPosition)
  {
    if ((mCurrentState == MEDIA_STATE.NOT_READY) || (mCurrentState == MEDIA_STATE.ERROR))
    {
      Log.d(TAG, "Cannot play this video if it is not ready");
      return false;
    }

    mMediaPlayerLock.lock();
    // If the client requests a given position
    if (seekPosition != CURRENT_POSITION)
    {
      try
      {
        mMediaPlayer.seekTo(seekPosition);
      }
      catch (Exception e)
      {
        mMediaPlayerLock.unlock();
        Log.e(TAG, "Could not seek to position");
      }
    }
    else    // If it had reached the end loop it back
    {
      if (mCurrentState == MEDIA_STATE.REACHED_END)
      {
        try
        {
          mMediaPlayer.seekTo(0);
        }
        catch (Exception e)
        {
          mMediaPlayerLock.unlock();
          Log.e(TAG, "Could not seek to position");
        }
      }
    }

    // Then simply start playing
    try
    {
      mMediaPlayer.start();
    }
    catch (Exception e)
    {
      mMediaPlayerLock.unlock();
      Log.e(TAG, "Could not start playback");
    }
    mCurrentState = MEDIA_STATE.PLAYING;

    mMediaPlayerLock.unlock();

    return true;
  }

  /** Pauses the current movie being played */
  public boolean pause()
  {
    if ((mCurrentState == MEDIA_STATE.NOT_READY) || (mCurrentState == MEDIA_STATE.ERROR))
    {
      // Log.d(TAG, "Cannot pause this video if it is not ready");
      return false;
    }

    boolean result = false;

    mMediaPlayerLock.lock();
    if (mMediaPlayer != null)
    {
      if (mMediaPlayer.isPlaying())
      {
        try
        {
          mMediaPlayer.pause();
        }
        catch (Exception e)
        {
          mMediaPlayerLock.unlock();
          Log.e(TAG, "Could not pause playback");
        }
        mCurrentState = MEDIA_STATE.PAUSED;
        result = true;
      }
    }
    mMediaPlayerLock.unlock();

    return result;
  }

  /** Stops the current movie being played */
  public boolean stop()
  {
    if ((mCurrentState == MEDIA_STATE.NOT_READY) || (mCurrentState == MEDIA_STATE.ERROR))
    {
      // Log.d(TAG, "Cannot stop this video if it is not ready");
      return false;
    }

    boolean result = false;

    mMediaPlayerLock.lock();
    if (mMediaPlayer != null)
    {
      mCurrentState = MEDIA_STATE.STOPPED;
      try
      {
        mMediaPlayer.stop();
      }
      catch (Exception e)
      {
        mMediaPlayerLock.unlock();
        Log.e(TAG, "Could not stop playback");
      }

      result = true;
    }
    mMediaPlayerLock.unlock();

    return result;
  }

  /** Tells the VideoPlayerHelper to update the data from the video feed */
  @SuppressLint("NewApi")
  public int updateVideoData()
  {
    int result = 0;

    mSurfaceTextureLock.lock();
    if (mSurfaceTexture != null)
    {
      // Only request an update if currently playing
      if (mCurrentState == MEDIA_STATE.PLAYING)
        mSurfaceTexture.updateTexImage();

      result = mTextureID;
    }
    mSurfaceTextureLock.unlock();

    return result;
  }

  /** Moves the movie to the requested seek position */
  public boolean seekTo(int position)
  {
    if ((mCurrentState == MEDIA_STATE.NOT_READY) || (mCurrentState == MEDIA_STATE.ERROR))
    {
      // Log.d(TAG, "Cannot seek-to on this video if it is not ready");
      return false;
    }

    boolean result = false;
    mMediaPlayerLock.lock();
    if (mMediaPlayer != null)
    {
      try
      {
        mMediaPlayer.seekTo(position);
      }
      catch (Exception e)
      {
        mMediaPlayerLock.unlock();
        Log.e(TAG, "Could not seek to position");
      }
      result = true;
    }
    mMediaPlayerLock.unlock();

    return result;
  }

  /** Gets the current seek position */
  public int getCurrentPosition()
  {
    if ((mCurrentState == MEDIA_STATE.NOT_READY) || (mCurrentState == MEDIA_STATE.ERROR))
    {
      // Log.d(TAG, "Cannot get the current playback position of this video if it is not ready");
      return -1;
    }

    int result = -1;
    mMediaPlayerLock.lock();
    if (mMediaPlayer != null)
      result = mMediaPlayer.getCurrentPosition();
    mMediaPlayerLock.unlock();

    return result;
  }

  /** Sets the volume of the movie to the desired value */
  public boolean setVolume(float value)
  {
    if ((mCurrentState == MEDIA_STATE.NOT_READY) || (mCurrentState == MEDIA_STATE.ERROR))
    {
      // Log.d(TAG, "Cannot set the volume of this video if it is not ready");
      return false;
    }

    boolean result = false;
    mMediaPlayerLock.lock();
    if (mMediaPlayer != null)
    {
      mMediaPlayer.setVolume(value, value);
      result = true;
    }
    mMediaPlayerLock.unlock();

    return result;
  }

  /**
   *  The following functions are specific to Android
   *  and will likely not be implemented on other platforms
   */

  /** With this we can set the parent activity */
  public void setActivity(Activity newActivity)
  {
    mParentActivity = newActivity;
  }

  /** To set a value upon completion */
  public void onCompletion(MediaPlayer arg0) {
    // Signal that the video finished playing
    mCurrentState = MEDIA_STATE.REACHED_END;
  }

  /** Used to set up the surface texture*/
  @SuppressLint("NewApi")
  public boolean setupSurfaceTexture(int nativeTextureID)
  {
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.ICE_CREAM_SANDWICH)
    {
      // We create a surface texture where the video can be played
      // We have to give it a texture id of an already created (in native) // OpenGL texture
      mSurfaceTextureLock.lock();
      mSurfaceTexture = new SurfaceTexture(nativeTextureID);
      mTextureID = nativeTextureID;
      mSurfaceTextureLock.unlock();

      return true;
    }
    else
      return false;
  }

  @SuppressLint("NewApi")
  public void getSurfaceTextureTransformMatrix(float []mtx)
  {
    mSurfaceTextureLock.lock();
    if (mSurfaceTexture != null)
      mSurfaceTexture.getTransformMatrix(mtx);
    mSurfaceTextureLock.unlock();
  }

  public boolean onError(MediaPlayer mp, int what, int extra) {

    if (mp == mMediaPlayer)
    {
      String errorDescription;

      switch (what)
      {
      case MediaPlayer.MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK:
        errorDescription = "The video is streamed and its container is not valid for progressive playback";
        break;
      case MediaPlayer.MEDIA_ERROR_SERVER_DIED:
        errorDescription = "Media server died";
        break;
      case MediaPlayer.MEDIA_ERROR_UNKNOWN:
        errorDescription = "Unspecified media player error";
        break;
      default:
        errorDescription = "Unknown error " + what;
      }

      Log.e(TAG, "Error while opening the file. Unloading the media player (" + errorDescription + ", " + extra + ")");

      unload();

      mCurrentState = MEDIA_STATE.ERROR;

      return true;
    }

    return false;
  }
}
