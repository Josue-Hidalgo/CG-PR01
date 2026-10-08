#include "geometry.h"
#include <string.h>

#define SCALE_MIN 0.1
#define SCALE_MAX 500.0

CAMERA camera = {0.0, 0.0, 1.0, 0.0};

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
  if (camera.scale < SCALE_MIN)
    camera.scale = SCALE_MIN;
  if (camera.scale > SCALE_MAX)
    camera.scale = SCALE_MAX;
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

static int clip_code(double x, double y)
{
  int code = CS_INSIDE;

  if (x < 0.0)
    code |= CS_LEFT;
  else if (x > HRES - 1.0)
    code |= CS_RIGHT;

  if (y < 0.0)
    code |= CS_BOTTOM;
  else if (y > VRES - 1.0)
    code |= CS_TOP;

  return code;
}

int geometry_clip_line(POINT *a, POINT *b)
{
  const double xmin = 0.0;
  const double xmax = HRES - 1.0;
  const double ymin = 0.0;
  const double ymax = VRES - 1.0;

  double x0 = a->x;
  double y0 = a->y;
  double x1 = b->x;
  double y1 = b->y;

  int code_a = clip_code(x0, y0);
  int code_b = clip_code(x1, y1);

  while (
      ((code_a | code_b) != 0) &&
      ((code_a & code_b) == 0))
  {
    int outside = code_a != 0 ? code_a : code_b;

    double dx = x1 - x0;
    double dy = y1 - y0;
    double x, y;

    if (outside & CS_TOP)
    {
      if (dy == 0.0)
        return 0;

      y = ymax;
      x = x0 + dx * (ymax - y0) / dy;
    }
    else if (outside & CS_BOTTOM)
    {
      if (dy == 0.0)
        return 0;

      y = ymin;
      x = x0 + dx * (ymin - y0) / dy;
    }
    else if (outside & CS_RIGHT)
    {
      if (dx == 0.0)
        return 0;

      x = xmax;
      y = y0 + dy * (xmax - x0) / dx;
    }
    else
    {
      if (dx == 0.0)
        return 0;

      x = xmin;
      y = y0 + dy * (xmin - x0) / dx;
    }

    if (code_a != 0)
    {
      x0 = x;
      y0 = y;
      code_a = clip_code(x0, y0);
    }
    else
    {
      x1 = x;
      y1 = y;
      code_b = clip_code(x1, y1);
    }
  }

  if ((code_a & code_b) != 0)
    return 0;

  a->x = (int)lround(x0);
  a->y = (int)lround(y0);
  b->x = (int)lround(x1);
  b->y = (int)lround(y1);
  return 1;
}

typedef enum
{
  CLIP_LEFT,
  CLIP_RIGHT,
  CLIP_BOTTOM,
  CLIP_TOP
} CLIP_EDGE;

static int is_inside(VEC2 p, CLIP_EDGE edge)
{
  switch (edge)
  {
  case CLIP_LEFT:
    return p.x >= 0.0;
  case CLIP_RIGHT:
    return p.x <= HRES - 1.0;
  case CLIP_BOTTOM:
    return p.y >= 0.0;
  default:
    return p.y <= VRES - 1.0;
  }
}

static VEC2 intersect(VEC2 s, VEC2 e, CLIP_EDGE edge)
{
  VEC2 r;
  double t;

  if (edge == CLIP_LEFT || edge == CLIP_RIGHT)
  {
    r.x = edge == CLIP_LEFT ? 0.0 : HRES - 1.0;
    t = (r.x - s.x) / (e.x - s.x);
    r.y = s.y + t * (e.y - s.y);
  }
  else
  {
    r.y = edge == CLIP_BOTTOM ? 0.0 : VRES - 1.0;
    t = (r.y - s.y) / (e.y - s.y);
    r.x = s.x + t * (e.x - s.x);
  }
  return r;
}

static VEC2 *clip_pass(const VEC2 *in, int n, int *out_n, CLIP_EDGE edge)
{
  VEC2 *out, s, e;
  int i, m = 0, s_in, e_in;

  *out_n = 0;
  if (n == 0)
    return NULL;

  out = (VEC2 *)malloc((2 * n + 2) * sizeof(VEC2));
  if (out == NULL)
    return NULL;

  s = in[n - 1];
  s_in = is_inside(s, edge);
  for (i = 0; i < n; i++)
  {
    e = in[i];
    e_in = is_inside(e, edge);
    if (e_in)
    {
      if (!s_in)
        out[m++] = intersect(s, e, edge);
      out[m++] = e;
    }
    else if (s_in)
      out[m++] = intersect(s, e, edge);
    s = e;
    s_in = e_in;
  }

  *out_n = m;
  return out;
}

int geometry_clip_polygon(const VEC2 *in, int n, VEC2 **out)
{
  VEC2 *cur, *next;
  int edge, m = n;

  *out = NULL;
  if (n <= 0)
    return 0;

  cur = (VEC2 *)malloc(n * sizeof(VEC2));
  if (cur == NULL)
    return 0;
  memcpy(cur, in, n * sizeof(VEC2));

  for (edge = CLIP_LEFT; edge <= CLIP_TOP; edge++)
  {
    next = clip_pass(cur, m, &m, (CLIP_EDGE)edge);
    free(cur);
    cur = next;
    if (cur == NULL || m == 0)
    {
      free(cur);
      return 0;
    }
  }

  *out = cur;
  return m;
}
