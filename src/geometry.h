#ifndef GEOMETRY_H
#define GEOMETRY_H

/*
 * geometry.h - [Persona 2: Geometria]
 * Zoom, desplazamiento (pan), rotacion y clipping.
 * Las transformaciones afectan SOLO al mapa (nunca al panel de la GUI).
 */

#include "main.h"
#include "data.h"

/* Matriz homogenea 3x3 */
typedef struct
{
  double m[3][3];
} MAT3;

/* Estado de la "camara" que se aplica a todo el mapa */
typedef struct
{
  double tx;    /* desplazamiento en x (pixeles) */
  double ty;    /* desplazamiento en y (pixeles) */
  double scale; /* factor de zoom, 1.0 = sin zoom */
  double angle; /* rotacion en radianes */
} CAMERA;

extern CAMERA camera;

/* Operaciones sobre la camara */
void geometry_reset(void);
void geometry_zoom(double factor); /* scale *= factor */
void geometry_pan(double dx, double dy);
void geometry_rotate(double rad);

/* Matrices (base para usar la matriz dada en clase) */
MAT3 mat3_identity(void);
MAT3 mat3_multiply(MAT3 a, MAT3 b);
MAT3 mat3_translate(double tx, double ty);
MAT3 mat3_scale(double sx, double sy);
MAT3 mat3_rotate(double rad);

/* Matriz total de la camara y aplicacion a un vertice */
MAT3 geometry_matrix(void);
VEC2 geometry_apply(MAT3 m, VEC2 p);

/*
 * Clipping de una linea contra la ventana [0,HRES-1] x [0,VRES-1].
 * Devuelve 1 si queda algo visible (y modifica los extremos), 0 si se descarta.
 */
int geometry_clip_line(POINT *a, POINT *b);

#endif /* GEOMETRY_H */
