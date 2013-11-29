#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>

#import "MSAvailability.h"
#import "MSScanner.h"
#import "MSImage.h"
#import "MSResult.h"
#import "MSObjC.h"

@protocol MoodstocksControllerDelegate;

/** 
 * Class wrapping the Moodstocks scanner.
 * **This is an internal class, only expert users might
 * have to modify it!**
 */
@interface MoodstocksController : NSObject <MSScannerDelegate>

/** The `MoodstocksControllerDelegate` to notify. */
@property (nonatomic, weak) id<MoodstocksControllerDelegate> delegate;

/**
 * Initializer.
 * @param scanner the **opened** `MSScanner` to use.
 * @param delegate the `MoodstocksControllerDelegate` to notify.
 * @return the MoodstocksController object.
 */
- (id)initWithScanner:(MSScanner *)scanner delegate:(id<MoodstocksControllerDelegate>)delegate;

/**
 * Performs a Moodstocks local search. Result will be notified to the 
 * `MoodstocksControllerDelegate` provided at initialization.
 * @param pixels the input image pixels, as a **grayscale, 8bit per pixel** image.
 * @param width the input image width.
 * @param height the input image height.
 * @param stride the input image stride, *i.e.* number of bytes per row. Useful
 * if the input image has memory alignment.
 */
- (void)scanPixels:(const unsigned char *)pixels width:(int)width height:(int)height stride:(int)stride;

/**
 * Notifies the MoodstocksController that the UI orientation has changed,
 * and that the camera frames should be rotated accordingly.
 * @param orientation the new `UIInterfaceOrientation`.
 */
- (void)setOrientation:(UIInterfaceOrientation)orientation;

@end

/** 
 * Protocol implemented to receive notifications from the MoodstocksController.
 * **This is an internal protocol, only expert users might have to modify it!**
 */
@protocol MoodstocksControllerDelegate <NSObject>

/** 
 * Notifies that the Moodstocks SDK has successfully scanned a frame.
 * @param result the result of this scan: will be null if nothing was
 * found, non null otherwise.
 */
- (void)didScan:(MSResult *)result;

/** Notifies that the Moodstocks SDK has failed at scanning a frame.
 * @param error the error that caused the failure.
 */
- (void)failedToScan:(NSError *)error;

@end
