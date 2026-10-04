#include "render.h"

COLOR **buffer;
COLOR current_color;

/*
 * draw_scene():
 * - Función que Ejecuta el Glut al momento del Refresco.
 */
void draw_scene()
{
  static int last_x = 0;
  int i, j;
  COLOR color;

  for (i = 0; i < last_x; i++)
    for (j = 0; j < VRES; j++)
    {
      glColor3f(buffer[i][j].r, buffer[i][j].g, buffer[i][j].b);
      glBegin(GL_POINTS);
      glVertex2i(i, j);
      glEnd();
    }

  for (i = last_x; i < HRES; i++)
    for (j = 0; j < VRES; j++)
    {

      glColor3f(buffer[i][j].r, buffer[i][j].g, buffer[i][j].b);
      glBegin(GL_POINTS);
      glVertex2i(i, j);
      glEnd();
      last_x = i;
    }

  // Línea blanca con extremos dentro del framebuffer.
  color_province(&current_color, CARTAGO);
  bresenham(HRES / 4, VRES / 4, 3 * HRES / 4, 3 * VRES / 4);

  glFlush();
}

/*
 * plot(int x, int y):
 * - Pinta en el buffer un pixel con el color activo (current_color).
 * - Cambio respecto al original: antes recibia el COLOR por parametro, ahora
 *   usa el global current_color para que plot sea plot(x, y) de 2 argumentos
 *   igual que plot_nothing y se puedan intercambiar con un puntero.
 */
void plot(int x, int y)
{
  // Ángel de la Guarda, dulce compañía, no me desampares ni de noche ni de día.
  if (x < 0 || x >= HRES || y < 0 || y >= VRES)
    return;

  buffer[x][y] = current_color;

  glColor3f(current_color.r, current_color.g, current_color.b);
  glBegin(GL_POINTS);
  glVertex2i(x, y);
  glEnd();
}

/*
 * color_province(COLOR c, PROVINCES province):
 * - Función cambia el color de una línea de un polígono de la provincia para antes de dibujarla.
 * - Uso: usar esta función al inicio de aplicar un algoritmo de trazado.
 */
void color_province(COLOR *c, PROVINCES province)
{
  switch (province)
  {
  case SANJOSE:
    // Morado
    c->r = 0.5;
    c->g = 0.0;
    c->b = 0.5;
    break;

  case ALAJUELA:
    // Rojo
    c->r = 1.0;
    c->g = 0.0;
    c->b = 0.0;
    break;

  case CARTAGO:
    // Azul
    c->r = 0.0;
    c->g = 0.0;
    c->b = 1.0;
    break;

  case HEREDIA:
    // Amarillo
    c->r = 1.0;
    c->g = 1.0;
    c->b = 0.0;
    break;

  case GUANACASTE:
    // Rosado
    c->r = 1.0;
    c->g = 0.0;
    c->b = 0.5;
    break;

  case PUNTARENAS:
    // Naranja
    c->r = 1.0;
    c->g = 0.5;
    c->b = 0.0;
    break;

  case LIMON:
    // Verde
    c->r = 0.0;
    c->g = 1.0;
    c->b = 0.0;
    break;

  default:
    break;
  }
}