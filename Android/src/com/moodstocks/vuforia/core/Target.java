package com.moodstocks.vuforia.core;

import com.moodstocks.android.Result;

/**
 * Class representing the information about a tracking target.
 */
public class Target {

  private String name = null;
  private int width = -1;
  private int height = -1;

  private Target(String n, int[] dims) {
    name = n;
    width = dims[0];
    height = dims[1];
  }

  /**
   * Create a new Target from a {@link com.moodstocks.android.Result} object
   * @param r the result to use
   * @return the target.
   */
  public static Target fromResult(Result r) {
    if (r == null) return null;
    return new Target(r.getValue(), r.getDimensions());
  }

  /**
   * Target name accessor
   * @return the name
   */
  public String getName() {
    return name;
  }

  /**
   * Target width accessor
   * @return the width
   */
  public int getWidth() {
    return width;
  }

  /**
   * Target height accessor
   * @return the height
   */
  public int getHeight() {
    return height;
  }

}
