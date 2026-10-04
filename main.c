#include "main.h"

COLOR **buffer;           // Framebuffer

/*
 * current_color:
 * Color Activo que usa plot_something al pintar. lo dejamos global para que
 * la rutina plot(x, y) reciba solo 2 argumentos como pide el enunciado
 */
COLOR current_color;

int main(int argc, char *argv[])
{
  
  //                  //
  //      Ventana     //
  //                  //

  // i = scanline     // j = columna
  int i, j;

  // --- Reserva de Memoria ---
  buffer = (COLOR **)malloc(HRES * sizeof(COLOR *));
  if (buffer == NULL)
  {
    printf("Uy, no me alcanzo la memoria para una ventana de %dx%d :c  probemos una resolucion mas pequeña!\n", HRES, VRES);
    return 1;
  }

  for (i = 0; i < HRES; i++)
  {
    buffer[i] = (COLOR *)malloc(VRES * sizeof(COLOR));
    if (buffer[i] == NULL)
    {
      printf("Uy, no me alcanzo la memoria para una ventana de %dx%d :c  probemos una resolucion mas pequeña!\n", HRES, VRES);
      for (j = 0; j < i; j++)
        free(buffer[j]);
      free(buffer);
      return 1;
    }
  }

  // --- Pantalla en Negro ---
  for (i = 0; i < HRES; i++)
    for (j = 0; j < VRES; j++)
    {
      buffer[i][j].r = 0;
      buffer[i][j].g = 0;
      buffer[i][j].b = 0;
    }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(HRES, VRES);
  glutCreateWindow("Mesa Example");
  glClear(GL_COLOR_BUFFER_BIT);
  gluOrtho2D(-0.5, HRES + 0.5, -0.5, VRES + 0.5);
  glutDisplayFunc(draw_scene);

  //                  //
  //  Fin del Archivo //
  //                  //

  // --- Cosas de Inicialización de las Bibliotecas / Headers ---
  glutMainLoop();

  //                  //
  //Liberando  Memoria//
  //                  //

  for (i = 0; i < HRES; i++)
    free(buffer[i]);
  free(buffer);
}

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
    glFlush();
}

//-------------------------------------------------------------------------------
//---------------Funciones PRO1--------------------------------------------------
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
//---------------Funciones PRO0--------------------------------------------------
//-------------------------------------------------------------------------------

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

//                          //
//   ALGORITMOS DE LINEAS   //
//                          //

/*
 * bresenham(int x0, int y0, int x1, int y1):
 * - Algoritmo de Bresenham o Punto Medio para los 8 octantes.
 */
