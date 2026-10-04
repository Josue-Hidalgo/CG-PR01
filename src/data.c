#include "data.h"

MAP map = {NULL, 0};

COLOR **texture = NULL;
int texture_w = 0;
int texture_h = 0;

/*
 * data_load_map(path):
 * TODO (Persona 3):
 *  1. Inspeccionar el formato de assets/cr.json.
 *  2. Leer los poligonos y llenar `map` (reservar polygons[] y vertices[]).
 *  3. Normalizar las coordenadas del mapa al area de la ventana (0..HRES, 0..VRES),
 *     de modo que geometry/render trabajen directamente en "pixeles de mundo".
 *  4. Asignar a cada POLYGON su PROVINCES.
 *
 * Por ahora es un stub: devuelve exito con 0 poligonos, asi el programa
 * arranca y render dibuja la linea de prueba.
 */
int data_load_map(const char *path)
{
  (void)path;
  map.polygons = NULL;
  map.count = 0;
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
}

/* TODO (Persona 3): leer el .avs y llenar texture / texture_w / texture_h. */
int data_load_texture(const char *path)
{
  (void)path;
  return 0;
}

void data_free_texture(void)
{
  int i;
  if (texture != NULL)
  {
    for (i = 0; i < texture_w; i++)
      free(texture[i]);
    free(texture);
  }
  texture = NULL;
  texture_w = texture_h = 0;
}

/* TODO (Persona 3): guardar el framebuffer en un archivo. */
int data_save_buffer(const char *filename)
{
  (void)filename;
  return 0;
}
