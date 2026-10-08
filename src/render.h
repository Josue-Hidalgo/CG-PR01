#ifndef RENDER_H
#define RENDER_H
#define CENTER_RADIUS 3


#include "main.h"
#include "data.h"

extern COLOR **buffer;
extern COLOR current_color;
extern DISPLAY_MODES current_mode;
extern int ref_point;

typedef struct
{
  int ylow, yhigh;
  double xhigh;
  double dxdy;
  double x;
  int active;
} EDGE;

int render_init(void);
void render_destroy(void);
void render_clear(void);

void draw_scene(void);

void plot(int x, int y);
void color_province(COLOR *color, PROVINCES province);

extern void bresenham(int x0, int y0, int x1, int y1);

POINT *polygon_to_screen(const POLYGON *p);

void DrawPolygon(const POLYGON *p);
void PaintPolygon(const POLYGON *p);
void TexturePolygon(const POLYGON *p);
void scanline_fill_color(const POLYGON *p);
void scanline_fill_texture(const POLYGON *p);
#endif /* RENDER_H */
