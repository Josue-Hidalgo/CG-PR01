#include "GUI.h"
#include "render.h"
#include "geometry.h"
#include "data.h"
#include <string.h>
#include <ctype.h>

#define ZOOM_STEP 1.1
#define PAN_STEP 20.0
#define ROTATE_STEP 0.1

#define SPEED_FAST 3.0
#define SPEED_SLOW (1.0 / 3.0)

#define PANEL_MARGIN 10
#define BUTTON_H 28
#define BUTTON_GAP 6

typedef enum
{
  ACT_MODE_SIMPLE,
  ACT_MODE_FILL,
  ACT_MODE_TEXTURE,
  ACT_ZOOM_IN,
  ACT_ZOOM_OUT,
  ACT_PAN_LEFT,
  ACT_PAN_RIGHT,
  ACT_PAN_UP,
  ACT_PAN_DOWN,
  ACT_ROTATE_CW,
  ACT_ROTATE_CCW,
  ACT_RESET,
  ACT_EXIT,
  ACT_COUNT
} ACTION;

typedef struct
{
  int x, y, w, h;
  const char *label;
  ACTION action;
} BUTTON;

typedef struct
{
  int x, y;
  const char *text;
} LABEL;

static BUTTON buttons[ACT_COUNT];
static int button_count = 0;
static LABEL labels[8];
static int label_count = 0;
static int help_y = 0;

static GLfloat *panel_pixels = NULL;
static COLOR panel_color = {1.0, 1.0, 1.0};

static double current_speed(void)
{
  int mods = glutGetModifiers();

  if (mods & GLUT_ACTIVE_CTRL)
    return SPEED_SLOW;
  if (mods & GLUT_ACTIVE_SHIFT)
    return SPEED_FAST;
  return 1.0;
}

static void do_action(ACTION action, double speed)
{
  switch (action)
  {
  case ACT_MODE_SIMPLE:
    current_mode = MODE_SIMPLE;
    break;
  case ACT_MODE_FILL:
    current_mode = MODE_FILL;
    break;
  case ACT_MODE_TEXTURE:
    current_mode = MODE_TEXTURE;
    break;
  case ACT_ZOOM_IN:
    geometry_zoom(pow(ZOOM_STEP, speed));
    break;
  case ACT_ZOOM_OUT:
    geometry_zoom(1.0 / pow(ZOOM_STEP, speed));
    break;
  case ACT_PAN_LEFT:
    geometry_pan(-PAN_STEP * speed, 0);
    break;
  case ACT_PAN_RIGHT:
    geometry_pan(PAN_STEP * speed, 0);
    break;
  case ACT_PAN_UP:
    geometry_pan(0, PAN_STEP * speed);
    break;
  case ACT_PAN_DOWN:
    geometry_pan(0, -PAN_STEP * speed);
    break;
  case ACT_ROTATE_CW:
    geometry_rotate(-ROTATE_STEP * speed);
    break;
  case ACT_ROTATE_CCW:
    geometry_rotate(ROTATE_STEP * speed);
    break;
  case ACT_RESET:
    geometry_reset();
    break;
  case ACT_EXIT:
    gui_quit();
    break;
  default:
    return;
  }
  glutPostRedisplay();
}

void gui_quit(void)
{
  free(panel_pixels);
  panel_pixels = NULL;
  exit(EXIT_SUCCESS);
}

