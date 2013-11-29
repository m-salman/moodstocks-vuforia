#include "MSVMesh.h"
#include "MSVPlane.h"

#include <string.h>

MSVMesh::MSVMesh() :
nVertices(0),
vertices(NULL),
normals(NULL),
texCoords(NULL),
nFaces(0),
faces(NULL)
{}

MSVMesh::MSVMesh(unsigned int nVertices,
                 float *vertices,
                 float *normals,
                 float *texCoords,
                 unsigned int nFaces,
                 float *faces)
{
  set(nVertices, vertices, normals, texCoords, nFaces, faces);
}

void
MSVMesh::set(unsigned int nVertices,
             float *vertices,
             float *normals,
             float *texCoords,
             unsigned int nFaces,
             float *faces)
{
  this->nVertices = nVertices;
  this->vertices  = new float[3*nVertices];
  memcpy(this->vertices, vertices, 3*nVertices*sizeof(float));
  this->normals   = new float[3*nVertices];
  memcpy(this->normals, normals, 3*nVertices*sizeof(float));
  this->texCoords = new float[2*nVertices];
  memcpy(this->texCoords, texCoords, 2*nVertices*sizeof(float));
  this->nFaces = nFaces;
  this->faces     = new float[3*nFaces];
  memcpy(this->faces, faces, 3*nFaces*sizeof(float));
}

MSVMesh::~MSVMesh()
{
  if (vertices)  delete [] vertices;
  if (normals)   delete [] normals;
  if (texCoords) delete [] texCoords;
  if (faces)     delete [] faces;
}

MSVMesh *
MSVMesh::getNormalizedPlane()
{
  MSVMesh *m = new MSVMesh();
  m->nVertices = NUM_PLANE_OBJECT_VERTEX;
  m->nFaces  = NUM_PLANE_OBJECT_FACES;
  m->vertices  = new float[3*m->nVertices];
  memcpy(m->vertices, planeVertices, 3*m->nVertices*sizeof(float));
  m->normals   = new float[3*m->nVertices];
  memcpy(m->normals, planeNormals, 3*m->nVertices*sizeof(float));
  m->texCoords = new float[2*m->nVertices];
  memcpy(m->texCoords, planeTexCoords, 2*m->nVertices*sizeof(float));
  m->faces     = new float[3*m->nFaces];
  memcpy(m->faces, planeIndices, 3*m->nFaces*sizeof(float));
  return m;
}

unsigned int
MSVMesh::getVerticesCount() const
{
  return this->nVertices;
}

const float *
MSVMesh::getVertices() const
{
  return this->vertices;
}

const float *
MSVMesh::getNormals() const
{
  return this->normals;
}

const float *
MSVMesh::getTexCoords() const
{
  return this->texCoords;
}

unsigned int
MSVMesh::getFacesCount() const
{
  return this->nFaces;
}

const float *
MSVMesh::getFaces() const
{
  return this->faces;
}
