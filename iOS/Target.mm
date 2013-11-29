#import "Target.h"

@implementation Target

- (id)initWithResult:(MSResult *)result {
    self = [super init];
    if (self) {
        _name = [result getValue];
        [result getDimensions:&_dimensions];
    }
    return self;
}

@end
