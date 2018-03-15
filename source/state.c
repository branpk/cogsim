#include "state.h"

#include "cog.h"
#include "mario.h"
#include "surface.h"
#include "util.h"

#include <math.h>
#include <stdio.h>


Object cog;
MarioState mario;
int overrideRngLength;


static bool checkInput(MarioState *m, f32 mag, f32 yaw) {
  f32 startHSpeed = m->hSpeed;

  m->intendedMag = mag;
  m->intendedYaw = yaw;
  updateAirWithoutTurn(m);

  bool works = quarterStepLands(m);

  m->hSpeed = startHSpeed;
  return works;
}


static bool computeOptimalInput(MarioState *m) {
  for (u16 dyaw = 0; dyaw <= 0x8000; dyaw += 0x10) {
    if (checkInput(m, 32.0f, m->facingYaw + dyaw)) return true;
    if (checkInput(m, 32.0f, m->facingYaw - dyaw)) return true;
  }
  return false;
}


FrameResult frameAdvance(void) {
  clearSurfaces();
  updateTtcCog(&cog);
  loadObjectCollisionModel(&cog);

  if (onFloor(&mario))
    return fr_landed_on_cog;

  f32 startHSpeed = mario.hSpeed;

  if (!computeOptimalInput(&mario))
    return fr_failed_to_land;

  updateAirWithoutTurn(&mario);
  
  if (!quarterStepLands(&mario)) {
    printf("Internal error: quarter step inconsistency\n");
    return fr_failed_to_land;
  }

  v3f qstep = {
    mario.pos.x + mario.vel.x / 4.0f,
    mario.pos.y + mario.vel.y / 4.0f,
    mario.pos.z + mario.vel.z / 4.0f,
  };
  f32 dx = qstep.x - 1215;
  f32 dz = qstep.z - -1215;
  if (sqrtf(dx*dx + dz*dz) > 264)
    return fr_not_under_ceil;

  if (mario.hSpeed <= startHSpeed)
    return fr_slowed_down;

  return fr_success;
}
