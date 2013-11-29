#import <Foundation/Foundation.h>
#import "MSResult.h"

/** 
 * Small class representing a tracking target.
 * **This is an internal class, only expert users might
 * have to modify it!**
 */
@interface Target : NSObject

/** 
 * Initializes a Target object from a Moodstocks result.
 * @param result the `MSResult` object returned by the Moodstocks SDK.
 * @return the Target object.
 */
- (id)initWithResult:(MSResult *)result;

/** The target name */
@property (getter = getName) NSString *name;
/** The target dimensions */
@property (getter = getDimensions) CGSize dimensions;

@end
