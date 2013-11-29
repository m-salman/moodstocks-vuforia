package com.moodstocks.vuforia;

import android.graphics.Bitmap;
import android.graphics.Color;

/**
 * Class representing a Texture to apply on a 3D model.
 */
public class Texture
{
    /**
     * These fields are actually called from JNI, this is why
     * we suppress the "unused" warnings
     */
    @SuppressWarnings("unused")
    private int width;          /// The width of the texture.
    @SuppressWarnings("unused")
    private int height;         /// The height of the texture.
    @SuppressWarnings("unused")
    private int channelCount;   /// The number of channels.
    @SuppressWarnings("unused")
    private byte[] data;        /// The pixel data.

    /**
     * Creates a new Texture from a {@link Bitmap}.
     * @param bmp the bitmap from which to create the texture
     * @return the texture.
     */
    public static Texture textureFromBitmap(Bitmap bmp) {
      int w = bmp.getWidth();
      int h = bmp.getHeight();
      int[] data = new int[w*h];
      bmp.getPixels(data, 0, w, 0, 0, w, h);

      // Convert:
      byte[] dataBytes = new byte[4*w*h];
      for (int p = 0; p < w*h; ++p)
      {
          int c = data[p];
          dataBytes[p * 4]        = (byte)Color.red(c);    // R
          dataBytes[p * 4 + 1]    = (byte)Color.green(c);  // G
          dataBytes[p * 4 + 2]    = (byte)Color.blue(c);   // B
          dataBytes[p * 4 + 3]    = (byte)Color.alpha(c);  // A
      }

      Texture texture = new Texture();
      texture.width         = w;
      texture.height        = h;
      texture.channelCount  = 4;
      texture.data          = dataBytes;

      return texture;
    }
}