void bresenham(int x0, int y0, int x1, int y1)
{
    int dx = x1 - x0;
    int dy = y1 - y0;

    int adx = abs(dx);
    int ady = abs(dy);

    int xp = x0;
    int yp = y0;

    int d;
    int delta_E;
    int delta_NE;

    plot(xp, yp);

    /*
     * OCTANTE 1
     * dx > 0, dy > 0
     * 0 <= pendiente <= 1
     *
     * E  = (x+1, y)
     * NE = (x+1, y+1)
     */
    if (dx >= 0 && dy >= 0 && adx >= ady)
    {
        delta_E  = 2 * ady;
        delta_NE = 2 * (ady - adx);
        d = 2 * ady - adx;

        while (xp < x1)
        {
            if (d < 0)
            {
                // E
                xp++;
                d += delta_E;
            }
            else
            {
                // NE
                xp++;
                yp++;
                d += delta_NE;
            }

            plot(xp, yp);
        }
    }

    /*
     * OCTANTE 2
     * dx > 0, dy > 0
     * pendiente > 1
     *
     * N  = (x, y+1)
     * NE = (x+1, y+1)
     */
    else if (dx >= 0 && dy >= 0 && ady > adx)
    {
        delta_E  = 2 * adx;
        delta_NE = 2 * (adx - ady);
        d = 2 * adx - ady;

        while (yp < y1)
        {
            if (d < 0)
            {
                // N
                yp++;
                d += delta_E;
            }
            else
            {
                // NE
                xp++;
                yp++;
                d += delta_NE;
            }

            plot(xp, yp);
        }
    }

    /*
     * OCTANTE 3
     * dx < 0, dy > 0
     * pendiente < -1
     *
     * N  = (x, y+1)
     * NW = (x-1, y+1)
     */
    else if (dx < 0 && dy >= 0 && ady > adx)
    {
        delta_E  = 2 * adx;
        delta_NE = 2 * (adx - ady);
        d = 2 * adx - ady;

        while (yp < y1)
        {
            if (d < 0)
            {
                // N
                yp++;
                d += delta_E;
            }
            else
            {
                // NW
                xp--;
                yp++;
                d += delta_NE;
            }

            plot(xp, yp);
        }
    }

    /*
     * OCTANTE 4
     * dx < 0, dy > 0
     * -1 <= pendiente < 0
     *
     * W  = (x-1, y)
     * NW = (x-1, y+1)
     */
    else if (dx < 0 && dy >= 0 && adx >= ady)
    {
        delta_E  = 2 * ady;
        delta_NE = 2 * (ady - adx);
        d = 2 * ady - adx;

        while (xp > x1)
        {
            if (d < 0)
            {
                // W
                xp--;
                d += delta_E;
            }
            else
            {
                // NW
                xp--;
                yp++;
                d += delta_NE;
            }

            plot(xp, yp);
        }
    }

    /*
     * OCTANTE 5
     * dx < 0, dy < 0
     * 0 <= pendiente <= 1
     * (visto avanzando hacia la izquierda)
     *
     * W  = (x-1, y)
     * SW = (x-1, y-1)
     */
    else if (dx < 0 && dy < 0 && adx >= ady)
    {
        delta_E  = 2 * ady;
        delta_NE = 2 * (ady - adx);
        d = 2 * ady - adx;

        while (xp > x1)
        {
            if (d < 0)
            {
                // W
                xp--;
                d += delta_E;
            }
            else
            {
                // SW
                xp--;
                yp--;
                d += delta_NE;
            }

            plot(xp, yp);
        }
    }

    /*
     * OCTANTE 6
     * dx < 0, dy < 0
     * pendiente > 1
     *
     * S  = (x, y-1)
     * SW = (x-1, y-1)
     */
    else if (dx < 0 && dy < 0 && ady > adx)
    {
        delta_E  = 2 * adx;
        delta_NE = 2 * (adx - ady);
        d = 2 * adx - ady;

        while (yp > y1)
        {
            if (d < 0)
            {
                // S
                yp--;
                d += delta_E;
            }
            else
            {
                // SW
                xp--;
                yp--;
                d += delta_NE;
            }

            plot(xp, yp);
        }
    }

    /*
     * OCTANTE 7
     * dx > 0, dy < 0
     * pendiente < -1
     *
     * S  = (x, y-1)
     * SE = (x+1, y-1)
     */
    else if (dx >= 0 && dy < 0 && ady > adx)
    {
        delta_E  = 2 * adx;
        delta_NE = 2 * (adx - ady);
        d = 2 * adx - ady;

        while (yp > y1)
        {
            if (d < 0)
            {
                // S
                yp--;
                d += delta_E;
            }
            else
            {
                // SE
                xp++;
                yp--;
                d += delta_NE;
            }

            plot(xp, yp);
        }
    }

    /*
     * OCTANTE 8
     * dx > 0, dy < 0
     * -1 <= pendiente < 0
     *
     * E  = (x+1, y)
     * SE = (x+1, y-1)
     */
    else if (dx >= 0 && dy < 0 && adx >= ady)
    {
        delta_E  = 2 * ady;
        delta_NE = 2 * (ady - adx);
        d = 2 * ady - adx;

        while (xp < x1)
        {
            if (d < 0)
            {
                // E
                xp++;
                d += delta_E;
            }
            else
            {
                // SE
                xp++;
                yp--;
                d += delta_NE;
            }

            plot(xp, yp);
        }
    }
}
