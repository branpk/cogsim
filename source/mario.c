#include "mario.h"

#include "surface.h"
#include "util.h"

#include <stdlib.h>


static Surface *findTriFromListBelow(
  SurfaceNode *triangles,
  s32 x,
  s32 y,
  s32 z,
  f32 *pheight)
{
  while (triangles != NULL) {
    Surface *tri = triangles->head;
    triangles = triangles->tail;

    s32 x1 = tri->vertex1.x;
    s32 z1 = tri->vertex1.z;
    s32 x2 = tri->vertex2.x;
    s32 z2 = tri->vertex2.z;
    s32 x3 = tri->vertex3.x;
    s32 z3 = tri->vertex3.z;

    if ((z1 - z) * (x2 - x1) - (x1 - x) * (z2 - z1) < 0) continue;
    if ((z2 - z) * (x3 - x2) - (x2 - x) * (z3 - z2) < 0) continue;
    if ((z3 - z) * (x1 - x3) - (x3 - x) * (z1 - z3) < 0) continue;

    f32 nx = tri->normal.x;
    f32 ny = tri->normal.y;
    f32 nz = tri->normal.z;
    f32 oo = tri->originOffset;

    if (ny == 0.0f) continue;

    f32 height = -(x * nx + nz * z + oo) / ny;
    if (y - (height + -78.0f) < 0.0f) continue;

    *pheight = height;
    return tri;
  }

  return NULL;
}


f32 findFloor(v3f pos, Surface **pfloor) {
  s16 x = (s16) pos.x;
  s16 y = (s16) pos.y;
  s16 z = (s16) pos.z;
  
  *pfloor = NULL;

  if (x <= -0x2000 || x >= 0x2000) return -11000.0f;
  if (z <= -0x2000 || z >= 0x2000) return -11000.0f;

  f32 height = -11000.0f;
  *pfloor = findTriFromListBelow(allFloors.tail, x, y, z, &height);
  
  return height;
}
