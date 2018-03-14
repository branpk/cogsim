#include "cog.h"

#include "util.h"


s16 ttcSpeedSetting = 0;

static s16 ttcCogSpeeds[] = { 200, 400 };

s16 cogModel[] = {
  0x0040, 0x0006,
  0xff67, 0x0000, 0xfef7,
  0xfece, 0x0000, 0x0000,
  0xff67, 0x0000, 0x010a,
  0x009a, 0x0000, 0x010a,
  0x0133, 0x0000, 0x0000,
  0x009a, 0x0000, 0xfef7,
  0x0015, 0x0004,
  0x0000, 0x0001, 0x0002,
  0x0000, 0x0002, 0x0003,
  0x0000, 0x0003, 0x0004,
  0x0000, 0x0004, 0x0005,
  0x0041,
};


void updateTtcCog(Object *o) {
  switch (ttcSpeedSetting) {
  case 0:
  case 1:
    o->yawVel = ttcCogSpeeds[ttcSpeedSetting];
    break;
  
  case 2:
    if (incTowardSymFP(&o->yawVel, o->yawVelTarget, 50.0f))
      o->yawVelTarget = 200.0f * (randomU16() % 7) * randomUnit();
    break;

  case 3:
    break;
  }

  o->displayAngle.yaw += (s32) o->yawVel;
}