static void menu_option(int option)
{
  switch ((MENU)option)
  {
  case DISPLAY_MODE:
    current_mode = (DISPLAY_MODES)((current_mode + 1) % MODE_COUNT);
    glutPostRedisplay();
    break;
  case ZOOM_IN:
    do_action(ACT_ZOOM_IN, 1.0);
    break;
  case ZOOM_OUT:
    do_action(ACT_ZOOM_OUT, 1.0);
    break;
  case ROTATION:
    do_action(ACT_ROTATE_CCW, 1.0);
    break;
  case RESTART:
    do_action(ACT_RESET, 1.0);
    break;
  case EXIT:
    do_action(ACT_EXIT, 1.0);
    break;
  }
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

static void keyboard(unsigned char key, int x, int y)
{
  double speed = current_speed();
  (void)x;
  (void)y;

  if ((glutGetModifiers() & GLUT_ACTIVE_CTRL) && key >= 1 && key <= 26)
    key = key + 'a' - 1;
  key = (unsigned char)tolower(key);

  switch (key)
  {
  case '1':
    do_action(ACT_MODE_SIMPLE, speed);
    break;
  case '2':
    do_action(ACT_MODE_FILL, speed);
    break;
  case '3':
    do_action(ACT_MODE_TEXTURE, speed);
    break;
  case 'm':
    menu_option(DISPLAY_MODE);
    break;
  case '+':
  case '=':
    do_action(ACT_ZOOM_IN, speed);
    break;
  case '-':
  case '_':
    do_action(ACT_ZOOM_OUT, speed);
    break;
  case 'e':
    do_action(ACT_ROTATE_CW, speed);
    break;
  case 'q':
    do_action(ACT_ROTATE_CCW, speed);
    break;
  case 'r':
    do_action(ACT_RESET, speed);
    break;
  case 27:
    do_action(ACT_EXIT, speed);
    break;
  default:
    break;
  }
}

static void special(int key, int x, int y)
{
  double speed = current_speed();
  (void)x;
  (void)y;

  switch (key)
  {
  case GLUT_KEY_LEFT:
    do_action(ACT_PAN_LEFT, speed);
    break;
  case GLUT_KEY_RIGHT:
    do_action(ACT_PAN_RIGHT, speed);
    break;
  case GLUT_KEY_UP:
    do_action(ACT_PAN_UP, speed);
    break;
  case GLUT_KEY_DOWN:
    do_action(ACT_PAN_DOWN, speed);
    break;
  default:
    break;
  }
}

static const unsigned char font[128][7] = {
    ['A'] = {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11},
    ['B'] = {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E},
    ['C'] = {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E},
    ['D'] = {0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E},
    ['E'] = {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F},
    ['F'] = {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10},
    ['G'] = {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F},
    ['H'] = {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11},
    ['I'] = {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E},
    ['J'] = {0x07, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0C},
    ['K'] = {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11},
    ['L'] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F},
    ['M'] = {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11},
    ['N'] = {0x11, 0x11, 0x19, 0x15, 0x13, 0x11, 0x11},
    ['O'] = {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E},
    ['P'] = {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10},
    ['Q'] = {0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D},
    ['R'] = {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11},
    ['S'] = {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E},
    ['T'] = {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},
    ['U'] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E},
    ['V'] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04},
    ['W'] = {0x11, 0x11, 0x11, 0x15, 0x15, 0x15, 0x0A},
    ['X'] = {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11},
    ['Y'] = {0x11, 0x11, 0x11, 0x0A, 0x04, 0x04, 0x04},
    ['Z'] = {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F},
    ['0'] = {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E},
    ['1'] = {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E},
    ['2'] = {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F},
    ['3'] = {0x1F, 0x02, 0x04, 0x02, 0x01, 0x11, 0x0E},
    ['4'] = {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02},
    ['5'] = {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E},
    ['6'] = {0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E},
    ['7'] = {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08},
    ['8'] = {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E},
    ['9'] = {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C},
    ['+'] = {0x00, 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00},
    ['-'] = {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00},
    ['/'] = {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x00},
    [':'] = {0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x00},
    ['.'] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C},
    ['<'] = {0x00, 0x04, 0x08, 0x1F, 0x08, 0x04, 0x00},
    ['>'] = {0x00, 0x04, 0x02, 0x1F, 0x02, 0x04, 0x00},
    ['^'] = {0x04, 0x0E, 0x15, 0x04, 0x04, 0x04, 0x00},
    ['v'] = {0x00, 0x04, 0x04, 0x04, 0x15, 0x0E, 0x04},
};

static void set_color(double r, double g, double b)
{
  panel_color.r = r;
  panel_color.g = g;
  panel_color.b = b;
}

static void panel_plot(int x, int y)
{
  int idx;

  if (x < 0 || x >= PANEL_W || y < 0 || y >= VRES)
    return;

  idx = 3 * (y * PANEL_W + x);
  panel_pixels[idx] = (GLfloat)panel_color.r;
  panel_pixels[idx + 1] = (GLfloat)panel_color.g;
  panel_pixels[idx + 2] = (GLfloat)panel_color.b;
}

static void fill_rect(int x, int y, int w, int h)
{
  int i, j;
  for (i = x; i < x + w; i++)
    for (j = y; j < y + h; j++)
      panel_plot(i, j);
}

static int text_width(const char *text, int scale)
{
  int len = (int)strlen(text);
  return len == 0 ? 0 : (len * 6 - 1) * scale;
}

// (x, y) = esquina inferior izquierda del texto
static void draw_text(int x, int y, const char *text, int scale)
{
  const unsigned char *glyph;
  int row, col;

  for (; *text != '\0'; text++, x += 6 * scale)
  {
    glyph = font[(unsigned char)*text & 0x7F];
    for (row = 0; row < 7; row++)
      for (col = 0; col < 5; col++)
        if (glyph[row] & (0x10 >> col))
          fill_rect(x + col * scale, y + (6 - row) * scale, scale, scale);
  }
}

static void add_button(int x, int y, int w, const char *label, ACTION action)
{
  BUTTON *b = &buttons[button_count++];
  b->x = x;
  b->y = y;
  b->w = w;
  b->h = BUTTON_H;
  b->label = label;
  b->action = action;
}

static void add_label(int x, int y, const char *text)
{
  labels[label_count].x = x;
  labels[label_count].y = y;
  labels[label_count].text = text;
  label_count++;
}

