#import "AbstractModel.h"
#import "DynamicModel.h"
#include <string.h>

@implementation AbstractModel

- (void)dealloc {
    if (_scale) delete _scale;
}

- (BOOL)isDynamic {
    return [self isKindOfClass:[DynamicModel class]];
}

+ (float *)scaleToFitTargetSize:(CGSize)targetSize modelSize:(CGSize)modelSize {
    float *scale = (float *)malloc(3*sizeof(float));
    scale[2] = 1.0;
    if (targetSize.width > targetSize.height) {
        if (targetSize.width/targetSize.height > modelSize.width/modelSize.height) {
            scale[0] = modelSize.width/modelSize.height;
            scale[1] = 1;
        }
        else {
            scale[0] = targetSize.width/targetSize.height;
            scale[1] = scale[0]*modelSize.height/modelSize.width;
        }
    }
    else {
        if (targetSize.width/targetSize.height > modelSize.width/modelSize.height) {
            scale[1] = targetSize.height/targetSize.width;
            scale[0] = scale[1]*modelSize.width/modelSize.height;
        }
        else {
            scale[1] = modelSize.height/modelSize.width;
            scale[0] = 1;
        }
    }
    return scale;
}

@end
