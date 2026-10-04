#include "render.h"
#include "geometry.h"

COLOR **buffer = NULL;
COLOR current_color = {1.0, 1.0, 1.0};
DISPLAY_MODES current_mode = MODE_SIMPLE;

static GLfloat *pixels = NULL; /* copia lineal del buffer para glDrawPixels */

/* ---------- Ciclo de vida ---------- */

int render_init(void)
{
  int i;

  buffer = (COLOR **)malloc(HRES * sizeof(COLOR *));
  if (buffer == NULL)
    return -1;

  for (i = 0; i < HRES; i++)
  {
    buffer[i] = (COLOR *)malloc(VRES * sizeof(COLOR));
    if (buffer[i] == NULL)
    {
      while (--i >= 0)
        free(buffer[i]);
      free(buffer);
      buffer = NULL;
      return -1;
    }
  }

  render_clear();
  return 0;
}

void render_destroy(void)
{
  int i;
  if (buffer != NULL)
  {
    for (i = 0; i < HRES; i++)
      free(buffer[i]);
    free(buffer);
    buffer = NULL;
  }
  free(pixels);
  pixels = NULL;
}

void render_clear(void)
{
  int i, j;
  for (i = 0; i < HRES; i++)
    for (j = 0; j < VRES; j++)
    {
      buffer[i][j].r = 0.0;
      buffer[i][j].g = 0.0;
      buffer[i][j].b = 0.0;
    }
}

/* ---------- Pantalla ---------- */

/* Vuelca el framebuffer a la ventana con una sola llamada (mucho mas rapido que GL_POINTS). */
static void present_buffer(void)
{
  int x, y, idx;

  if (pixels == NULL)
  {
    pixels = (GLfloat *)malloc(sizeof(GLfloat) * 3 * HRES * VRES);
    if (pixels == NULL)
      return;
  }

  for (y = 0; y < VRES; y++)
    for (x = 0; x < HRES; x++)
    {
      idx = 3 * (y * HRES + x);
      pixels[idx] = (GLfloat)buffer[x][y].r;
      pixels[idx + 1] = (GLfloat)buffer[x][y].g;
      pixels[idx + 2] = (GLfloat)buffer[x][y].b;
    }

  glRasterPos2i(0, 0);
  glDrawPixels(HRES, VRES, GL_RGB, GL_FLOAT, pixels);
}

static void draw_map(void)
{
  int i;

  for (i = 0; i < map.count; i++)
  {
    const POLYGON *p = &map.polygons[i];

    switch (current_mode)
    {
    case MODE_FILL:
      PaintPolygon(p);
      break;
    case MODE_TEXTURE:
      TexturePolygon(p);
      break;
    default:
      break;
    }
    DrawPolygon(p); /* los bordes siempre van encima */
  }
}

/* Callback de GLUT: redibuja TODO el cuadro. */
void draw_scene(void)
{
  render_clear();

  if (map.count == 0)
  {
    /* Mapa aun no cargado: linea de prueba para verificar Bresenham.s */
    color_province(&current_color, CARTAGO);
    bresenham(HRES / 4, VRES / 4, 3 * HRES / 4, 3 * VRES / 4);
  }
  else
  {
    draw_map();
  }

  present_buffer();
  glFlush();
}

/* ---------- Primitivas ---------- */

/*
 * plot(x, y): pinta un pixel en el framebuffer con current_color.
 * Solo escribe en buffer; draw_scene lo muestra al final.
 */
void plot(int x, int y)
{
  if (x < 0 || x >= HRES || y < 0 || y >= VRES)
    return;

  buffer[x][y] = current_color;
}

/* Color de cada provincia (usar al inicio de un algoritmo de trazado). */
void color_province(COLOR *c, PROVINCES province)
{
  switch (province)
  {
  case SANJOSE: /* Morado */
    c->r = 0.5; c->g = 0.0; c->b = 0.5;
    break;
  case ALAJUELA: /* Rojo */
    c->r = 1.0; c->g = 0.0; c->b = 0.0;
    break;
  case CARTAGO: /* Azul */
    c->r = 0.0; c->g = 0.0; c->b = 1.0;
    break;
  case HEREDIA: /* Amarillo */
    c->r = 1.0; c->g = 1.0; c->b = 0.0;
    break;
  case GUANACASTE: /* Rosado */
    c->r = 1.0; c->g = 0.0; c->b = 0.5;
    break;
  case PUNTARENAS: /* Naranja */
    c->r = 1.0; c->g = 0.5; c->b = 0.0;
    break;
  case LIMON: /* Verde */
    c->r = 0.0; c->g = 1.0; c->b = 0.0;
    break;
  default:
    break;
  }
}

/* ---------- Poligonos ---------- */

/* Aplica la camara (geometry) a los vertices y los pasa a pixeles enteros. */
POINT *polygon_to_screen(const POLYGON *p)
{
  MAT3 m = geometry_matrix();
  POINT *pts;
  VEC2 v;
  int i;

  pts = (POINT *)malloc(p->count * sizeof(POINT));
  if (pts == NULL)
    return NULL;

  for (i = 0; i < p->count; i++)
  {
    v = geometry_apply(m, p->vertices[i]);
    pts[i].x = (int)lround(v.x);
    pts[i].y = (int)lround(v.y);
  }
  return pts;
}

/* Bordes: n llamadas a bresenham, cerrando el poligono (ultimo -> primero). */
void DrawPolygon(const POLYGON *p)
{
  POINT *pts;
  POINT a, b;
  int i, next;

  if (p->count < 2)
    return;

  pts = polygon_to_screen(p);
  if (pts == NULL)
    return;

  color_province(&current_color, p->province);

  for (i = 0; i < p->count; i++)
  {
    next = (i + 1) % p->count;
    a = pts[i];
    b = pts[next];
    if (geometry_clip_line(&a, &b))
      bresenham(a.x, a.y, b.x, b.y);
  }

  free(pts);
}

/* TODO (Persona 1): relleno de color solido (scanline / algoritmo visto en clase). */
void PaintPolygon(const POLYGON *p)
{
  (void)p;
}

/* TODO (Persona 1): relleno usando la textura de data.c (texture[][]). */
void TexturePolygon(const POLYGON *p)
{
  (void)p;
}
