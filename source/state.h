#ifndef STATE_H
#define STATE_H


#include "cog.h"
#include "mario.h"

#include <stdio.h>


extern Object cog;
extern MarioState mario;
extern int overrideRngLength;


typedef enum {
  fr_success,
  fr_landed_on_cog,
  fr_failed_to_land,
  fr_slowed_down,
  fr_not_under_ceil,
} FrameResult;


FrameResult frameAdvance(void);


#endif