#include "geometry.h"

CAMERA camera = {0.0, 0.0, 1.0, 0.0};

/* ---------- Matrices ---------- */

MAT3 mat3_identity(void)
{
  MAT3 r = {{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};
  return r;
}

MAT3 mat3_multiply(MAT3 a, MAT3 b)
{
  MAT3 r;
  int i, j, k;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
    {
      r.m[i][j] = 0.0;
      for (k = 0; k < 3; k++)
        r.m[i][j] += a.m[i][k] * b.m[k][j];
    }
  return r;
}

MAT3 mat3_translate(double tx, double ty)
{
  MAT3 r = mat3_identity();
  r.m[0][2] = tx;
  r.m[1][2] = ty;
  return r;
}

MAT3 mat3_scale(double sx, double sy)
{
  MAT3 r = mat3_identity();
  r.m[0][0] = sx;
  r.m[1][1] = sy;
  return r;
}

MAT3 mat3_rotate(double rad)
{
  MAT3 r = mat3_identity();
  r.m[0][0] = cos(rad);
  r.m[0][1] = -sin(rad);
  r.m[1][0] = sin(rad);
  r.m[1][1] = cos(rad);
  return r;
}

/* ---------- Camara ---------- */

void geometry_reset(void)
{
  camera.tx = 0.0;
  camera.ty = 0.0;
  camera.scale = 1.0;
  camera.angle = 0.0;
}

void geometry_zoom(double factor)
{
  camera.scale *= factor;
}

void geometry_pan(double dx, double dy)
{
  camera.tx += dx;
  camera.ty += dy;
}

void geometry_rotate(double rad)
{
  camera.angle += rad;
}

/*
 * M = T(centro + pan) * R * S * T(-centro)
 * Zoom y rotacion se hacen alrededor del centro del area del mapa.
 */
MAT3 geometry_matrix(void)
{
  double cx = HRES / 2.0;
  double cy = VRES / 2.0;
  MAT3 m = mat3_translate(-cx, -cy);

  m = mat3_multiply(mat3_scale(camera.scale, camera.scale), m);
  m = mat3_multiply(mat3_rotate(camera.angle), m);
  m = mat3_multiply(mat3_translate(cx + camera.tx, cy + camera.ty), m);
  return m;
}

VEC2 geometry_apply(MAT3 m, VEC2 p)
{
  VEC2 r;
  r.x = m.m[0][0] * p.x + m.m[0][1] * p.y + m.m[0][2];
  r.y = m.m[1][0] * p.x + m.m[1][1] * p.y + m.m[1][2];
  return r;
}

/* ---------- Clipping ---------- */

/*
 * TODO (Persona 2): implementar (por ejemplo Cohen-Sutherland).
 * Mientras tanto devuelve 1 sin recortar; plot() ya descarta pixeles
 * fuera del framebuffer, asi que no hay accesos invalidos.
 */
int geometry_clip_line(POINT *a, POINT *b)
{
  (void)a;
  (void)b;
  return 1;
}
