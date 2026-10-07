#include "render.h"
#include "geometry.h"

COLOR **buffer = NULL;
COLOR current_color = {1.0, 1.0, 1.0};
DISPLAY_MODES current_mode = MODE_SIMPLE;

static GLfloat *pixels = NULL; // copia lineal del buffer para glDrawPixels 

// ---------- Ciclo de vida ---------- 

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

// Vuelca el framebuffer a la ventana con una sola llamada (mucho mas rapido que GL_POINTS).
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
    DrawPolygon(p); // los bordes siempre van encima
  }
}

// Callback de GLUT: redibuja TODO el cuadro.
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

// Color de cada provincia (usar al inicio de un algoritmo de trazado).
void color_province(COLOR *c, PROVINCES province)
{
  switch (province)
  {
  case SANJOSE: // Morado
    c->r = 0.5; c->g = 0.0; c->b = 0.5;
    break;
  case ALAJUELA: // Rojo
    c->r = 1.0; c->g = 0.0; c->b = 0.0;
    break;
  case CARTAGO: // Azul 
    c->r = 0.0; c->g = 0.0; c->b = 1.0;
    break;
  case HEREDIA: // Amarillo 
    c->r = 1.0; c->g = 1.0; c->b = 0.0;
    break;
  case GUANACASTE: //        Rosado
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

/* ---------- Relleno (scanline) ---------- */


//Ayuda para ordenar las intersecciones por x
static int cmp_double(const void *a, const void *b)
{
  double x = *(const double *)a, y = *(const double *)b;
  return (x > y) - (x < y);
}

/*
 * Construye la tabla de bordes (sin los horizontales) y devuelve la cantidad.
 * Tambien entrega Ymin/Ymax de los vertices. El que llama hace free(*edges).
 */
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
    if (pts[i].y < *ymin) *ymin = pts[i].y;
    if (pts[i].y > *ymax) *ymax = pts[i].y;

    a = pts[i];
    b = pts[(i + 1) % count];
    if (a.y == b.y)
      continue;
    if (a.y < b.y)
    {
      t = a; a = b; b = t; // a = extremo alto, b = extremo bajo
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

/*
 * Scanline con color solido.
 * En resumen es el algrotimo de Torres, la version 3
 */
static void scanline_fill_color(const POLYGON *p)
{
  POINT *pts;
  EDGE *edges;
  double *xs;
  int nedges, ymin, ymax, scanline;
  int i, k, n, x, x0, x1;

  if (p->count < 3)
    return;

  pts = polygon_to_screen(p);
  if (pts == NULL)
    return;

  nedges = build_edges(pts, p->count, &edges, &ymin, &ymax);
  free(pts);
  if (nedges < 0)
    return;

  xs = (double *)malloc((nedges > 0 ? nedges : 1) * sizeof(double));
  if (xs == NULL)
  {
    free(edges);
    return;
  }

  color_province(&current_color, p->province);

  if (ymin < 0) ymin = 0;
  if (ymax > VRES - 1) ymax = VRES - 1;

  scanline = ymax;
  while (scanline >= ymin)
  {
    // Activar bordes 
    for (i = 0; i < nedges; i++)
      if (!edges[i].active && edges[i].ylow < scanline && scanline <= edges[i].yhigh)
      {
        edges[i].active = 1;
        edges[i].x = edges[i].xhigh + (scanline - edges[i].yhigh) * edges[i].dxdy;
      }

    // Ordenar intersecciones 
    n = 0;
    for (i = 0; i < nedges; i++)
      if (edges[i].active)
        xs[n++] = edges[i].x;
    qsort(xs, n, sizeof(double), cmp_double);

    // Pintar en pares 
    for (k = 0; k + 1 < n; k += 2)
    {
      x0 = (int)lround(xs[k]);
      x1 = (int)lround(xs[k + 1]);
      if (x0 < 0) x0 = 0;
      if (x1 > HRES - 1) x1 = HRES - 1;
      for (x = x0; x <= x1; x++)
        plot(x, scanline);
    }

    // Actualizar bordes (la siguiente scanline es una unidad mas abajo) 
    for (i = 0; i < nedges; i++)
      if (edges[i].active)
        edges[i].x -= edges[i].dxdy;

    // Desactivar bordes que no cubren scanline - 1 
    for (i = 0; i < nedges; i++)
      if (edges[i].active && edges[i].ylow >= scanline - 1)
        edges[i].active = 0;

    scanline--;
  }

  free(xs);
  free(edges);
}

/*
 * Scanline con textura: mismo algoritmo, pero cada pixel toma su color de
 * texture[][] (repetida en mosaico segun la posicion en pantalla).
 * Si no hay textura cargada, rellena con el color solido de la provincia.
 */
static void scanline_fill_texture(const POLYGON *p)
{
  POINT *pts;
  EDGE *edges;
  double *xs;
  int nedges, ymin, ymax, scanline;
  int i, k, n, x, x0, x1, u, v;

  if (texture == NULL || texture_w <= 0 || texture_h <= 0)
  {
    scanline_fill_color(p);
    return;
  }

  if (p->count < 3)
    return;

  pts = polygon_to_screen(p);
  if (pts == NULL)
    return;

  nedges = build_edges(pts, p->count, &edges, &ymin, &ymax);
  free(pts);
  if (nedges < 0)
    return;

  xs = (double *)malloc((nedges > 0 ? nedges : 1) * sizeof(double));
  if (xs == NULL)
  {
    free(edges);
    return;
  }

  if (ymin < 0) ymin = 0;
  if (ymax > VRES - 1) ymax = VRES - 1;

  scanline = ymax;
  while (scanline >= ymin)
  {
    // Activar bordes
    for (i = 0; i < nedges; i++)
      if (!edges[i].active && edges[i].ylow < scanline && scanline <= edges[i].yhigh)
      {
        edges[i].active = 1;
        edges[i].x = edges[i].xhigh + (scanline - edges[i].yhigh) * edges[i].dxdy;
      }

    // Ordenar intersecciones 
    n = 0;
    for (i = 0; i < nedges; i++)
      if (edges[i].active)
        xs[n++] = edges[i].x;
    qsort(xs, n, sizeof(double), cmp_double);

    // Pintar en pares con la textura 
    v = scanline % texture_h; // scanline >= 0 aqui
    for (k = 0; k + 1 < n; k += 2)
    {
      x0 = (int)lround(xs[k]);
      x1 = (int)lround(xs[k + 1]);
      if (x0 < 0) x0 = 0;
      if (x1 > HRES - 1) x1 = HRES - 1;
      for (x = x0; x <= x1; x++)
      {
        u = x % texture_w; /* x >= 0 aqui */
        current_color = texture[u][v];
        plot(x, scanline);
      }
    }

    // Actualizar bordes 
    for (i = 0; i < nedges; i++)
      if (edges[i].active)
        edges[i].x -= edges[i].dxdy;

    // Desactivar bordes 
    for (i = 0; i < nedges; i++)
      if (edges[i].active && edges[i].ylow >= scanline - 1)
        edges[i].active = 0;

    scanline--;
  }

  free(xs);
  free(edges);
}

/* Relleno de color solido. */
void PaintPolygon(const POLYGON *p)
{
  scanline_fill_color(p);
}

/* Relleno con textura. */
void TexturePolygon(const POLYGON *p)
{
  scanline_fill_texture(p);
}