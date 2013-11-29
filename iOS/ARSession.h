#import <Foundation/Foundation.h>

#import "AbstractModel.h"
#import "MSResult.h"

@protocol ARSessionDelegate;

/**
 * Main class, controlling the Vuforia SDK, the Moodstocks SDK and their interactions.
 */
@interface ARSession : NSObject

/** The `ARSessionDelegate` to notify. */
@property (nonatomic, weak) id<ARSessionDelegate> delegate;

/**
 * Adds a bundled Vuforia dataset to the tracker. **Must be called
 * before `initWithDelegate:error:`**
 * @param dataset the name of the `.dat` and `.xml` files generated
 * using the Vuforia Target Manager (without file extension), and
 * bundled with the app in the `assets` directory.
 * @return true if the dataset exists and has been loaded, false
 * otherwise.
 */
+ (BOOL)addDataset:(NSString *)dataset;

/**
 * Initializes the ARSession.
 * @param delegate the `ARSessionDelegate` object to notify.
 * @param error the pointer to a `NSError` object filled with
 * the error that caused this method to fail, if it failed.
 * @return the ARSession object
 */
- (id)initWithDelegate:(id<ARSessionDelegate>)delegate error:(NSError *__autoreleasing *)error;

/**
 * Inflates the camera preview and AR view inside the provided
 * **loaded** `UIView` object.
 * @param view the `UIView` in which to inflate the camera preview
 * and AR view.
 */
- (void)setARView:(UIView *)view;

/**
 * Notify the ARSession that the UI orientation will change, so
 * it can adapt to the new orientation.
 * Should be called from the `willAnimateRotationToInterfaceOrientation: duration:`
 * method of your view controller.
 * @param orientation the future `UIInterfaceOrientation`.
 */
- (void)orientationWillChange:(UIInterfaceOrientation)orientation;

/** 
 * Starts the session.
 * @param orientation the current UI orientation
 * @param error the pointer to an error that will be filled if
 * the camera cannot be started.
 */
- (void)startWithUIOrientation:(UIInterfaceOrientation)orientation error:(NSError *__autoreleasing *)error;

/** Resumes the session after a call to `pause`. */
- (void)resume;

/** 
 * Pauses the session, *i.e.* the camera preview keeps
 * displaying, but the frames are not being processed anymore
 * and the `delegate` won't be called until `resume` is called.
 */
- (void)pause;

/** Stops the session, closing the camera and stopping its preview */
- (void)stop;

@end

/** Protocol implemented to control the behavior of the `ARSession` */
@protocol ARSessionDelegate <NSObject>

/**
 * Called when the Moodstocks Scanner returns a new non-null result.
 * @param result the incoming result.
 * @return the name of the dataset to use for tracking if you wish this target
 * to be tracked, null otherwise.
 */
- (NSString *)onResultFound:(MSResult *)result;

/**
 * Method called asynchronously to build the `Model` to display
 * from the name and dimensions of the found result.
 *
 * IMPORTANT: The returned object will be displayed in the scene using 
 * the conventions on coordinate systems used in this project, i.e for 
 * a given target:
 *
 * - the origin is located at the center of the target, the X axis points
 * to its right, the Y axis to its top, and the Z axis follows the right
 * hand rule.
 *
 * - this coordinate system is orthonormal, with the norm given by half the
 * **smallest** dimension of the target. For example, if the indexed target
 * is 450px wide and 600px long, the X axis will go from the center to the right
 * edge of the target, while the Y axis will go from the center to 0.75% of the
 * top of the image.
 *
 * Implications on examples:
 *
 * - Displaying a 2x2 plane on a square target will exactly fit the target.
 *
 * - Displaying a 3x2 plane on a rectangular 300x200px (or any 3:2 ratio)
 *    target will exactly fit the target, so will a 2x3 plane on a rectangular
 *    200x300px target.
 *
 * - If displaying a more complex 3D model, you should probably ensure that
 *   its x and y coordinates are in the [-1..1] range.
 *
 * @param name the tracked target name
 * @param size the size of the found target in pixels , as returned by the 
 * Moodstocks SDK, *i.e.* corresponding to the size of the indexed image.
 * @return the Model to display.
 */
- (AbstractModel *)buildModelWithName:(NSString *)name size:(CGSize)size;

@end