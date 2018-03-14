#ifndef SURFACE_H
#define SURFACE_H


#include "util.h"


typedef struct Surface Surface;
typedef struct SurfaceNode SurfaceNode;


struct Surface {
  s16 type;

  s16 v02;
  s8 v04;
  u8 v05;

  s16 lowerY;
  s16 upperY;

  v3h vertex1;
  v3h vertex2;
  v3h vertex3;

  v3f normal;
  f32 originOffset;
  Object *object;
};


struct SurfaceNode {
  SurfaceNode *tail;
  Surface *head;
};


extern SurfaceNode allFloors;


void clearSurfaces(void);
void loadObjectCollisionModel(Object *o);


#endif