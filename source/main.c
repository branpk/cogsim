#include "cog.h"
#include "mario.h"
#include "surface.h"
#include "util.h"

#include <GLFW/glfw3.h>

#include <math.h>
#include <stdio.h>


bool checkInput(MarioState *m, f32 mag, f32 yaw) {
  f32 startHSpeed = m->hSpeed;

  m->intendedMag = mag;
  m->intendedYaw = yaw;
  updateAirWithoutTurn(m);

  bool works = quarterStepLands(m);

  m->hSpeed = startHSpeed;
  return works;
}


bool computeOptimalInput(MarioState *m) {
  for (u16 dyaw = 0; dyaw <= 0x8000; dyaw += 0x10) {
    if (checkInput(m, 32.0f, m->facingYaw + dyaw)) return true;
    if (checkInput(m, 32.0f, m->facingYaw - dyaw)) return true;
  }
  return false;
}


Object cog;
MarioState mario;


typedef enum {
  fr_success,
  fr_landed_on_cog,
  fr_failed_to_land,
  fr_slowed_down,
} FrameResult;


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

  if (mario.hSpeed <= startHSpeed)
    return fr_slowed_down;

  return fr_success;
}


bool unitSquareMode = false;
int zoomAmount = 0;


void drawWalls(void) {
  glColor3f(0.4f, 0.4f, 0.4f);

  glBegin(GL_LINES);
  glVertex2f(2081, -861);
  glVertex2f(862, -2080);

  glVertex2f(2081, 862);
  glVertex2f(2081, -861);
  glEnd();
}


void drawUnitSquares(s16 x0, s16 z0, s16 x1, s16 z1) {
  for (s16 x = x0; x < x1; x++) {
    for (s16 z = z0; z < z1; z++) {
      v3f pos = { x, cog.pos.y, z };

      Surface *floor;
      findFloor(pos, &floor);

      if (floor != NULL) {
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


void drawSurface(Surface *tri) {
  glBegin(GL_LINE_LOOP);
  glVertex2f(tri->vertex1.x, tri->vertex1.z);
  glVertex2f(tri->vertex2.x, tri->vertex2.z);
  glVertex2f(tri->vertex3.x, tri->vertex3.z);
  glEnd();
}


void drawSurfaces(v3f center, f32 span) {
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


void drawMario(MarioState *m) {
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


void kbCallback(
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


f32 computeZoomSpan(int zoom) {
  if (zoom > 0) return 0.5f * computeZoomSpan(zoom - 1);
  if (zoom < 0) return 1.5f * computeZoomSpan(zoom + 1);
  return 1200.0f;
}


int main(int argc, char **argv) {
  (void) argc;
  (void) argv;

  ttcSpeedSetting = 3;

  cog.pos = (v3f) { 1490, -2088, -873 };
  cog.surfaceModel = &cogModel[0];

  mario.facingYaw = -0x40C;
  mario.pos = (v3f) { 1420, -2088, -1139.4f };
  mario.hSpeed = 33;

  glfwInit();

  GLFWwindow *window = glfwCreateWindow(
    480, 480, "TTC Cog Simulator", NULL, NULL);
  glfwSetKeyCallback(window, kbCallback);
  glfwMakeContextCurrent(window);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    f32 span = computeZoomSpan(zoomAmount);

    glLoadIdentity();
    glRotatef(45, 0, 0, 1);
    float scale = 2/span;
    glScalef(-scale, scale, 1);
    glTranslatef(-mario.pos.x, -mario.pos.z, 0);

    FrameResult result = frameAdvance();
    bool failure = false;
    switch (result) {
    case fr_success:
      break;
    case fr_landed_on_cog:
      printf("Cog slid under Mario\n");
      failure = true;
      break;
    case fr_failed_to_land:
      printf("No input causes next quarter step to land\n");
      failure = true;
      break;
    case fr_slowed_down:
      printf("Impossible to land without losing speed\n");
      failure = true;
      break;
    }
    if (failure) break;

    drawWalls();
    drawSurfaces(mario.pos, span);
    drawMario(&mario);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
