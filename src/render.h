#ifndef RENDER_H
#define RENDER_H

/*
 * render.h - [Persona 1: Despliegue]
 * Framebuffer, plot, Bresenham, bordes, relleno y textura.
 */

#include "main.h"
#include "data.h"

/* Estado global del despliegue (definido en render.c) */
extern COLOR **buffer;
extern COLOR current_color;
extern DISPLAY_MODES current_mode;

int render_init(void);   /* reserva y limpia el framebuffer */
void render_destroy(void);
void render_clear(void);

void draw_scene(void); /* callback de glutDisplayFunc */

void plot(int x, int y); /* escribe current_color en buffer[x][y] */
void color_province(COLOR *color, PROVINCES province);

/* Implementada en src/Bresenham.s (NASM, ELF64). Llama a plot(x, y). */
extern void bresenham(int x0, int y0, int x1, int y1);

/* Vertices del poligono ya transformados a pixeles. El que llama hace free(). */
POINT *polygon_to_screen(const POLYGON *p);

void DrawPolygon(const POLYGON *p);    /* bordes con bresenham */
void PaintPolygon(const POLYGON *p);   /* relleno de color solido */
void TexturePolygon(const POLYGON *p); /* relleno con textura */

#endif /* RENDER_H */
