#ifndef DATA_H
#define DATA_H

#include "main.h"

typedef struct
{
  double x;
  double y;
} VEC2;

typedef struct
{
  PROVINCES province;
  VEC2 *vertices;
  int count;
} POLYGON;

typedef struct
{
  POLYGON *polygons;
  int count;
} MAP;

extern MAP map;

extern COLOR **texture;
extern int texture_w;
extern int texture_h;

int data_load_map(const char *path);
void data_free_map(void);

int data_load_textures(void);
void data_select_texture(PROVINCES province);
void data_free_texture(void);
int data_save_buffer(const char *filename);

#endif /* DATA_H */
