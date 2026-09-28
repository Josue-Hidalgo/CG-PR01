#ifndef MAIN_H
#define MAIN_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

typedef struct
{
  double r;
  double g;
  double b;
} COLOR;

typedef struct {
  int x;
  int y;
} POINT;

typedef struct {
  POINT A;
  POINT B;
} LINE;

typedef enum {
  BRUTEFORCE,
  INCREMENTAL,
  INCREMENTAL_V2,
  BRESENHAM,
  BRESENHAM_ASSEMBLY
} ALGORITHM;

void Zoom(void);//Operacion zoom usar matriz dada por torres, entrada seria el delta de crecimiento o decrecimiento, arbitrario o configurable
void Pan(void);//Operacion Pan (mover) dada por torres, misma idea que con zoom, valor (x,y) para mover la ventana
void Rotate(void);//Operacion de rotar el poligono (Usar Rad)
void Reset(void);//Restaura la camara al punto (0,0) puede ser arbitrario a otro punto en realidad, decidir luego
void Finish(void);// Termina el programa
void startKeyboardReading(void);//Inicia thread para la lectura de las teclas presionadas por el usuario ¿EventListener?
void keyPressed(void);//Switch? con las distintas opciones dependiendo la tecla
void DrawPolygon(void);//Dibuja el polygono dado un point** haciendo n llamados a bresenham, (Tomar en cuenta dibuja posicion [n,n] a [0,0] al final o inicio de la ejecucion
void PaintPolygon(void);// Pinta el poligono con un color RGB solido, revisar algoritmo de torres
void TexturePolygon(void);//Pinta el poligono a partir de una textura en un archivo avs. (Ya esta implementado practicamente, solo es unificar el frameBuffer con el buffer dado por el metodo cargarImgBuffer()
void SimplePolygon(void);//Solo bordes

void cargarImgBuffer(void);
void guardarImgBuffer(char *filename);

#endif // MAIN_H