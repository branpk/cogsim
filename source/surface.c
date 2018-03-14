#include "surface.h"

#include "util.h"

#include <math.h>
#include <stdlib.h>


SurfaceNode allFloors;


static Surface surfacePool[100];
static s32 surfacesAllocated;

static SurfaceNode surfaceNodePool[100];
static s32 surfaceNodesAllocated;


void clearSurfaces(void) {
  allFloors.tail = NULL;
  surfacesAllocated = 0;
  surfaceNodesAllocated = 0;
}


static SurfaceNode *allocSurfaceNode(void) {
  SurfaceNode *node = &surfaceNodePool[surfaceNodesAllocated++];
  node->tail = NULL;
  return node;
}


static Surface *allocSurface(void) {
  Surface *tri = &surfacePool[surfacesAllocated++];
  tri->type = 0;
  tri->v02 = 0;
  tri->v04 = 0;
  tri->v05 = 0;
  tri->object = NULL;
  return tri;
}


static void addSurface(Surface *tri) {
  SurfaceNode *newNode = allocSurfaceNode();

  s16 triPriority = tri->vertex1.y;
  newNode->head = tri;
  
  SurfaceNode *list = &allFloors;

  while (list->tail != NULL) {
    s16 priority = list->tail->head->vertex1.y;
    if (triPriority > priority) break;
    list = list->tail;
  }

  newNode->tail = list->tail;
  list->tail = newNode;
}


static Surface *readSurfaceData(s16 *vertexData, s16 **data) {
  s16 offset1 = 3 * *(*data + 0);
  s16 offset2 = 3 * *(*data + 1);
  s16 offset3 = 3 * *(*data + 2);

  s32 x1 = *(vertexData + offset1 + 0);
  s32 y1 = *(vertexData + offset1 + 1);
  s32 z1 = *(vertexData + offset1 + 2);
  
  s32 x2 = *(vertexData + offset2 + 0);
  s32 y2 = *(vertexData + offset2 + 1);
  s32 z2 = *(vertexData + offset2 + 2);
  
  s32 x3 = *(vertexData + offset3 + 0);
  s32 y3 = *(vertexData + offset3 + 1);
  s32 z3 = *(vertexData + offset3 + 2);
  
  f32 nx = (y2 - y1) * (z3 - z2) - (z2 - z1) * (y3 - y2);
  f32 ny = (z2 - z1) * (x3 - x2) - (x2 - x1) * (z3 - z2);
  f32 nz = (x2 - x1) * (y3 - y2) - (y2 - y1) * (x3 - x2);
  f32 mag = sqrtf(nx*nx + ny*ny + nz*nz);

  s32 minY = y1;
  if (y2 < minY) minY = y2;
  if (y3 < minY) minY = y3;
  
  s32 maxY = y1;
  if (y2 > maxY) maxY = y2;
  if (y3 > maxY) maxY = y3;

  if (mag < 0.0001) return NULL;
  mag = (f32) (1.0 / mag);
  nx *= mag;
  ny *= mag;
  nz *= mag;

  Surface *tri = allocSurface();
  
  tri->vertex1.x = x1;
  tri->vertex1.y = y1;
  tri->vertex1.z = z1;

  tri->vertex2.x = x2;
  tri->vertex2.y = y2;
  tri->vertex2.z = z2;

  tri->vertex3.x = x3;
  tri->vertex3.y = y3;
  tri->vertex3.z = z3;
  
  tri->normal.x = nx;
  tri->normal.y = ny;
  tri->normal.z = nz;
  
  tri->originOffset = -(x1*nx + y1*ny + z1*nz);

  tri->lowerY = minY - 5;
  tri->upperY = maxY + 5;
  
  return tri;
}


static void buildObjectTransform(Object *o) {
  v3f translate = {
    o->pos.x,
    o->pos.y,
    o->pos.z,
  };

  v3h rotate = {
    (s16) o->displayAngle.pitch,
    (s16) o->displayAngle.yaw,
    (s16) o->displayAngle.roll,
  };

  matrixFromTransAndRot(&o->v21C[0], &translate, &rotate);
}


static void readObjectCollisionVertices(Object *o, s16 **data, s16 *vertexData) {
  buildObjectTransform(o);

  Mtxfp m = &o->v21C[0];
  
  s16 numVerts = *(*data)++;
  while (numVerts-- != 0) {
    s16 vx = *(*data)++;
    s16 vy = *(*data)++;
    s16 vz = *(*data)++;
    
    *vertexData++ = (s16) (m[0][0]*vx + m[1][0]*vy + m[2][0]*vz + m[3][0]);
    *vertexData++ = (s16) (m[0][1]*vx + m[1][1]*vy + m[2][1]*vz + m[3][1]);
    *vertexData++ = (s16) (m[0][2]*vx + m[1][2]*vy + m[2][2]*vz + m[3][2]);
  }
}


static void loadObjColModelFromVertexData(Object *o, s16 **data, s16 *vertexData) {
  s16 surfaceType = *(*data)++;
  s32 numTris = *(*data)++;
  
  for (s32 i = 0; i < numTris; i++) {
    Surface *tri = readSurfaceData(vertexData, data);

    if (tri != NULL) {
      tri->object = o;
      tri->type = surfaceType;
      addSurface(tri);
    }

    *data += 3;
  }
}


void loadObjectCollisionModel(Object *o) {
  s16 vertexData[600];

  s16 *data = o->surfaceModel;

  data++;
  readObjectCollisionVertices(o, &data, &vertexData[0]);

  while (*data != 0x41) {
    loadObjColModelFromVertexData(o, &data, &vertexData[0]);
  }
}
