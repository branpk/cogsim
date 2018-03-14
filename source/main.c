#include "cog.h"
#include "mario.h"
#include "surface.h"
#include "util.h"

#include <GLFW/glfw3.h>

#include <math.h>
#include <stdio.h>


Object cog;
MarioState mario;


void drawWalls(void) {
  glColor3f(0.4f, 0.4f, 0.4f);

  glBegin(GL_LINES);
  glVertex2f(2081, -861);
  glVertex2f(862, -2080);

  glVertex2f(2081, 862);
  glVertex2f(2081, -861);
  glEnd();
}


void drawLegend(f32 x, f32 z, f32 size) {
  glColor3f(1, 0, 0);
  glBegin(GL_LINES);
  glVertex2f(x, z);
  glVertex2f(x + size, z);
  glEnd();

  glColor3f(0, 0, 1);
  glBegin(GL_LINES);
  glVertex2f(x, z);
  glVertex2f(x, z + size);
  glEnd();
}


void drawSurface(Surface *tri) {
  glBegin(GL_LINE_LOOP);
  glVertex2f(tri->vertex1.x, tri->vertex1.z);
  glVertex2f(tri->vertex2.x, tri->vertex2.z);
  glVertex2f(tri->vertex3.x, tri->vertex3.z);
  glEnd();
}


void drawSurfaces(void) {
  glColor3f(0.8f, 0.8f, 0.8f);
  for (SurfaceNode *n = allFloors.tail; n != NULL; n = n->tail)
    drawSurface(n->head);
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


int main(int argc, char **argv) {
  (void) argc;
  (void) argv;

  ttcSpeedSetting = 3;

  cog.pos = (v3f) { 1490, -2088, -873 };
  cog.surfaceModel = &cogModel[0];

  mario.intendedMag = 32.0f;
  mario.intendedYaw = -0x122E;
  mario.facingYaw = -0x40C;
  mario.pos = (v3f) { 1420, -2088, -1139.4f };
  mario.hSpeed = 33;

  glfwInit();

  GLFWwindow *window = glfwCreateWindow(
    480, 480, "TTC Cog Simulator", NULL, NULL);
  glfwMakeContextCurrent(window);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    glRotatef(45, 0, 0, 1);
    float scale = 1/600.0f;
    glScalef(-scale, scale, 1);
    glTranslatef(-cog.pos.x, -cog.pos.z, 0);

    clearSurfaces();
    updateTtcCog(&cog);
    loadObjectCollisionModel(&cog);

    if (onFloor(&mario))
      printf("Landed at hspeed = %f\n", mario.hSpeed);
    updateAirWithoutTurn(&mario);
    if (!quarterStepLands(&mario))
      printf("Failed to land at hspeed = %f\n", mario.hSpeed);

    drawWalls();
    drawLegend(cog.pos.x, cog.pos.z + 600, 100);
    drawSurfaces();
    drawMario(&mario);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
