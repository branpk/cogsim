#ifndef STATE_H
#define STATE_H


#include "cog.h"
#include "mario.h"

#include <stdio.h>


extern Object cog;
extern MarioState mario;
extern int overrideRngLength;


bool frameAdvance(void);


#endif