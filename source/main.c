#include "cog.h"
#include "mario.h"
#include "surface.h"
#include "util.h"

#include <GLFW/glfw3.h>

#include <math.h>
#include <stdio.h>


void frameAdvance(void) {
  clearSurfaces();
  updateTtcCog();
  loadObjectCollisionModel(&cog);
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


int main(int argc, char **argv) {
  (void) argc;
  (void) argv;

  ttcSpeedSetting = 2;


  glfwInit();

  GLFWwindow *window = glfwCreateWindow(
    480, 480, "TTC Cog Simulator", NULL, NULL);
  glfwMakeContextCurrent(window);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    glRotatef(90, 0, 0, 1);
    float scale = 1/600.0f;
    glScalef(-scale, scale, 1);
    glTranslatef(-cog.pos.x, -cog.pos.z, 0);

    frameAdvance();

    drawLegend(cog.pos.x + 400, cog.pos.z + 400, 100);

    glColor3f(0.8f, 0.8f, 0.8f);
    for (SurfaceNode *n = allFloors.tail; n != NULL; n = n->tail)
      drawSurface(n->head);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
