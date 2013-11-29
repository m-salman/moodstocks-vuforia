#import "VuforiaController.h"
#import "StaticModel.h"
#import "DynamicModel.h"
#import <QCAR/QCAR.h>
#import <QCAR/QCAR_iOS.h>
#import <QCAR/Image.h>
#include "MSVController.h"
#include "MSVCallback.h"
#include "MSVCamera.h"
#include "MSVTargetInfo.h"
#include "MSVTexture.h"
#include "MSVMesh.h"

#pragma mark - C++ `MSVCallback` subclass declaration

@protocol MSVCallbackImplDelegate;

class MSVCallbackImpl : public MSVCallback {
public:
    MSVCallbackImpl(id<MSVCallbackImplDelegate> d);
protected:
    void onStatusUpdate();
    void getFrame(const QCAR::Image *frame) const;
private:
    id<MSVCallbackImplDelegate> __weak delegate;
};

@protocol MSVCallbackImplDelegate <NSObject>
- (void)onStatusUpdate;
- (void)getFrame:(const QCAR::Image *)frame;
@end

#pragma mark - C++ `MSVTexture` subclass declaration

class MSVTextureImpl : public MSVTexture {
public:
    MSVTextureImpl(Texture *t);
};

#pragma mark - C++ `MSVMesh` subclass declaration

class MSVMeshImpl : public MSVMesh {
public:
    MSVMeshImpl(Mesh *m);
};

#pragma mark - C++ `MSVTextureCallback` subclass declaration

/* 
 * Transforms the texture coordinates so that the video does not
 * appear upside-down.
 */
static const float defaultTexCoordTransform[16] = {1,  0,  0,  0,
                                                   0, -1,  0,  0,
                                                   0,  0,  1,  0,
                                                   0,  1,  0,  1};

class MSVTextureCallbackImpl : public MSVTextureCallback {
public:
    MSVTextureCallbackImpl(id<DynamicModelDelegate> d);
    ~MSVTextureCallbackImpl();
    GLuint getTexture(float mtx[16]);
    void stop();
private:
    id<DynamicModelDelegate> __weak delegate;
};

#pragma mark - VuforiaController private interface

@interface VuforiaController () <MSVCallbackImplDelegate>

@property (nonatomic) MSVCallbackImpl *cb;

@property BOOL initFailed;

- (void)loadDatasets;
- (BOOL)startCamera;
- (void)stopCamera;

@end

#pragma mark - VuforiaController implementation

@implementation VuforiaController

static NSMutableArray *_datasets = [NSMutableArray arrayWithCapacity:15];

+ (BOOL)addDataset:(NSString *)dataset {
    NSString *path = [[NSBundle mainBundle] pathForResource:dataset ofType:@"xml"];
    if (![[NSFileManager defaultManager] fileExistsAtPath:path]) return NO;
    path = [[NSBundle mainBundle] pathForResource:dataset ofType:@"dat"];
    if (![[NSFileManager defaultManager] fileExistsAtPath:path]) return NO;
    [_datasets addObject:dataset];
    return YES;
}

- (id)initWithDelegate:(id<VuforiaControllerDelegate>)delegate error:(NSError *__autoreleasing *)error {
    if (!delegate) return nil;
    self = [super init];
    if (self) {
        _delegate = delegate;
        _pixels = nil;
        _width = -1;
        _height = -1;
        _stride = -1;
        _cb = new MSVCallbackImpl(self);
        _initFailed = NO;
        QCAR::setInitParameters(QCAR::GL_20);
        NSInteger initSuccess = 0;
        do {
            initSuccess = QCAR::init();
        } while (0 <= initSuccess && 100 > initSuccess);
        if (initSuccess != 100) {
            NSString *errMsg = (initSuccess == QCAR::INIT_DEVICE_NOT_SUPPORTED) ?
                @"Vuforia Error: Device is not supported" :
                @"Vuforia Error: Initialization failed";
            *error = [NSError errorWithDomain:@"Vuforia-SDK"
                                         code:0
                                     userInfo:[NSDictionary dictionaryWithObject:errMsg forKey:NSLocalizedDescriptionKey]];
            if (_cb) delete _cb;
            _cb = nil;
            _initFailed = YES;
            return nil;
        }
        MSVController::init();
        MSVController::registerCallback(_cb);

        [self performSelectorInBackground:@selector(loadDatasets) withObject:nil];
    }
    return self;
}

- (void)loadDatasets {
    for (NSString *dataset : _datasets) {
        MSVController::addDataset([dataset cStringUsingEncoding:NSUTF8StringEncoding]);
    }
}

- (void)dealloc {
    MSVController::unregisterCallback();
    MSVController::deInit();
    QCAR::deinit();
    if (_pixels) delete _pixels;
    _pixels = nil;
    if (_cb) delete _cb;
    _cb = nil;
    _delegate = nil;
    [_datasets removeAllObjects];
}

