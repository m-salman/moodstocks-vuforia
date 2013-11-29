package com.moodstocks.vuforia;

/** Representation of an augmented reality video model */
public class DynamicModel extends AbstractModel {

  /** Interface implemented to control the video.
   */
  public static interface Callback {
    /**
     * Called at each rendered frame to ask for an update of the
     * displayed texture.
     * @param texCoordTransform the 4x4 transformation matrix to apply to the texture
     * coordinates.  This transform matrix maps 2D homogeneous texture coordinates of
     * the form (s, t, 0, 1) with s and t in the inclusive range [0, 1] to the texture
     * coordinate that should be used to sample that location from the texture
     * (see {@link android.graphics.SurfaceTexture#getTransformMatrix(float[])}).
     * By default, it is filled with the identity. If you wish to modify it, you should
     * fill it with the desired 4x4 matrix in <b>colum-major</b> order.
     * @return the texture ID. Must correspond to a GL_TEXTURE_EXTERNAL_OES
     * texture.
     */
    public int getTexture(float texCoordTransform[]);
    /** Called when the target stops being tracked, so the video can be stopped */
    public void stop();
  }

  private Callback cb;

  /**
   * Constructor.
   * @param callback the {@link Callback} controlling the video.
   * @param scale the scaling to apply to the video, as {@code [scaleX, scaleY, scaleZ]}.
   */
  public DynamicModel(Callback callback, float[] scale) {
    this.cb = callback;
    this.s = scale;
  }

  /** {@link Callback} accessor */
  public Callback getCallback() {
    return cb;
  }

}
