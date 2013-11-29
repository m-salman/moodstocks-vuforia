#import <QCAR/UIGLViewProtocol.h>

/** Class wrapping the OpenGL view
 * **This is an internal class, only expert users might
 * have to modify it!**
 */
@interface GLView : UIView <UIGLViewProtocol>

/** 
 * Initializes a GLView inside a parent UIView.
 * @param parent the parent UIView.
 * @return the GLView object.
 */
- (id)initInView:(UIView *)parent;

/** 
 * Adapts the display to the UI orientation.
 * @param orientation the new `UIInterfaceOrientation`.
 */
- (void)setOrientation:(UIInterfaceOrientation)orientation;

@end
