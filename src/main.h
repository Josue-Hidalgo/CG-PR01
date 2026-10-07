#ifndef MAIN_H
#define MAIN_H

/*
 * main.h - Tipos, constantes y enumeraciones COMPARTIDAS por todos los modulos.
 * Aqui NO se declaran funciones de otros modulos (cada .h declara las suyas).
 */

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

/* Modos de despliegue (confirmar contra el enunciado PR01-CG.pdf) */
typedef enum
{
  MODE_SIMPLE,  /* solo bordes   */
  MODE_FILL,    /* color solido  */
  MODE_TEXTURE, /* textura .avs  */
  MODE_COUNT
} DISPLAY_MODES;

/* Opciones del menu contextual */
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
