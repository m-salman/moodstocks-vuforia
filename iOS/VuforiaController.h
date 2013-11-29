#import <Foundation/Foundation.h>
#import "Target.h"
#import "AbstractModel.h"

@protocol VuforiaControllerDelegate;

/**
 * Class wrapping the Vuforia SDK.
 * **This is an internal class, only expert users might
 * have to modify it!**
 */
@interface VuforiaController : NSObject

/** The `VuforiaControllerDelegate` to notify. */
@property (nonatomic, weak) id<VuforiaControllerDelegate> delegate;

/** The camera frame pixels. */
@property (atomic) unsigned char *pixels;
/** The camera frame width. */
@property int width;
/** The camera frame height. */
@property int height;
/** The camera frame stride, in bytes. */
@property int stride;

/**
 * Adds a bundled Vuforia dataset to the tracker. **Must be called
 * before `initWithDelegate:error:`**
 * @param name the name of the `.dat` and `.xml` files generated
 * using the Vuforia Target Manager (without file extension), and
 * bundled with the app in the `assets` directory.
 * @return true if the dataset exists and has been loaded, false
 * otherwise.
 */
+ (BOOL)addDataset:(NSString *)name;

/** 
 * Initializer.
 * @param delegate the `VuforiaControllerDelegate` to notify.
 * @param error the pointer to an error that will be filled with
 * the cause of any initialization failure.
 * @return the VuforiaController object.
 */
- (id)initWithDelegate:(id<VuforiaControllerDelegate>)delegate error:(NSError *__autoreleasing *)error;

/**
 * Starts the VuforiaController, which has the effect of:
 * - starting the Vuforia SDK
 * - starting the camera and starting to display the preview
 * - start providing callbacks through the delegate.
 * @param error the pointer to an error that will be filled if
 * the camera cannot be started.
 */
- (void)start:(NSError *__autoreleasing *)error;

/** Stops the Vuforia SDK, camera preview and callbacks. */
- (void)stop;

/**
 * Starts tracking a target.
 * Should be called only when not already tracking a target. Otherwise it will
 * automatically reset this Controller in order for a second call
 * to this function to have the desired effect.
 * @param target the `Target` to track.
 * @param dataset the name of the dataset containing this target.
 */
- (void)startTrackingTarget:(Target *)target inDataset:(NSString *)dataset;

/** Stops tracking the current target, if any. */
- (void)stopTracking;

/**
 * @return YES if the Vuforia SDK is currently tracking a target, NO otherwise.
 */
- (BOOL)isTracking;

/**
 * @return the currently tracked `Target` object, if any.
 */
- (Target *)getCurrentTarget;

/** 
 * Changes the `Model` displayed on the current target.
 * @param model the new `Model` object to use.
 */
- (void)changeCurrentModel:(AbstractModel *)model;

/** 
 * Requires a status update.
 * This will trigger a call to the `onStatusUpdate` method of the
 * delegate as soon as possible.
 */
- (void)requireUpdate;

/** 
 * Get the current camera frame.
 * This updates the values of `pixels`, `width`, `height` and `stride`
 * variables.
 * Should be called only from `onStatusUpdate` method.
 * @return `YES` if the frame could be fetched, `NO` otherwise.
 */
- (BOOL)getFrame;

/**
 * Check whether the currently tracked target, if any,
 * is a new target, i.e. this is the first status update
 * in which it is tracked.
 * Should be called only from `onStatusUpdate` method.
 * @return `YES` if a new target, `NO` otherwise/
 */
- (BOOL)isNewTarget;

/** 
 * Check whether the currently tracked target has been lost.
 * Should be called only from `onStatusUpdate` method.
 * @return `YES` if the target is lost, `NO` otherwise.
 */
- (BOOL)isTargetLost;

@end

/** 
 * Protocol implemented to receive status updates from the VuforiaController. 
 * ** This is an internal protocol, only expert users might have to modify it!**
 */
@protocol VuforiaControllerDelegate <NSObject>

/** Called shortly after each call to `requireUpdate`. */
- (void)onStatusUpdate;

@end