#import "StaticModel.h"
#include <string.h>

@implementation StaticModel

- (id)initWithTexture:(Texture *)tex mesh:(Mesh *)mesh scale:(const float[3])scale {
    self = [super init];
    if (self) {
        _texture = tex;
        _mesh = mesh;
        float *tmp = new float[3];
        memcpy(tmp, scale, 3*sizeof(float));
        [super setScale:tmp];
    }
    return self;
}

- (id)initWithTexture:(Texture *)tex scale:(const float[3])scale {
    return [self initWithTexture:tex mesh:nil scale:scale];
}

@end
