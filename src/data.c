#include "data.h"
#include <string.h>
#include <ctype.h>

#define MAP_MARGIN 0.03

MAP map = {NULL, 0};
static int map_capacity = 0;

COLOR **texture = NULL;
int texture_w = 0;
int texture_h = 0;

static const char *province_ids[PROVINCE_COUNT] = {
    "CRSJ", "CRA", "CRC", "CRH", "CRG", "CRP", "CRL"};

static char *read_file(const char *path)
{
  FILE *f;
  long size;
  char *text;

  f = fopen(path, "rb");
  if (f == NULL)
    return NULL;

  if (fseek(f, 0, SEEK_END) != 0 || (size = ftell(f)) < 0 || fseek(f, 0, SEEK_SET) != 0)
  {
    fclose(f);
    return NULL;
  }

  text = (char *)malloc(size + 1);
  if (text != NULL && fread(text, 1, size, f) != (size_t)size)
  {
    free(text);
    text = NULL;
  }
  if (text != NULL)
    text[size] = '\0';

  fclose(f);
  return text;
}

static char *find_object_end(char *s)
{
  int depth = 0;
  int in_string = 0;

  do
  {
    if (*s == '\0')
      return NULL;

    if (in_string)
    {
      if (*s == '\\' && s[1] != '\0')
        s++;
      else if (*s == '"')
        in_string = 0;
    }
    else if (*s == '"')
      in_string = 1;
    else if (*s == '{')
      depth++;
    else if (*s == '}')
      depth--;

    s++;
  } while (depth > 0);

  return s - 1;
}

static int read_province(const char *feature, PROVINCES *province)
{
  const char *s = strstr(feature, "\"id\"");
  size_t len;
  int i;

  if (s == NULL || (s = strchr(s + 4, '"')) == NULL)
    return -1;

  s++;
  len = strcspn(s, "\"");

  for (i = 0; i < PROVINCE_COUNT; i++)
    if (strlen(province_ids[i]) == len && strncmp(s, province_ids[i], len) == 0)
    {
      *province = (PROVINCES)i;
      return 0;
    }

  return -1;
}

static int add_polygon(PROVINCES province, VEC2 *vertices, int count)
{
  POLYGON *tmp;

  if (map.count == map_capacity)
  {
    map_capacity = map_capacity == 0 ? 16 : map_capacity * 2;
    tmp = (POLYGON *)realloc(map.polygons, map_capacity * sizeof(POLYGON));
    if (tmp == NULL)
      return -1;
    map.polygons = tmp;
  }

  map.polygons[map.count].province = province;
  map.polygons[map.count].vertices = vertices;
  map.polygons[map.count].count = count;
  map.count++;
  return 0;
}

static int finish_ring(PROVINCES province, VEC2 *v, int count)
{
  if (count > 1 && v[0].x == v[count - 1].x && v[0].y == v[count - 1].y)
    count--;

  if (count < 3)
  {
    free(v);
    return 0;
  }

  if (add_polygon(province, v, count) != 0)
  {
    free(v);
    return -1;
  }
  return 0;
}

static int parse_coordinates(const char *s, int ring_depth, PROVINCES province)
{
  VEC2 *v = NULL, *tmp;
  int count = 0, capacity = 0;
  int depth = 0;
  int ring = 0;
  int ok = 1;
  char *end;
  double lon, lat;

  do
  {
    if (*s == '\0')
      ok = 0;
    else if (*s == '[')
    {
      depth++;
      if (depth == ring_depth - 1)
        ring = 0;
      else if (depth == ring_depth)
        count = 0;
      else if (depth == ring_depth + 1 && ring == 0)
      {
        lon = strtod(s + 1, &end);
        while (isspace((unsigned char)*end) || *end == ',')
          end++;
        lat = strtod(end, &end);
        while (isspace((unsigned char)*end))
          end++;
        if (*end != ']')
          ok = 0;

        if (ok && count == capacity)
        {
          capacity = capacity == 0 ? 256 : capacity * 2;
          tmp = (VEC2 *)realloc(v, capacity * sizeof(VEC2));
          if (tmp == NULL)
            ok = 0;
          else
            v = tmp;
        }
        if (ok)
        {
          v[count].x = lon;
          v[count].y = lat;
          count++;
          s = end - 1;
        }
      }
    }
    else if (*s == ']')
    {
      if (depth == ring_depth && ring == 0)
      {
        ok = finish_ring(province, v, count) == 0;
        v = NULL;
        count = capacity = 0;
      }
      if (depth == ring_depth)
        ring++;
      depth--;
    }
    s++;
  } while (ok && depth > 0);

  free(v);
  return ok ? 0 : -1;
}

static int parse_feature(const char *feature)
{
  PROVINCES province;
  const char *coords;
  int ring_depth;

  if (read_province(feature, &province) != 0)
    return -1;

  coords = strstr(feature, "\"coordinates\"");
  if (coords == NULL || (coords = strchr(coords, '[')) == NULL)
    return -1;

  ring_depth = strstr(feature, "\"MultiPolygon\"") != NULL ? 3 : 2;
  return parse_coordinates(coords, ring_depth, province);
}

