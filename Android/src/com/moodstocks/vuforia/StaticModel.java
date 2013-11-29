package com.moodstocks.vuforia;

/**
 * Representation of a simple textured 3D mesh to display in
 * augmented reality.
 */
public class StaticModel extends AbstractModel {

  private Mesh m = null;
  private Texture t = null;

  /**
   * Creates a new Model from {@link Mesh} and {@link Texture}.
   * @param model the {@link Mesh} to display
   * @param tex the corresponding {@link Texture}
   * @param scale the scaling to apply to the mesh, as {@code [scaleX, scaleY, scaleZ]}.
   */
  public StaticModel(Mesh model, Texture tex, float[] scale) {
    this.m = model;
    this.t = tex;
    this.s = scale;
  }

  /**
   * Creates a new 2x2 textured plane.
   * @param tex the {@link Texture}
   * @param scale the scaling to apply to the plane, as {@code [scaleX, scaleY, scaleZ]}.
   */
  public StaticModel(Texture tex, float[] scale) {
    this.m = null;
    this.t = tex;
    this.s = scale;
  }

  /** {@link Mesh} accessor */
  public Mesh getMesh() {
    return m;
  }

  /** {@link Texture} accessor */
  public Texture getTexture() {
    return t;
  }

}
