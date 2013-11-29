#import <Foundation/Foundation.h>
#import "MSVTexture.h"

/** Class representing a texture, used on a mesh */
@interface Texture : NSObject

/** The pixels of this texture, as RGBA */
@property (nonatomic) unsigned char *pixels;
/** The width of the texture in pixels */
@property unsigned int width;
/** The height of the texture in pixels */
@property unsigned int height;
/** 
 * The number of channels in this texture: currently 
 * only RGBA is supported, so this value is set to `4`.
 */
@property unsigned int channelCount;

/** 
 * Initializes a new Texture from a `CGImageRef` object.
 * @param img the `CGImageRef` object from which to initialize
 * a Texture.
 * @return the Texture object.
 */
- (id)initWithCGImage:(CGImageRef)img;

/** 
 * Initializes a new Texture from a `UIImage` object.
 * @param img the `UIImage` from which to initialize a Texture.
 * @return the Texture object.
 */
- (id)initWithUIImage:(UIImage *)img;

@end