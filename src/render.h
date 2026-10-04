#ifndef RENDER_H
#define RENDER_H

#include "main.h"

int render_init(void);
void render_destroy(void);

void draw_scene(void);
void plot(int x, int y);
void color_province(COLOR *color, PROVINCES province);

void bresenham(int x0, int y0, int x1, int y1);

#endif