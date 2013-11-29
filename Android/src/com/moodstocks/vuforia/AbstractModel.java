package com.moodstocks.vuforia;

/** Common superclass of {@link StaticModel} and {@link DynamicModel} */
public abstract class AbstractModel {

  protected float[] s = null;

  /** Scale accessor */
  public float[] getScale() {
    return s;
  }

  /** Helper method to detect whether this instance is
   * of type {@link StaticModel} or {@link DynamicModel}.
   * @return true if this is a DynamicModel, false otherwise.
   */
  public boolean isDynamic() {
    return (this instanceof DynamicModel);
  }

  /** Helper method: given the size of the target to track and the size of
   * a 2D model to display, returns the scale to apply to that model in order
   * for it to be displayed with the right aspect ratio while taking as much
   * space as possible on the target.
   * @param targetX the target width
   * @param targetY the target height
   * @param modelX the model width
   * @param modelY the model height
   * @return the scale to apply.
   */
  public static float[] scaleFitRatio(float targetX, float targetY,
                                      float modelX,  float modelY) {
    float scale[] = {1.0f, 1.0f, 1.0f};
    if (targetX > targetY) {
      if ((float)targetX/targetY > (float)modelX/modelY) {
        scale[0]*=(float)modelX/modelY;
      }
      else {
        scale[0] = (float)targetX/targetY;
        scale[1] = scale[0]*(float)modelY/modelX;
      }
    }
    else {
      if ((float)targetX/targetY > (float)modelX/modelY) {
        scale[1] = (float)targetY/targetX;
        scale[0] = scale[1]*(float)modelX/modelY;
      }
      else {
        scale[1] *= (float)modelY/modelX;
      }
    }
    return scale;
  }

}
