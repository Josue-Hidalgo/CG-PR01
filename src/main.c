#include "main.h"
#include "render.h"
#include "data.h"
#include "geometry.h"
#include "GUI.h"

static void cleanup(void)
{
  data_free_map();
  data_free_texture();
  render_destroy();
}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(HRES + PANEL_W, VRES);
  glutCreateWindow("Mapa de Costa Rica");

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-0.5, HRES + PANEL_W + 0.5, -0.5, VRES + 0.5);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if (render_init() != 0)
  {
    fprintf(stderr, "Error: no se pudo reservar el framebuffer\n");
    return 1;
  }
  atexit(cleanup);

  if (data_load_map("assets/cr.json") != 0)
    fprintf(stderr, "Aviso: no se pudo cargar assets/cr.json\n");
  if (data_load_textures() != 0)
    fprintf(stderr, "Aviso: no se pudieron cargar todas las texturas de assets/\n");

  geometry_reset();
  gui_init();

  glutDisplayFunc(draw_scene);
  glutMainLoop();
  return 0;
}
