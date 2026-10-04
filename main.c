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
  //    Argumentos    //
  //                  //

  // Restricción: # de argumentos
  if (argc != 4)
  {
    printf("Uso: %s <resolucion> <# lineas> <# veces>\n", argv[0]);
    printf("Ejemplo: %s 256 100 500\n", argv[0]);
    return 1;
  }

  // Restricción: Deben ser números
  // Arreglo: la version original usaba "end" sin inicializar (leia basura y
  // podia botar el programa). ahora convertimos con strtol y revisamos que
  // el argumento sea de verdad un numero :)

  char *endptr;
  long vals[3];
  const char *nombres[3] = {"la resolucion", "el # de lineas", "el # de veces"};

  for (int i = 0; i < 3; i++)
  {
    errno = 0;
    vals[i] = strtol(argv[i + 1], &endptr, 10);

    // no era un numero (o traia basura pegada, tipo "12abc")
    if (endptr == argv[i + 1] || *endptr != '\0')
    {
      printf("Mmm.. %s ('%s') no parece un numero entero valido :/  probemos otra vez!\n", nombres[i], argv[i + 1]);
      return 1;
    }

    // el numero era valido pero gigantesco, no cabe en un int
    if (errno != 0 || vals[i] > 2147483647L || vals[i] < -2147483648L)
    {
      printf("Wow, %s ('%s') se pasa de grande!! usemos un numero mas pequeño porfa..\n", nombres[i], argv[i + 1]);
      return 1;
    }
  }

  // Obteniendo Argumentos
  RES = (int)vals[0];    // Resolucion
  NLINES = (int)vals[1]; // # Líneas
  NTIMES = (int)vals[2]; // # Veces

  // sin esto un RES de 0 o negativo revienta el malloc y los algoritmos 
  if (RES <= 0)
  {
    printf("La resolucion tiene que ser mayor que 0, (me pasaste %d) :c\n", RES);
    return 1;
  }
  if (NLINES < 0 || NTIMES < 0)
  {
    printf("Ni el # de lineas ni el # de veces pueden ser negativos\n");
    return 1;
  }

  //                  //
  // Generando Líneas //
  //                  //

  // Reserva de memoria
  lines = (LINE *)malloc(NLINES * sizeof(LINE));

  // si pidieron lineas pero no hubo memoria, mejor cancelamos y avisamos
  if (NLINES > 0 && lines == NULL)
  {
    printf("Uy, no me alcanzo la memoria para %d lineas :c  probemos con menos!\n", NLINES);
    return 1;
  }

  for (int i = 0; i < NLINES; i++)
    lines[i] = generate_random_line();

  //                  //
  //      Ventana     //
  //                  //

  // i = scanline     // j = columna
  int i, j;

  // --- Reserva de Memoria ---
  buffer = (COLOR **)malloc(RES * sizeof(COLOR *));

  // si la resolucion es enorme puede que no quepa en memoria, lo revisamos
  // para no explotar mas adelante :]
  if (buffer == NULL)
  {
    printf("Uy, no me alcanzo la memoria para una ventana de %dx%d :c  probemos una resolucion mas pequeña!\n", RES, RES);
    return 1;
  }

  for (i = 0; i < RES; i++)
  {
    buffer[i] = (COLOR *)malloc(RES * sizeof(COLOR));

    // si falla a medio camino, liberamos lo que ya pedimos
    if (buffer[i] == NULL)
    {
      printf("Uy, no me alcanzo la memoria para una ventana de %dx%d :c  probemos una resolucion mas pequeña!\n", RES, RES);
      for (j = 0; j < i; j++)
        free(buffer[j]);
      free(buffer);
      free(lines);
      return 1;
    }
  }

  // --- Pantalla en Negro ---
  for (i = 0; i < RES; i++)
    for (j = 0; j < RES; j++)
    {
      buffer[i][j].r = 0;
      buffer[i][j].g = 0;
      buffer[i][j].b = 0;
    }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(RES, RES);
  glutCreateWindow("Mesa Example");
  glClear(GL_COLOR_BUFFER_BIT);
  gluOrtho2D(-0.5, RES + 0.5, -0.5, RES + 0.5);
  glutDisplayFunc(draw_scene);

  
  


  //                  //
  //  Dibujar Líneas  //
  //                  //

  // primera pasada: plot vacio, medimos el tiempo puro de cada algoritmo
  plot = plot_nothing;
  run_all_algorithms(0);

  // segunda pasada: plot que pinta en el buffer (version GLUT/Mesa)
  // ojo: NO se borra nada entre algoritmos, asi se ve si todos escogen
  // exactamente los mismos pixeles (deberia quedar de un solo color al final)
  plot = plot_something;
  run_all_algorithms(1);

  // resultados finales en la consola, como pide el enunciado
  print_results();

  //                  //
  //  Fin del Archivo //
  //                  //

  // --- Cosas de Inicialización de las Bibliotecas / Headers ---
  glutMainLoop();

  //                  //
  //Liberando  Memoria//
  //                  //

  free(lines);
  for (i = 0; i < RES; i++)
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
    for (j = 0; j < RES; j++)
    {
      glColor3f(buffer[i][j].r, buffer[i][j].g, buffer[i][j].b);
      glBegin(GL_POINTS);
      glVertex2i(i, j);
      glEnd();
    }

  for (i = last_x; i < RES; i++)
    for (j = 0; j < RES; j++)
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
  if (x < 0 || x >= RES || y < 0 || y >= RES)
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