- (void)start:(NSError *__autoreleasing *)error {
    if (_initFailed) return;
    QCAR::onResume();
    if (![self startCamera]) {
        NSString *errMsg = @"Could not access the device camera";
        *error = [NSError errorWithDomain:@"Vuforia-SDK"
                                     code:0
                                 userInfo:[NSDictionary dictionaryWithObject:errMsg forKey:NSLocalizedDescriptionKey]];
    }
}

- (void)stop {
    if (_initFailed) return;
    [self stopTracking];
    QCAR::onPause();
    [self stopCamera];
}

- (void)startTrackingTarget:(Target *)target inDataset:(NSString *)dataset {
    if (_initFailed) return;
    const char *name = [[target getName] cStringUsingEncoding:NSUTF8StringEncoding];
    const char *data = [dataset cStringUsingEncoding:NSUTF8StringEncoding];
    CGSize s = [target getDimensions];
    int dims[2] = {(int)s.width, (int)s.height};
    MSVController::startTracking(name, dims, data);
}

- (void)stopTracking {
    if (_initFailed) return;
    MSVController::stopTracking();
}

- (BOOL)isTracking {
    if (_initFailed) return NO;
    return MSVController::isTracking();
}

- (Target *)getCurrentTarget {
    if (_initFailed) return nil;
    const MSVTargetInfo *i = MSVController::getCurrentTarget();
    if (!i) return nil;
    Target *t = [[Target alloc] init];
    [t setName:[NSString stringWithCString:i->getName() encoding:NSUTF8StringEncoding]];
    CGSize s = CGSizeMake(i->getWidth(), i->getHeight());
    [t setDimensions:s];
    return t;
}

- (void)changeCurrentModel:(AbstractModel *)model {
    if (_initFailed) return;
    MSVMesh *mesh = NULL;
    if ([model isDynamic]) {
        DynamicModel *mod = (DynamicModel *)model;
        MSVTextureCallback *cb = NULL;
        id<DynamicModelDelegate> d = [mod delegate];
        if (d) cb = new MSVTextureCallbackImpl(d);
        MSVController::setDynamicModel(cb, [mod scale]);
    }
    else {
        StaticModel *mod = (StaticModel *)model;
        Mesh *m = [mod mesh];
        if (m) mesh = new MSVMeshImpl(m);
        MSVTexture *tex = NULL;
        Texture *t = [mod texture];
        if (t) tex = new MSVTextureImpl(t);
        MSVController::setStaticModel(mesh, tex, [mod scale]);
    }
}

- (BOOL)startCamera {
    return MSVCamera::start();
}

- (void)stopCamera {
    MSVCamera::stop();
}

- (void)requireUpdate {
    if (_initFailed) return;
    _cb->MSVCallback::requireUpdate();
}

- (BOOL)getFrame {
    if (_initFailed) return NO;
    return _cb->MSVCallback::getFrame() ? YES : NO;
}

- (BOOL)isNewTarget {
    if (_initFailed) return NO;
    return _cb->MSVCallback::isNewTarget();
}

- (BOOL)isTargetLost {
    if (_initFailed) return NO;
    return _cb->MSVCallback::isTargetLost();
}

- (void)onStatusUpdate {
    if (_initFailed) return;
    [_delegate onStatusUpdate];
}

- (void)getFrame:(const QCAR::Image *)frame {
    if (_initFailed) return;
    const void *p = frame->getPixels();
    int w = frame->getWidth();
    int h = frame->getHeight();
    int s = frame->getStride();
    if (!_pixels) {
        _pixels = new unsigned char[s*h];
        _width = w;
        _height = h;
        _stride = s;
    }
    memcpy(_pixels, p, s*h);
}

@end

#pragma mark - C++ `MSVCallback` subclass implementation

MSVCallbackImpl::MSVCallbackImpl(id<MSVCallbackImplDelegate> d)
{
    delegate = d;
}

void
MSVCallbackImpl::onStatusUpdate()
{
    if (delegate) [delegate onStatusUpdate];
}

void
MSVCallbackImpl::getFrame(const QCAR::Image *frame) const
{
    if (delegate) [delegate getFrame:frame];
}

#pragma mark - C++ `MSVTexture` subclass implementation

MSVTextureImpl::MSVTextureImpl(Texture *t) :
MSVTexture()
{
    set([t pixels], [t width], [t height]);
}

#pragma mark - C++ `MSVMesh` subclass implementation

MSVMeshImpl::MSVMeshImpl(Mesh *m) :
MSVMesh()
{
    // TODO
}

#pragma mark - C++ `MSVTextureCallback` subclass implementation

MSVTextureCallbackImpl::MSVTextureCallbackImpl(id<DynamicModelDelegate> d)
{
    delegate = d;
}

MSVTextureCallbackImpl::~MSVTextureCallbackImpl()
{
    delegate = nil;
}

GLuint
MSVTextureCallbackImpl::getTexture(float mtx[16])
{
    memcpy(mtx, defaultTexCoordTransform, 16*sizeof(float));
    if (delegate) return [delegate getTexture];
    return 0;
}

void
MSVTextureCallbackImpl::stop()
{
    if (delegate) [delegate stop];
}
