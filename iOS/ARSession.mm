#import "ARSession.h"
#import "GLView.h"
#import "VuforiaController.h"
#import "MoodstocksController.h"
#import "Target.h"

#define UNTRACK_DELAY 0.5 /*seconds*/

@interface ARSession () <MoodstocksControllerDelegate, VuforiaControllerDelegate>

@property (nonatomic) VuforiaController *vuforia;
@property (nonatomic) MoodstocksController *moodstocks;

@property (nonatomic) GLView *glView;
@property double lastFound;

@property (atomic) AbstractModel *builtModel;

@property BOOL paused;

- (void)buildModel;

@end

@implementation ARSession

+ (BOOL)addDataset:(NSString *)dataset {
    return [VuforiaController addDataset:dataset];
}

- (id)initWithDelegate:(id<ARSessionDelegate>)delegate error:(NSError *__autoreleasing *)error {
    if (!delegate) return nil;
    self = [super init];
    if (self) {
        _delegate = delegate;
        _vuforia = [[VuforiaController alloc] initWithDelegate:self error:error];
        _moodstocks = [[MoodstocksController alloc] initWithScanner:[MSScanner sharedInstance]
                                                           delegate:self];
        _glView = nil;
        _builtModel = nil;
        _paused = NO;
    }
    return self;
}

- (void)setARView:(UIView *)view {
    _glView = [[GLView alloc] initInView:view];
}

- (void)orientationWillChange:(UIInterfaceOrientation)orientation {
    [_glView setOrientation:orientation];
    [_moodstocks setOrientation:orientation];
}

- (void)dealloc {
    _delegate = nil;
    _vuforia = nil;
    _moodstocks = nil;
    _glView = nil;
    _builtModel = nil;
}

- (void)startWithUIOrientation:(UIInterfaceOrientation)orientation error:(NSError *__autoreleasing *)error {
    [self orientationWillChange:orientation];
    [_vuforia start:error];
}

- (void)resume {
    if (_paused) {
        _paused = NO;
        [_vuforia requireUpdate];
    }
}

- (void)pause {
    _paused = YES;
}

- (void)stop {
    [_vuforia stop];
}

- (void)onStatusUpdate {
    if (_paused) return;
    if ([_vuforia isTracking]) {
        if ([_vuforia isNewTarget]) {
            [self performSelectorInBackground:@selector(buildModel) withObject:nil];
        }
        else if (_builtModel) {
            [_vuforia changeCurrentModel:_builtModel];
            _builtModel = nil;
        }
        if (![_vuforia isTargetLost]) {
            _lastFound = [[NSDate date] timeIntervalSince1970];
        }
        else if ([[NSDate date] timeIntervalSince1970] - _lastFound > UNTRACK_DELAY){
            [_vuforia stopTracking];
        }
        [_vuforia requireUpdate];
    }
    else {
        [_vuforia getFrame];
        [_moodstocks scanPixels:[_vuforia pixels] width:[_vuforia width] height:[_vuforia height] stride:[_vuforia stride]];
    }
}

- (void)didScan:(MSResult *)result {
    if (_paused) return;
    if ([_vuforia isTracking]) return;
    if (result) {
        NSString *dataset = nil;
        if (_delegate) dataset = [_delegate onResultFound:result];
        if (dataset) {
            [_vuforia startTrackingTarget:[[Target alloc] initWithResult:result] inDataset:dataset];
        }
    }
    [_vuforia requireUpdate];
}

- (void)failedToScan:(NSError *)error {
    if (_paused) return;
    [_vuforia requireUpdate];
}

- (void)buildModel {
    Target *target = [_vuforia getCurrentTarget];
    if (!target) return;
    CGSize s = [target getDimensions];
    AbstractModel *m = nil;
    if (_delegate) m = [_delegate buildModelWithName:[target getName] size:s];
    _builtModel = m;
}


@end
