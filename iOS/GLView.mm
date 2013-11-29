#import "GLView.h"

#import <QuartzCore/QuartzCore.h>
#import <QCAR/QCAR.h>
#import <QCAR/VideoBackgroundConfig.h>

#import "MSVController.h"
#import "MSVRenderer.h"
#import "MSVState.h"

@interface GLView () {
    GLuint defaultFramebuffer;
    GLuint colorRenderbuffer;
    GLuint depthRenderbuffer;
    EAGLContext *context;
}

@property UIView *parent;
@property CGSize glSize;
@property BOOL inited;

@property BOOL isRendering;
@property BOOL hasChanged;

- (void)setFramebuffer;
- (BOOL)presentFramebuffer;
- (void)createFramebuffer;
- (void)deleteFramebuffer;

@end


@implementation GLView

// You must implement this method
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

// test to see if the screen has hi-res mode
- (BOOL) isRetinaEnabled
{
    return ([[UIScreen mainScreen] respondsToSelector:@selector(displayLinkWithTarget:selector:)]
            &&
            ([UIScreen mainScreen].scale == 2.0));
}

- (id)initInView:(UIView *)parent
{
    CGRect frame = CGRectMake(0, 0, 1, 1);
    self = [super initWithFrame:frame];
	if (self) {
        _parent = parent;
        _inited = NO;
        _isRendering = NO;
        _hasChanged = NO;
        if ([self isRetinaEnabled])
        {
            self.contentScaleFactor = 2.0f;
        }
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;

        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                        nil];

        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

        if (!context) {
            // TODO: error management
            NSLog(@"Failed to create ES context");
        }
        [parent addSubview:self];
        QCAR::onSurfaceCreated();
    }
    return self;
}

- (void)dealloc
{
    [self deleteFramebuffer];

    // Tear down context
    if ([EAGLContext currentContext] == context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)layoutSubviews {
    if (_isRendering) {
        _hasChanged = YES;
    }
    else {
        [self deleteFramebuffer];
    }
    if (!_inited) {
        MSVController::initRenderer();
        _inited = YES;
    }
}

- (void)setOrientation:(UIInterfaceOrientation)orientation {

    CGSize parentSize = _parent.frame.size;
    NSInteger selfRotation;
    CGSize selfSize;
    if (orientation == UIInterfaceOrientationPortrait)
    {
        selfSize = CGSizeMake(parentSize.height, parentSize.width);
        selfRotation = 90;
    }
    else if (orientation == UIInterfaceOrientationPortraitUpsideDown)
    {
        selfSize = CGSizeMake(parentSize.height, parentSize.width);
        selfRotation = 270;
    }
    else if (orientation == UIInterfaceOrientationLandscapeLeft)
    {
        selfSize = CGSizeMake(parentSize.width, parentSize.height);
        selfRotation = 180;
    }
    else
    {
        selfSize = CGSizeMake(parentSize.width, parentSize.height);
        selfRotation = 0;
    }

    // Position view at the center of the parent view.
    self.layer.bounds = CGRectMake(0, 0, selfSize.width, selfSize.height);
    CGPoint center = {.x = parentSize.width/2, .y = parentSize.height/2};
    self.layer.position = center;
    CGAffineTransform rotate = CGAffineTransformMakeRotation(selfRotation * M_PI  / 180);
    self.transform = rotate;

    _glSize = selfSize;
}

- (void)createFramebuffer
{
    if (context && !defaultFramebuffer) {
        [EAGLContext setCurrentContext:context];

        GLint framebufferWidth, framebufferHeight;

        // Create default framebuffer object
        glGenFramebuffers(1, &defaultFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);

        // Create colour render buffer and allocate backing store
        glGenRenderbuffers(1, &colorRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);

        // Allocate the renderbuffer's storage (shared with the drawable object)
        [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &framebufferWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &framebufferHeight);

        // Create the depth render buffer and allocate storage
        glGenRenderbuffers(1, &depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, framebufferWidth, framebufferHeight);

        // Attach colour and depth render buffers to the frame buffer
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

        // Leave the colour render buffer bound so future rendering operations will act on it
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        }
    }
}

- (void)deleteFramebuffer
{
    if (context) {
        [EAGLContext setCurrentContext:context];

        if (defaultFramebuffer) {
            glDeleteFramebuffers(1, &defaultFramebuffer);
            defaultFramebuffer = 0;
        }

        if (colorRenderbuffer) {
            glDeleteRenderbuffers(1, &colorRenderbuffer);
            colorRenderbuffer = 0;
        }

        if (depthRenderbuffer) {
            glDeleteRenderbuffers(1, &depthRenderbuffer);
            depthRenderbuffer = 0;
        }
    }
}

- (void)setFramebuffer
{
    if (context) {
        [EAGLContext setCurrentContext:context];

        if (!defaultFramebuffer) {
            // Perform on the main thread to ensure safe memory allocation for
            // the shared buffer.  Block until the operation is complete to
            // prevent simultaneous access to the OpenGL context
            [self performSelectorOnMainThread:@selector(createFramebuffer) withObject:self waitUntilDone:YES];

            // Give the *actual GLView size* to the VuforiaWrapper.
            int f = [self isRetinaEnabled] ? 2 : 1;
            MSVState::setGLViewSize(_glSize.width*f, _glSize.height*f);
            MSVState::setPortrait(false);
            MSVController::getRenderer()->updateState();
            QCAR::onSurfaceChanged(_glSize.width*f, _glSize.height*f);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    }
}

- (BOOL)presentFramebuffer
{
    BOOL success = FALSE;

    if (context) {
        [EAGLContext setCurrentContext:context];
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
        success = [context presentRenderbuffer:GL_RENDERBUFFER];
    }

    return success;
}

- (void)renderFrameQCAR
{
    _isRendering = YES;
    [self setFramebuffer];

    MSVController::getRenderer()->renderFrame();

    [self presentFramebuffer];
    _isRendering = NO;

    if (_hasChanged) {
        [self deleteFramebuffer];
        _hasChanged = NO;
    }
}

@end
