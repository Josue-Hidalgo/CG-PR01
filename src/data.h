#ifndef DATA_H
#define DATA_H

/*
 * data.h - [Persona 3: Datos y GUI]
 * Carga del mapa (assets/cr.json) y de la textura (assets/Ejemplo.avs).
 * Convencion de retorno: 0 = exito, -1 = error.
 */

#include "main.h"

/* Vertice en coordenadas de "mundo" (double para que las transformaciones no pierdan precision). */
typedef struct
{
  double x;
  double y;
} VEC2;

/* Un poligono (una provincia, o una parte de ella). */
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

/* Mapa cargado (definido en data.c) */
extern MAP map;

/* Textura cargada (definida en data.c) */
extern COLOR **texture;
extern int texture_w;
extern int texture_h;

int data_load_map(const char *path);
void data_free_map(void);

int data_load_texture(const char *path); /* antes: cargarImgBuffer() */
void data_free_texture(void);
int data_save_buffer(const char *filename); /* antes: guardarImgBuffer() */

#endif /* DATA_H */
