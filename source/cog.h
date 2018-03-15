#ifndef COG_H
#define COG_H


#include "util.h"


extern s8 *cogRngOverride;
extern s32 numCogRngCalls;
extern s8 cogRngCall;

extern s16 ttcSpeedSetting;
extern s16 cogModel[];


void updateTtcCog(Object *o);


#endif