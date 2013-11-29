#import "Texture.h"

@interface Texture ()

- (void)buildFromCGImage:(CGImageRef)img;

@end

@implementation Texture

- (id)initWithCGImage:(CGImageRef)img {
    self = [super init];
    if (self) {
        [self buildFromCGImage:img];
    }
    return self;
}


- (id)initWithUIImage:(UIImage *)img {
    self = [super init];
    if (self) {
        [self buildFromCGImage:[img CGImage]];
    }
    return self;
}

- (void)dealloc {
    if (_pixels) delete _pixels;
}

- (void)buildFromCGImage:(CGImageRef)img {

    NSUInteger width = CGImageGetWidth(img);
    NSUInteger height = CGImageGetHeight(img);
    NSUInteger bitsPerComponent = CGImageGetBitsPerComponent(img);
    NSUInteger bytesPerRow = CGImageGetBytesPerRow(img);
    unsigned char *rawData = new unsigned char[bytesPerRow*height];

    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(rawData, width, height,
                                                 bitsPerComponent, bytesPerRow, colorSpace,
                                                 kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), img);
    CGColorSpaceRelease(colorSpace);
    CGContextRelease(context);

    [self setWidth:width];
    [self setHeight:height];
    [self setChannelCount:4];
    [self setPixels:rawData];
}

@end
