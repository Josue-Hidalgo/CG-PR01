#include "GUI.h"
#include "render.h"
#include "geometry.h"
#include "data.h"

#define ZOOM_STEP 1.1
#define PAN_STEP 20.0
#define ROTATE_STEP 0.1 /* radianes */

void gui_quit(void)
{
  exit(EXIT_SUCCESS); /* cleanup() de main.c se ejecuta por atexit */
}

static void menu_option(int option)
{
  switch ((MENU)option)
  {
  case DISPLAY_MODE:
    current_mode = (DISPLAY_MODES)((current_mode + 1) % MODE_COUNT);
    break;
  case ZOOM_IN:
    geometry_zoom(ZOOM_STEP);
    break;
  case ZOOM_OUT:
    geometry_zoom(1.0 / ZOOM_STEP);
    break;
  case ROTATION:
    geometry_rotate(ROTATE_STEP);
    break;
  case RESTART:
    geometry_reset();
    break;
  case EXIT:
    gui_quit();
    break;
  }

  glutPostRedisplay();
}

static void create_menu(void)
{
  glutCreateMenu(menu_option);

  glutAddMenuEntry("Modo de despliegue", DISPLAY_MODE);
  glutAddMenuEntry("Zoom +", ZOOM_IN);
  glutAddMenuEntry("Zoom -", ZOOM_OUT);
  glutAddMenuEntry("Rotacion", ROTATION);
  glutAddMenuEntry("Reiniciar", RESTART);
  glutAddMenuEntry("Salir", EXIT);

  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/* Teclas: + - zoom | r/R rotar | m modo | 0 reiniciar | ESC salir */
static void keyboard(unsigned char key, int x, int y)
{
  (void)x;
  (void)y;

  switch (key)
  {
  case '+': geometry_zoom(ZOOM_STEP); break;
  case '-': geometry_zoom(1.0 / ZOOM_STEP); break;
  case 'r': geometry_rotate(ROTATE_STEP); break;
  case 'R': geometry_rotate(-ROTATE_STEP); break;
  case 'm': menu_option(DISPLAY_MODE); return;
  case '0': geometry_reset(); break;
  case 27: gui_quit(); break;
  default: return;
  }
  glutPostRedisplay();
}

/* Flechas: desplazar el mapa */
static void special(int key, int x, int y)
{
  (void)x;
  (void)y;

  switch (key)
  {
  case GLUT_KEY_LEFT:  geometry_pan(-PAN_STEP, 0); break;
  case GLUT_KEY_RIGHT: geometry_pan(PAN_STEP, 0); break;
  case GLUT_KEY_UP:    geometry_pan(0, PAN_STEP); break;
  case GLUT_KEY_DOWN:  geometry_pan(0, -PAN_STEP); break;
  default: return;
  }
  glutPostRedisplay();
}

void gui_init(void)
{
  create_menu();
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
}
