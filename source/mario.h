#ifndef MARIO_H
#define MARIO_H


#include "surface.h"
#include "util.h"


typedef struct MarioState MarioState;


struct MarioState {
  f32 intendedMag;
  s16 intendedYaw;
  s16 facingYaw;
  v3f pos;
  v3f vel;
  f32 hSpeed;
};


f32 findFloor(v3f pos, Surface **pfloor);
void updateAirWithoutTurn(MarioState *m);
bool onFloor(MarioState *m);
bool quarterStepLands(MarioState *m);


#endif