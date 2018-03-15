#include "cog.h"
#include "mario.h"
#include "state.h"
#include "surface.h"
#include "util.h"

#include <GLFW/glfw3.h>

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


bool handleFrameResult(FrameResult result);


static bool unitSquareMode = false;
static int zoomAmount = 0;


static void drawWalls(void) {
  glColor3f(0.4f, 0.4f, 0.4f);

  glBegin(GL_LINES);
  glVertex2f(2081, -861);
  glVertex2f(862, -2080);

  glVertex2f(2081, 862);
  glVertex2f(2081, -861);
  glEnd();
}


static void drawUnitSquares(s16 x0, s16 z0, s16 x1, s16 z1) {
  for (s16 x = x0; x < x1; x++) {
    for (s16 z = z0; z < z1; z++) {
      v3f pos = { x, cog.pos.y, z };

      f32 dx = pos.x - cog.pos.x;
      f32 dz = pos.z - cog.pos.z;
      f32 dist = sqrtf(dx*dx + dz*dz);
      if (dist > 350) continue;

      Surface *floor = NULL;
      if (!(dist < 200))
        findFloor(pos, &floor);

      if (dist < 200 || floor != NULL) {
        glBegin(GL_TRIANGLE_STRIP);
        glVertex2f(pos.x, pos.z);
        glVertex2f(pos.x + 1, pos.z);
        glVertex2f(pos.x, pos.z + 1);
        glVertex2f(pos.x + 1, pos.z + 1);
        glEnd();
      }
    }
  }
}


static void drawSurface(Surface *tri) {
  glBegin(GL_LINE_LOOP);
  glVertex2f(tri->vertex1.x, tri->vertex1.z);
  glVertex2f(tri->vertex2.x, tri->vertex2.z);
  glVertex2f(tri->vertex3.x, tri->vertex3.z);
  glEnd();
}


static void drawSurfaces(v3f center, f32 span) {
  if (unitSquareMode) {
    glColor3f(0.7f, 0.7f, 0.7f);
    span = 1.5 * span;
    s16 x0 = (s16) (center.x - span/2) - 1;
    s16 z0 = (s16) (center.z - span/2) - 1;
    s16 x1 = (s16) (center.x + span/2) + 1;
    s16 z1 = (s16) (center.z + span/2) + 1;
    drawUnitSquares(x0, z0, x1, z1);
  }
  else {
    glColor3f(0.8f, 0.8f, 0.8f);
    for (SurfaceNode *n = allFloors.tail; n != NULL; n = n->tail)
      drawSurface(n->head);
  }
}


static void drawMario(MarioState *m) {
  v3f face = { sins(m->facingYaw), 0, coss(m->facingYaw) };
  v3f side = { coss(m->facingYaw), 0, -sins(m->facingYaw) };

  f32 width = 30;
  f32 length = 40;

  glColor3f(0.8f, 0, 0);

  glBegin(GL_TRIANGLES);
  glVertex2f(
    m->pos.x - face.x * length - side.x * width/2,
    m->pos.z - face.z * length - side.z * width/2);
  glVertex2f(
    m->pos.x - face.x * length + side.x * width/2,
    m->pos.z - face.z * length + side.z * width/2);
  glVertex2f(m->pos.x, m->pos.z);
  glEnd();

  v3f qstep = {
    m->pos.x + m->vel.x / 4.0f,
    m->pos.y + m->vel.y / 4.0f,
    m->pos.z + m->vel.z / 4.0f,
  };

  glBegin(GL_LINES);
  glVertex2f(m->pos.x, m->pos.z);
  glVertex2f(qstep.x, qstep.z);
  glEnd();
}


static void drawCircle(f32 x, f32 z, f32 r) {
  glBegin(GL_LINE_LOOP);
  for (f32 a = 0; a < 2*3.141592653f; a += 0.01f) {
    glVertex2f(x + r * cosf(a), z + r * sinf(a));
  }
  glEnd();
}


static void drawCogCircles(void) {
  glColor3f(0.3f, 0.3f, 0.3f);
  drawCircle(cog.pos.x, cog.pos.z, 308);
  drawCircle(cog.pos.x, cog.pos.z, 264);
  drawCircle(1215, -1215, 308);
  drawCircle(1215, -1215, 264);
}


static void kbCallback(
  GLFWwindow *window, int key, int scancode, int action, int mods)
{
  (void) window;
  (void) scancode;
  (void) mods;

  if (key == GLFW_KEY_U && action == GLFW_PRESS)
    unitSquareMode = !unitSquareMode;

  if (key == GLFW_KEY_UP && action != GLFW_RELEASE)
    zoomAmount += 1;
  if (key == GLFW_KEY_DOWN && action != GLFW_RELEASE)
    zoomAmount -= 1;
  if (zoomAmount < -1) zoomAmount = -1;
  if (zoomAmount > 10) zoomAmount = 10;
}


static f32 computeZoomSpan(int zoom) {
  if (zoom > 0) return 0.5f * computeZoomSpan(zoom - 1);
  if (zoom < 0) return 1.5f * computeZoomSpan(zoom + 1);
  return 1200.0f;
}


void runVisualizer(void) {
  glfwInit();

  GLFWwindow *window = glfwCreateWindow(
    480, 480, "TTC Cog Simulator", NULL, NULL);
  glfwSetKeyCallback(window, kbCallback);
  glfwMakeContextCurrent(window);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  double accumTime = 0;
  double lastTime = glfwGetTime();
  int framesPerSec = 30;

  bool contUpdating = true;

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    f32 span = computeZoomSpan(zoomAmount);

    glLoadIdentity();
    glRotatef(45, 0, 0, 1);
    float scale = 2/span;
    glScalef(-scale, scale, 1);
    glTranslatef(-mario.pos.x, -mario.pos.z, 0);

    double currentTime = glfwGetTime();
    accumTime += currentTime - lastTime;
    lastTime = currentTime;
    while (accumTime >= 1.0/framesPerSec) {
      frameAdvance();
      // contUpdating = contUpdating && handleFrameResult(frameAdvance());
      accumTime -= 1.0/framesPerSec;
    }

    drawWalls();
    drawSurfaces(mario.pos, span);
    drawMario(&mario);
    drawCogCircles();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}

