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


#if defined(WIN32)

#include <windows.h>

void win_enable_ansi(void) {
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD dwMode = 0;
  GetConsoleMode(hOut, &dwMode);
  dwMode |= 4;
  SetConsoleMode(hOut, dwMode);
}

#endif


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

static FILE *outputFile = NULL;


static void recordInitState(void) {
  if (outputFile == NULL) return;

  fprintf(outputFile, "frame,rng result,mag intended,yaw intended,");
  fprintf(outputFile, "m x,m z,m yaw,m hspeed,");
  fprintf(outputFile, "c yaw,c speed,c speed target,c rng calls\n");

  fprintf(outputFile, "0,,,,");

  fprintf(outputFile, "%f,%f,", mario.pos.x, mario.pos.z);
  fprintf(outputFile, "%d,", mario.facingYaw);
  fprintf(outputFile, "%f,", mario.hSpeed);

  fprintf(outputFile, "%d,", (s16) cog.displayAngle.yaw);
  fprintf(outputFile, "%f,%f,", cog.yawVel, cog.yawVelTarget);

  fprintf(outputFile, "0");

  fprintf(outputFile, "\n");
}


void recordState(void) {
  static int frames = 0;
  frames += 1;
  if (outputFile == NULL) return;

  fprintf(outputFile, "%d,", frames);
  if (cogRngCall != 127)
    fprintf(outputFile, "%d", cogRngCall);
  fprintf(outputFile, ",");

  fprintf(outputFile, "%f,", mario.intendedMag);
  fprintf(outputFile, "%d,", mario.intendedYaw);
  fprintf(outputFile, ",,,");
  fprintf(outputFile, "%f,", mario.hSpeed);

  fprintf(outputFile, "%d,", (s16) cog.displayAngle.yaw);
  fprintf(outputFile, "%f,%f,", cog.yawVel, cog.yawVelTarget);

  fprintf(outputFile, "%d", 2 * numCogRngCalls);

  fprintf(outputFile, "\n");
}


int main(int argc, char **argv) {
#if defined(WIN32)
  win_enable_ansi();
#endif

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
  if (outputFilename != NULL)
    printf("Output file: \x1b[1m%s\x1b[0m\n", outputFilename);
  if (visual)
    printf("Running in visual mode\n");

  if (outputFilename != NULL) {
    outputFile = fopen(outputFilename, "wb");
    if (outputFile == NULL)
      error("Failed to open '%s' for writing", outputFilename);
  }

  cog.pos = (v3f) { 1490, -2088, -873 };
  cog.surfaceModel = &cogModel[0];

  loadState(inputFilename);

  recordInitState();

  if (visual) {
    runVisualizer();
  }
  else {
    while (frameAdvance()) {}
  }

  if (outputFile != NULL)
    fclose(outputFile);
  return 0;
}
