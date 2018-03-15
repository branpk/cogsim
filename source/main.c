#include "cog.h"
#include "mario.h"
#include "ol.h"
#include "state.h"
#include "surface.h"
#include "util.h"

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int runVisualizer(void);


static void error(char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  fprintf(stderr, "\x1b[91mError:\x1b[0m ");
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  
  va_end(args);
  exit(1);
}


static void loadMario(OlBlock *b) {
  mario.pos.x = ol_checkFieldFloat(b, "x");
  mario.pos.y = cog.pos.y;
  mario.pos.z = ol_checkFieldFloat(b, "z");
  mario.facingYaw = (s16) ol_checkFieldInt(b, "yaw");
  mario.hSpeed = ol_checkFieldFloat(b, "hspeed");
}


static void loadCog(OlBlock *b) {
  cog.displayAngle.yaw = (s32) ol_checkFieldInt(b, "yaw");
  cog.yawVel = ol_checkFieldFloat(b, "speed");
  cog.yawVelTarget = ol_checkFieldFloat(b, "speedtarget");
}


static void loadRng(OlBlock *b) {
  overrideRngLength = 0;
  for (OlField *f = b->head; f != NULL; f = f->next)
    overrideRngLength += 1;
  
  cogRngOverride = (s8 *) malloc(overrideRngLength);

  size_t i = 0;
  for (OlField *f = b->head; f != NULL; f = f->next) {
    int value = (int) f->value->dec;
    if (value < -6 || value > 6)
      error("Invalid RNG result value: %d", value);

    cogRngOverride[i++] = (s8) value;
  }

  cogRngOverride[i] = 127;
}


static void loadState(char *filename) {
  OlBlock *b = ol_parseFile(filename);

  ttcSpeedSetting = ol_checkFieldInt(b, "setting");
  if (ttcSpeedSetting < 0 || ttcSpeedSetting > 3)
    error("Invalid TTC speed setting: %d", ttcSpeedSetting);

  loadMario(ol_checkField(b, "mario", ol_block)->block);
  loadCog(ol_checkField(b, "cog", ol_block)->block);
  loadRng(ol_checkFieldArray(b, "rng", ol_dec));

  ol_free(b);
}


static char *inputFilename = NULL;
static char *outputFilename = NULL;
static bool visual = false;


int main(int argc, char **argv) {
  int i = 1;
  while (i < argc) {
    char *arg = argv[i++];

    if (strcmp(arg, "-o") == 0) {
      if (i >= argc)
        error("Expected output filename after -o flag");
      outputFilename = argv[i++];
    }
    else if (strcmp(arg, "-v") == 0) {
      visual = true;
    }
    else {
      inputFilename = arg;
    }
  }

  if (inputFilename == NULL)
    error("Expected input filename");
  printf("Input file: \x1b[1m%s\x1b[0m\n", inputFilename);
  if (outputFilename)
    printf("Output file: \x1b[1m%s\x1b[0m\n", outputFilename);
  if (visual)
    printf("Running in visual mode\n");

  ttcSpeedSetting = 0;

  cog.pos = (v3f) { 1490, -2088, -873 };
  cog.surfaceModel = &cogModel[0];

  loadState(inputFilename);

  if (visual) {
    runVisualizer();
  }
  else {
    while (frameAdvance()) {}
  }

  return 0;
}
