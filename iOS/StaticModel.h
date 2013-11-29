#import "AbstractModel.h"
#import "Texture.h"

/** 
 * Class representing a "static" model, *i.e.* a simple
 * mesh and texture.
 */
@interface StaticModel : AbstractModel

/** The `Texture` object applied to the Mesh */
@property (nonatomic, readonly) Texture *texture;

/** The 3D `Mesh` object */
@property (nonatomic) Mesh *mesh;

/**
 * Initializes a new Model with a `Mesh`.
 * **Should not be used until `Mesh` is implemented**
 * @param tex the `Texture` to apply to the `Mesh`.
 * @param mesh the `Mesh` object to display.
 * @param scale the scale to use when displaying this model, as
 * an array of floats: `{scaleX, scaleY, scaleZ}`.
 * @return the Model object.
 */
- (id)initWithTexture:(Texture *)tex mesh:(Mesh *)mesh scale:(const float[3])scale;

/**
 * Initializes a 2D Model made of a 2x2 textured plane.
 * @param tex the texture to display.
 * @param scale the scale to use when displaying this model, as
 * an array of floats: `{scaleX, scaleY, scaleZ}`. `scaleZ` does
 * not matter here and should be set to 1.
 * @return the Model object.
 */
- (id)initWithTexture:(Texture *)tex scale:(const float[3])scale;

@end
