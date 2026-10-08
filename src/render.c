#include "render.h"
#include "geometry.h"
#include "GUI.h"

COLOR **buffer = NULL;
COLOR current_color = {1.0, 1.0, 1.0};
DISPLAY_MODES current_mode = MODE_SIMPLE;
static const COLOR border_color = {0.0, 0.0, 0.0};

static GLfloat *pixels = NULL;
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

  if (current_mode != MODE_SIMPLE)
    for (i = 0; i < map.count; i++)
    {
      if (current_mode == MODE_FILL)
        PaintPolygon(&map.polygons[i]);
      else
        TexturePolygon(&map.polygons[i]);
    }

  for (i = 0; i < map.count; i++)
    DrawPolygon(&map.polygons[i]);
}

#define CENTER_RADIUS 3

// Punto fijo en el centro del area de visualizacion (coordenadas de pantalla).
static void draw_center_point(void)
{
  const COLOR fill = {1.0, 1.0, 1.0};
  int cx = HRES / 2;
  int cy = VRES / 2;
  int dx, dy, d2;

  current_color = fill;
  for (dy = -CENTER_RADIUS; dy <= CENTER_RADIUS; dy++)
    for (dx = -CENTER_RADIUS; dx <= CENTER_RADIUS; dx++)
    {
      d2 = dx * dx + dy * dy;
      if (d2 <= CENTER_RADIUS * CENTER_RADIUS)
        plot(cx + dx, cy + dy);
    }
}

void draw_scene(void)
{
  render_clear();

  if (map.count == 0)
  {
    color_province(&current_color, CARTAGO);
    bresenham(HRES / 4, VRES / 4, 3 * HRES / 4, 3 * VRES / 4);
  }
  else
  {
    draw_map();
  }
  draw_center_point();   
  present_buffer();
  gui_draw_panel();
  glFlush();
}

void plot(int x, int y)
{
  if (x < 0 || x >= HRES || y < 0 || y >= VRES)
    return;

  buffer[x][y] = current_color;
}

void color_province(COLOR *c, PROVINCES province)
{
  switch (province)
  {
  case SANJOSE:
    c->r = 0.5;
    c->g = 0.0;
    c->b = 0.5;
    break;
  case ALAJUELA:
    c->r = 1.0;
    c->g = 0.0;
    c->b = 0.0;
    break;
  case CARTAGO:
    c->r = 0.0;
    c->g = 0.0;
    c->b = 1.0;
    break;
  case HEREDIA:
    c->r = 1.0;
    c->g = 1.0;
    c->b = 0.0;
    break;
  case GUANACASTE:
    c->r = 1.0;
    c->g = 0.0;
    c->b = 0.5;
    break;
  case PUNTARENAS:
    c->r = 1.0;
    c->g = 0.5;
    c->b = 0.0;
    break;
  case LIMON:
    c->r = 0.0;
    c->g = 1.0;
    c->b = 0.0;
    break;
  default:
    break;
  }
}

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
static void draw_segment(POINT a, POINT b)
{
  POINT t;

  if (a.x > b.x || (a.x == b.x && a.y > b.y))
  {
    t = a;
    a = b;
    b = t;
  }

  if (geometry_clip_line(&a, &b))
    bresenham(a.x, a.y, b.x, b.y);
}

void DrawPolygon(const POLYGON *p)
{
  POINT *pts;
  int i;

  if (p->count < 2)
    return;

  pts = polygon_to_screen(p);
  if (pts == NULL)
    return;

  if (current_mode == MODE_SIMPLE)
    color_province(&current_color, p->province);
  else
    current_color = border_color;

  for (i = 0; i < p->count; i++)
    draw_segment(pts[i], pts[(i + 1) % p->count]);

  free(pts);
}

/* ---------- Poligonos: relleno ---------- */

static int cmp_double(const void *a, const void *b)
{
  double x = *(const double *)a, y = *(const double *)b;
  return (x > y) - (x < y);
}

// camara + recorte Sutherland-Hodgman contra la ventana + redondeo a pixeles
static POINT *polygon_to_clipped_screen(const POLYGON *p, int *count)
{
  MAT3 m = geometry_matrix();
  VEC2 *v, *clipped;
  POINT *pts;
  int i, n;

  *count = 0;

  v = (VEC2 *)malloc(p->count * sizeof(VEC2));
  if (v == NULL)
    return NULL;
  for (i = 0; i < p->count; i++)
    v[i] = geometry_apply(m, p->vertices[i]);

  n = geometry_clip_polygon(v, p->count, &clipped);
  free(v);
  if (n < 3)
  {
    free(clipped);
    return NULL;
  }

  pts = (POINT *)malloc(n * sizeof(POINT));
  if (pts != NULL)
  {
    for (i = 0; i < n; i++)
    {
      pts[i].x = (int)lround(clipped[i].x);
      pts[i].y = (int)lround(clipped[i].y);
    }
    *count = n;
  }
  free(clipped);
  return pts;
}

