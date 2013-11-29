#ifndef MSV_MESH_H
#define MSV_MESH_H

/** Class representing a 3D mesh.
 *
 * Currently, it is assumed that it will be rendered using the
 * GL_TRIANGLES mode.
 */
class MSVMesh {

  public:
    MSVMesh(unsigned int nVertices,
            float *vertices,
            float *normals,
            float *texCoords,
            unsigned int nFaces,
            float *faces);
    virtual ~MSVMesh();
    unsigned int getVerticesCount() const;
    const float *getVertices() const;
    const float *getNormals() const;
    const float *getTexCoords() const;
    unsigned int getFacesCount() const;
    const float *getFaces() const;

    /** Returns a 2x2 plane */
    static MSVMesh *getNormalizedPlane();

  protected:
    MSVMesh();
    void set(unsigned int nVertices,
             float *vertices,
             float *normals,
             float *texCoords,
             unsigned int nFaces,
             float *faces);

    private:
      unsigned int nVertices;
      float *vertices;
      float *normals;
      float *texCoords;
      unsigned int nFaces;
      float *faces;
};

#endif
