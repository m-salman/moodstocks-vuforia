#import <Foundation/Foundation.h>
#import "Mesh.h"

/** "Abstract" parent class of `StaticModel` and `DynamicModel` */
@interface AbstractModel : NSObject

/** 
 * The scale to apply to this model when displayed,
 * as a floating point array: {scaleX, scaleY, scaleZ}
 */
@property (nonatomic) float *scale;

/** 
 * Get the nature of this model.
 * @return YES if the model is of class `DynamicModel`,
 * NO otherwise.
 */
- (BOOL)isDynamic;

/** 
 * Helper method: given the size of the target to track and the size of
 * a 2D model to display, returns the scale to apply to that model in order
 * for it to be displayed with the right aspect ratio while taking as much
 * space as possible on the target.
 * @param targetSize the target dimensions
 * @param modelSize the model dimensions
 * @return the scale to apply. It is allocated using `malloc` and should be
 * freed using `free` when not used anymore.
 */
+ (float *)scaleToFitTargetSize:(CGSize)targetSize modelSize:(CGSize)modelSize;

@end
