#include "main.h"

// Framebuffer
COLOR **buffer;

/*
 * current_color:
 * - Color Activo que usa plot_something al pintar. lo dejamos global para que
 * la rutina plot(x, y) reciba solo 2 argumentos como pide el enunciado
 */
COLOR current_color;

static void menu_option(int option)
{
  switch ((MENU)option)
  {
  case DISPLAY_MODE:
    Display();
    break;

  case ZOOM:
    Zoom();
    break;

  case PAN:
    Pan();
    break;

  case ROTATION:
    Rotate();
    break;

  case RESTART:
    Reset();
    break;

  case EXIT:
    for (int i = 0; i < HRES; i++)
      free(buffer[i]);

    free(buffer);
    exit(EXIT_SUCCESS);
  }

  glutPostRedisplay();
}

static void create_menu(void)
{
  glutCreateMenu(menu_option);

  glutAddMenuEntry("Modo de despliegue", DISPLAY_MODE);
  glutAddMenuEntry("Zoom", ZOOM);
  glutAddMenuEntry("Desplazamiento", PAN);
  glutAddMenuEntry("Rotacion", ROTATION);
  glutAddMenuEntry("Reiniciar", RESTART);
  glutAddMenuEntry("Salir", EXIT);

  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

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
    return 1;

  for (i = 0; i < HRES; i++)
  {
    buffer[i] = (COLOR *)malloc(VRES * sizeof(COLOR));
    if (buffer[i] == NULL)
    {
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

  // Primero crear la ventana; después, asociarle el menú.
  glutCreateWindow("Mapa de Costa Rica");
  create_menu();

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-0.5, HRES + 0.5, -0.5, VRES + 0.5);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glutDisplayFunc(draw_scene);
  glutMainLoop();

  //                  //
  // Liberando  Memoria//
  //                  //
  for (i = 0; i < HRES; i++)
    free(buffer[i]);
  free(buffer);
}



//-------------------------------------------------------------------------------
//---------------Funciones PRO1--------------------------------------------------
//-------------------------------------------------------------------------------

void Display(){return;};
void Zoom(){return;};
void Pan(){return;};
void Rotate(){return;};
void Reset(){return;};

//-------------------------------------------------------------------------------
//---------------Funciones PRO0--------------------------------------------------
//-------------------------------------------------------------------------------

