#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "main.h"
#include "data.h"

typedef struct
{
  double m[3][3];
} MAT3;

typedef struct
{
  double tx;
  double ty;
  double scale;
  double angle;
} CAMERA;

extern CAMERA camera;

void geometry_reset(void);
void geometry_zoom(double factor);
void geometry_pan(double dx, double dy);
void geometry_rotate(double rad);

MAT3 mat3_identity(void);
MAT3 mat3_multiply(MAT3 a, MAT3 b);
MAT3 mat3_translate(double tx, double ty);
MAT3 mat3_scale(double sx, double sy);
MAT3 mat3_rotate(double rad);

MAT3 geometry_matrix(void);
VEC2 geometry_apply(MAT3 m, VEC2 p);

int geometry_clip_line(POINT *a, POINT *b);

enum
{
  CS_INSIDE = 0,
  CS_LEFT = 1,
  CS_RIGHT = 2,
  CS_BOTTOM = 4,
  CS_TOP = 8
};

#endif /* GEOMETRY_H */
