#import "AbstractModel.h"
#include "MSVTextureCallback.h"

@protocol DynamicModelDelegate;

/** Class representing a "dynamic" model, *i.e.*
 * a simple plane with a texture that changes along
 * time.
 */
@interface DynamicModel : AbstractModel

/** The `DynamicModelDelegate` */
@property (nonatomic, weak) id<DynamicModelDelegate> delegate;

/**
 * Initializes a new `DynamicModel` object.
 * @param delegate the `DynamicModelDelegate` to call from frame rendering.
 * @param scale the scale to use when displaying this model, as
 * an array of floats: `{scaleX, scaleY, scaleZ}`.
 */
- (id)initWithDelegate:(id<DynamicModelDelegate>)delegate scale:(const float[3])scale;

@end

/** 
 * Protocol implemented to display a texture that changes
 * along time.
 */
@protocol DynamicModelDelegate <NSObject>

/** 
 * Called each time the renderer needs a new frame to display.
 * @return the OpenGL Texture ID of a valid texture to display.
 */
- (GLuint)getTexture;

/** Called when the tracking stops or is lost. */
- (void)stop;

@end