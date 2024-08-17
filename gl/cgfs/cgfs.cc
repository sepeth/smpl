#include "cgfs.hh"
#include <glad/glad.h>

void PutPixel(int x, int y, Color color) {
  glEnable(GL_SCISSOR_TEST);
  glScissor(Width / 2 + x, Height / 2 - y, 10, 10);
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_SCISSOR_TEST);
}