static void layout_panel(void)
{
  int x = PANEL_MARGIN;
  int w = PANEL_W - 2 * PANEL_MARGIN;
  int half = (w - BUTTON_GAP) / 2;
  int third = (w - 2 * BUTTON_GAP) / 3;
  int y = VRES - 50;

  button_count = 0;
  label_count = 0;

  add_label(x, y -= 20, "CAMBIO DE MODO");
  add_button(x, y -= BUTTON_H + 4, w, "BORDES", ACT_MODE_SIMPLE);
  add_button(x, y -= BUTTON_H + BUTTON_GAP, w, "COLOR", ACT_MODE_FILL);
  add_button(x, y -= BUTTON_H + BUTTON_GAP, w, "TEXTURA", ACT_MODE_TEXTURE);

  add_label(x, y -= 24, "ZOOM");
  y -= BUTTON_H + 4;
  add_button(x, y, half, "+", ACT_ZOOM_IN);
  add_button(x + half + BUTTON_GAP, y, half, "-", ACT_ZOOM_OUT);

  // flechas acomodadas como en el teclado
  add_label(x, y -= 24, "PAN");
  y -= BUTTON_H + 4;
  add_button(x + third + BUTTON_GAP, y, third, "^", ACT_PAN_UP);
  y -= BUTTON_H + BUTTON_GAP;
  add_button(x, y, third, "<", ACT_PAN_LEFT);
  add_button(x + third + BUTTON_GAP, y, third, "v", ACT_PAN_DOWN);
  add_button(x + 2 * (third + BUTTON_GAP), y, third, ">", ACT_PAN_RIGHT);

  add_label(x, y -= 24, "ROTACION");
  add_button(x, y -= BUTTON_H + 4, w, "HORARIO", ACT_ROTATE_CW);
  add_button(x, y -= BUTTON_H + BUTTON_GAP, w, "ANTIHORARIO", ACT_ROTATE_CCW);

  y -= 16;
  add_button(x, y -= BUTTON_H, w, "REINICIAR", ACT_RESET);
  add_button(x, y -= BUTTON_H + BUTTON_GAP, w, "TERMINAR", ACT_EXIT);

  help_y = y - 24;
}

static int is_active(const BUTTON *b)
{
  return (b->action == ACT_MODE_SIMPLE && current_mode == MODE_SIMPLE) ||
         (b->action == ACT_MODE_FILL && current_mode == MODE_FILL) ||
         (b->action == ACT_MODE_TEXTURE && current_mode == MODE_TEXTURE);
}

static void draw_button(const BUTTON *b)
{
  set_color(0.55, 0.55, 0.6);
  fill_rect(b->x, b->y, b->w, b->h);
  if (is_active(b))
    set_color(0.15, 0.4, 0.75);
  else
    set_color(0.22, 0.22, 0.26);
  fill_rect(b->x + 1, b->y + 1, b->w - 2, b->h - 2);

  set_color(1.0, 1.0, 1.0);
  draw_text(b->x + (b->w - text_width(b->label, 2)) / 2, b->y + (b->h - 14) / 2, b->label, 2);
}

void gui_draw_panel(void)
{
  static const char *help[] = {
      "TECLAS",
      "1 2 3 / M  MODO",
      "+ -        ZOOM",
      "FLECHAS    PAN",
      "Q E        ROTAR",
      "R          REINICIAR",
      "ESC        TERMINAR",
      "",
      "SHIFT: RAPIDO",
      "CTRL:  LENTO"};
  int i;

  if (panel_pixels == NULL)
    return;

  set_color(0.12, 0.12, 0.14);
  fill_rect(0, 0, PANEL_W, VRES);
  set_color(0.55, 0.55, 0.6);
  fill_rect(0, 0, 2, VRES);

  set_color(1.0, 1.0, 1.0);
  draw_text((PANEL_W - text_width("MENU", 3)) / 2, VRES - 40, "MENU", 3);

  set_color(0.65, 0.65, 0.7);
  for (i = 0; i < label_count; i++)
    draw_text(labels[i].x, labels[i].y, labels[i].text, 1);

  for (i = 0; i < button_count; i++)
    draw_button(&buttons[i]);

  set_color(0.65, 0.65, 0.7);
  for (i = 0; i < (int)(sizeof(help) / sizeof(help[0])); i++)
    draw_text(PANEL_MARGIN, help_y - i * 12, help[i], 1);

  glRasterPos2i(HRES, 0);
  glDrawPixels(PANEL_W, VRES, GL_RGB, GL_FLOAT, panel_pixels);
}

static void mouse(int button, int state, int x, int y)
{
  int px = x - HRES;
  int py = glutGet(GLUT_WINDOW_HEIGHT) - 1 - y;
  int i = 0;

  if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN || px < 0)
    return;

  while (i < button_count &&
         !(px >= buttons[i].x && px < buttons[i].x + buttons[i].w &&
           py >= buttons[i].y && py < buttons[i].y + buttons[i].h))
    i++;

  if (i < button_count)
    do_action(buttons[i].action, current_speed());
}

void gui_init(void)
{
  panel_pixels = (GLfloat *)malloc(sizeof(GLfloat) * 3 * PANEL_W * VRES);
  layout_panel();
  create_menu();
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutMouseFunc(mouse);
}
