#pragma once

// Constants
extern int Width;
extern int Height;

// Structs
struct Color {
  float r, g, b, a;
};

// Func decls
void PutPixel(int x, int y, Color color);
