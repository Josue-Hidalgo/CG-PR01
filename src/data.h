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
// apunta a la textura de la provincia escogida con data_select_texture()
extern COLOR **texture;
extern int texture_w;
extern int texture_h;

int data_load_map(const char *path);
void data_free_map(void);

// carga los 7 .avs (uno por provincia), -1 si falto alguno
int data_load_textures(void);
void data_select_texture(PROVINCES province);
void data_free_texture(void);
int data_save_buffer(const char *filename); /* antes: guardarImgBuffer() */

#endif /* DATA_H */