static void normalize_map(void)
{
  double xmin, xmax, ymin, ymax, w, h, cx, cy;
  double aspect = (double)HRES / VRES;
  VEC2 *v;
  int i, j;

  xmin = xmax = map.polygons[0].vertices[0].x;
  ymin = ymax = map.polygons[0].vertices[0].y;

  for (i = 0; i < map.count; i++)
    for (j = 0; j < map.polygons[i].count; j++)
    {
      v = &map.polygons[i].vertices[j];
      xmin = fmin(xmin, v->x);
      xmax = fmax(xmax, v->x);
      ymin = fmin(ymin, v->y);
      ymax = fmax(ymax, v->y);
    }

  cx = (xmin + xmax) / 2.0;
  cy = (ymin + ymax) / 2.0;
  w = (xmax - xmin) * (1.0 + 2 * MAP_MARGIN);
  h = (ymax - ymin) * (1.0 + 2 * MAP_MARGIN);

  if (w / h < aspect)
    w = h * aspect;
  else
    h = w / aspect;

  xmin = cx - w / 2.0;
  ymin = cy - h / 2.0;

  for (i = 0; i < map.count; i++)
    for (j = 0; j < map.polygons[i].count; j++)
    {
      v = &map.polygons[i].vertices[j];
      v->x = (v->x - xmin) * HRES / w;
      v->y = (v->y - ymin) * VRES / h;
    }
}

int data_load_map(const char *path)
{
  char *text, *s, *end, saved;
  int ok = 1;

  data_free_map();

  text = read_file(path);
  if (text == NULL)
    return -1;

  s = strstr(text, "\"features\"");
  if (s == NULL || (s = strchr(s, '[')) == NULL)
    ok = 0;
  else
    s++;

  while (ok && *s != ']')
  {
    if (*s == '\0')
      ok = 0;
    else if (*s == '{')
    {
      end = find_object_end(s);
      if (end == NULL)
        ok = 0;
      else
      {
        saved = end[1];
        end[1] = '\0';
        ok = parse_feature(s) == 0;
        end[1] = saved;
        s = end + 1;
      }
    }
    else
      s++;
  }

  free(text);

  if (!ok || map.count == 0)
  {
    data_free_map();
    return -1;
  }

  normalize_map();
  return 0;
}

void data_free_map(void)
{
  int i;
  for (i = 0; i < map.count; i++)
    free(map.polygons[i].vertices);
  free(map.polygons);
  map.polygons = NULL;
  map.count = 0;
  map_capacity = 0;
}

#define SWAP(x) ((((x) << 24) & 0xff000000) | \
                 (((x) << 8) & 0x00ff0000) |  \
                 (((x) >> 8) & 0x0000ff00) |  \
                 (((x) >> 24) & 0x000000ff))
#define FIX(x) ((x) = SWAP((x)))

#define AVS_MAX_SIDE 8192

typedef struct
{
  COLOR **pixels;
  int w;
  int h;
} TEXTURE;

static TEXTURE textures[PROVINCE_COUNT];

static const char *texture_paths[PROVINCE_COUNT] = {
    "assets/sanjose.avs", "assets/alajuela.avs", "assets/cartago.avs",
    "assets/heredia.avs", "assets/guanacaste.avs", "assets/puntarenas.avs",
    "assets/limon.avs"};

static void free_pixels(COLOR **pixels, int w)
{
  int i;
  if (pixels == NULL)
    return;
  for (i = 0; i < w; i++)
    free(pixels[i]);
  free(pixels);
}

static int read_avs(const char *path, TEXTURE *t)
{
  FILE *f;
  unsigned int uw = 0, uh = 0;
  int w, h, i, j, ok = 1;
  int a, r, g, b;
  COLOR **pixels = NULL;

  f = fopen(path, "rb");
  if (f == NULL)
    return -1;

  if (fread(&uw, sizeof(uw), 1, f) != 1 || fread(&uh, sizeof(uh), 1, f) != 1)
    ok = 0;
  FIX(uw);
  FIX(uh);
  w = uw > AVS_MAX_SIDE ? 0 : (int)uw;
  h = uh > AVS_MAX_SIDE ? 0 : (int)uh;
  if (w == 0 || h == 0)
    ok = 0;

  if (ok)
  {
    pixels = (COLOR **)calloc(w, sizeof(COLOR *));
    ok = pixels != NULL;
  }
  for (i = 0; ok && i < w; i++)
  {
    pixels[i] = (COLOR *)malloc(h * sizeof(COLOR));
    ok = pixels[i] != NULL;
  }

  for (j = 0; ok && j < h; j++)
    for (i = 0; ok && i < w; i++)
    {
      a = fgetc(f);
      r = fgetc(f);
      g = fgetc(f);
      b = fgetc(f);
      if (a == EOF || r == EOF || g == EOF || b == EOF)
        ok = 0;
      else
      {
        pixels[i][h - 1 - j].r = r / 255.0;
        pixels[i][h - 1 - j].g = g / 255.0;
        pixels[i][h - 1 - j].b = b / 255.0;
      }
    }

  fclose(f);

  if (!ok)
  {
    free_pixels(pixels, w);
    return -1;
  }

  t->pixels = pixels;
  t->w = w;
  t->h = h;
  return 0;
}

int data_load_textures(void)
{
  int i, result = 0;

  data_free_texture();
  for (i = 0; i < PROVINCE_COUNT; i++)
    if (read_avs(texture_paths[i], &textures[i]) != 0)
      result = -1;

  return result;
}

void data_select_texture(PROVINCES province)
{
  texture = textures[province].pixels;
  texture_w = textures[province].w;
  texture_h = textures[province].h;
}

void data_free_texture(void)
{
  int i;
  for (i = 0; i < PROVINCE_COUNT; i++)
  {
    free_pixels(textures[i].pixels, textures[i].w);
    textures[i].pixels = NULL;
    textures[i].w = textures[i].h = 0;
  }
  texture = NULL;
  texture_w = texture_h = 0;
}

int data_save_buffer(const char *filename)
{
  (void)filename;
  return 0;
}
