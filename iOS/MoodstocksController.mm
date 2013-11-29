#import "MoodstocksController.h"
#include "moodstocks_sdk.h"

@interface MoodstocksController () {
    MSScanner *_scanner;
    const unsigned char *_pixels;
    int _width;
    int _height;
    int _stride;
    BOOL _locked;
    ms_ori_t _orientation;
}

- (void)scan;
- (void)scanComplete:(MSResult *)result;
- (void)scanFailed:(NSError *)error;

@end

@implementation MoodstocksController

- (id)initWithScanner:(MSScanner *)scanner delegate:(id<MoodstocksControllerDelegate>)delegate {
    self = [super init];
    if (self) {
        _scanner = scanner;
        _pixels = nil;
        _delegate = nil;
        _locked = NO;
        _delegate = delegate;
    }
    return self;
}

- (void)dealloc {
    _delegate = nil;
    _pixels = nil;
}

- (void)setOrientation:(UIInterfaceOrientation)orientation {
    switch (orientation) {
        case UIInterfaceOrientationPortrait:
            _orientation = MS_LEFT_BOTTOM_ORI;
            break;

        case UIInterfaceOrientationLandscapeRight:
            _orientation = MS_TOP_LEFT_ORI;
            break;

        case UIInterfaceOrientationLandscapeLeft:
            _orientation = MS_BOTTOM_RIGHT_ORI;
            break;

        case UIInterfaceOrientationPortraitUpsideDown:
            _orientation = MS_RIGHT_TOP_ORI;
            break;

        default:
            _orientation = MS_UNDEFINED_ORI;
            break;
    }
}

- (void)scanPixels:(const unsigned char *)pixels width:(int)width height:(int)height stride:(int)stride {
    if (!_locked) {
        _locked = YES;
        _pixels = pixels;
        _width = width;
        _height = height;
        _stride = stride;
        [self performSelectorInBackground:@selector(scan) withObject:nil];
    }
}

- (void)scan {
#if MS_SDK_REQUIREMENTS
    ms_img_t *img = NULL;
    ms_result_t *res = NULL;
    ms_errcode err = MS_SUCCESS;
    err = ms_img_new(_pixels, _width, _height, _stride, MS_PIX_FMT_GRAY8, _orientation, &img);
    if (err == MS_SUCCESS) {
        ms_scanner_search2([_scanner handle], img, &res, MS_SEARCH_SMALLTARGET);
        ms_img_del(img);
    }
    if (err != MS_SUCCESS) {
        NSError *error = [NSError errorWithDomain:@"moodstocks-sdk" code:err userInfo:nil];
        [self performSelectorOnMainThread:@selector(scanFailed:) withObject:error waitUntilDone:NO];
    }
    else {
        MSResult *result = nil;
        if (res) {
            result = [[MSResult alloc] initWithResult:res];
            ms_result_del(res);
        }
        [self performSelectorOnMainThread:@selector(scanComplete:) withObject:result waitUntilDone:NO];
    }
#endif
}

- (void)scanFailed:(NSError *)error {
    _locked = NO;
    if (_delegate && [_delegate respondsToSelector:@selector(failedToScan:)])
        [_delegate performSelector:@selector(failedToScan:) withObject:error];
}

- (void)scanComplete:(MSResult *)result {
    _locked = NO;
    if (_delegate && [_delegate respondsToSelector:@selector(didScan:)])
        [_delegate performSelector:@selector(didScan:) withObject:result];
}

@end
