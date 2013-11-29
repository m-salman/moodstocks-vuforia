#import "DynamicModel.h"

@implementation DynamicModel

- (id)initWithDelegate:(id<DynamicModelDelegate>)delegate scale:(const float[3])scale {
    self = [super init];
    if (self) {
        _delegate = delegate;
        float *tmp = new float[3];
        memcpy(tmp, scale, 3*sizeof(float));
        [super setScale:tmp];
    }
    return self;
}

- (void)dealloc {
    _delegate = nil;

}

@end
