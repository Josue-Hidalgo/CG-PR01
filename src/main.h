#ifndef MAIN_H
#define MAIN_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

#define VRES 768
#define HRES 768

#define PANEL_W 200

typedef struct
{
  double r;
  double g;
  double b;
} COLOR;

typedef struct
{
  int x;
  int y;
} POINT;

typedef struct
{
  POINT A;
  POINT B;
} LINE;

typedef enum
{
  SANJOSE,
  ALAJUELA,
  CARTAGO,
  HEREDIA,
  GUANACASTE,
  PUNTARENAS,
  LIMON,
  PROVINCE_COUNT
} PROVINCES;

typedef enum
{
  MODE_SIMPLE,
  MODE_FILL,
  MODE_TEXTURE,
  MODE_COUNT
} DISPLAY_MODES;

typedef enum
{
  DISPLAY_MODE,
  ZOOM_IN,
  ZOOM_OUT,
  ROTATION,
  RESTART,
  EXIT
} MENU;

#endif /* MAIN_H */