static int build_edges(const POINT *pts, int count, EDGE **edges, int *ymin, int *ymax)
{
  EDGE *e;
  POINT a, b, t;
  int i, n = 0;

  e = (EDGE *)malloc(count * sizeof(EDGE));
  if (e == NULL)
    return -1;

  *ymin = *ymax = pts[0].y;
  for (i = 0; i < count; i++)
  {
    if (pts[i].y < *ymin)
      *ymin = pts[i].y;
    if (pts[i].y > *ymax)
      *ymax = pts[i].y;

    a = pts[i];
    b = pts[(i + 1) % count];
    if (a.y == b.y)
      continue; // las horizontales no aportan cruces
    if (a.y < b.y)
    {
      t = a;
      a = b;
      b = t;
    }
    e[n].yhigh = a.y;
    e[n].ylow = b.y;
    e[n].xhigh = a.x;
    e[n].dxdy = (double)(a.x - b.x) / (double)(a.y - b.y);
    e[n].x = 0.0;
    e[n].active = 0;
    n++;
  }

  *edges = e;
  return n;
}

// Scanline con tabla de aristas activas, de arriba hacia abajo.
// use_texture: cada pixel toma su texel (mapeo simple: coordenada de pantalla modulo
// el tamano de la textura); si no, se usa el color de la provincia.
static void scanline_fill(const POLYGON *p, int use_texture)
{
  POINT *pts;
  EDGE *edges;
  double *xs;
  int count, nedges, ymin, ymax, scanline;
  int i, k, n, x, x0, x1, v = 0;

  pts = polygon_to_clipped_screen(p, &count);
  if (pts == NULL)
    return;

  nedges = build_edges(pts, count, &edges, &ymin, &ymax);
  free(pts);
  if (nedges < 0)
    return;

  xs = (double *)malloc((nedges > 0 ? nedges : 1) * sizeof(double));
  if (xs == NULL)
  {
    free(edges);
    return;
  }

  if (!use_texture)
    color_province(&current_color, p->province);

  if (ymin < 0)
    ymin = 0;
  if (ymax > VRES - 1)
    ymax = VRES - 1;

  for (scanline = ymax; scanline >= ymin; scanline--)
  {
    // activar las aristas que empiezan a cruzar esta linea
    for (i = 0; i < nedges; i++)
      if (!edges[i].active && edges[i].ylow < scanline && scanline <= edges[i].yhigh)
      {
        edges[i].active = 1;
        edges[i].x = edges[i].xhigh + (scanline - edges[i].yhigh) * edges[i].dxdy;
      }

    n = 0;
    for (i = 0; i < nedges; i++)
      if (edges[i].active)
        xs[n++] = edges[i].x;
    qsort(xs, n, sizeof(double), cmp_double);

    if (use_texture)
      v = scanline % texture_h;

    for (k = 0; k + 1 < n; k += 2)
    {
      x0 = (int)lround(xs[k]);
      x1 = (int)lround(xs[k + 1]);
      if (x0 < 0)
        x0 = 0;
      if (x1 > HRES - 1)
        x1 = HRES - 1;
      for (x = x0; x <= x1; x++)
      {
        if (use_texture)
          current_color = texture[x % texture_w][v];
        plot(x, scanline);
      }
    }

    // pasar a la siguiente linea: avanzar x y retirar las que terminaron
    for (i = 0; i < nedges; i++)
      if (edges[i].active)
      {
        edges[i].x -= edges[i].dxdy;
        if (edges[i].ylow >= scanline - 1)
          edges[i].active = 0;
      }
  }

  free(xs);
  free(edges);
}

void scanline_fill_color(const POLYGON *p)
{
  scanline_fill(p, 0);
}

void scanline_fill_texture(const POLYGON *p)
{
  // cada provincia usa su propia textura; si falta el archivo se pinta de color
  data_select_texture(p->province);
  if (texture == NULL || texture_w <= 0 || texture_h <= 0)
    scanline_fill(p, 0);
  else
    scanline_fill(p, 1);
}

void PaintPolygon(const POLYGON *p)
{
  scanline_fill_color(p);
}

void TexturePolygon(const POLYGON *p)
{
  scanline_fill_texture(p);
}
