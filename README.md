[README.md](https://github.com/user-attachments/files/32723712/README.md)
# CG-PR01# Proyecto 1: Manejo de Polígonos en 2D — Mapa de Costa Rica

**Curso:** Computer Graphics — Escuela de Computación, ITCR

**Integrantes:** 

José Emilio Alvarado Méndez - 2022163260

Josué Santiago Hidalgo Sandoval - 2024800128

Ian Jafeth Lopez Zamora - 2024222219

**Fecha de entrega:** Jueves 8 de Octubre



## 1. Descripción
 
Despliegue del mapa de Costa Rica con su división en 7 provincias, cada una representada por uno o más polígonos en coordenadas universales. Todos los algoritmos gráficos están escritos por el grupo; la biblioteca (MESA/GLUT) se usa únicamente para crear la ventana, encender píxeles y leer teclado/mouse.
 
**Estado actual:** base de dibujo de líneas (Bresenham) y lectura de archivos `.avs` listas. Operaciones del mapa, polígonos, clipping, relleno y datos: pendientes (ver sección 8).
 
## 2. Requisitos y compilación
 
- Sistema operativo: Linux (ejecución nativa, sin máquina virtual)
- Lenguaje: C
- Bibliotecas: MESA / GLUT (`GL/gl.h`, `GL/glu.h`, `GL/glut.h`), `math.h`
- Dependencias: `<completar, ej. paquetes freeglut / mesa>`
**Compilar** (verificar que coincida con lo que usen):
 
```bash
gcc main.c -o main -lGL -lGLU -lglut -lm
```
 
**Ejecutar** (versión actual, heredada de la Tarea 0):
 
```bash
./main <resolucion> <# lineas> <# veces>
# Ejemplo: ./main 256 100 500
```
 
> Los argumentos `<# lineas>` y `<# veces>` son de la Tarea 0 y se eliminarán/ajustarán cuando el `main` pase a dibujar el mapa.
> Idea: usar args <flagConvertir> <direccionImg>
 
## 3. Estructura del proyecto
 
```
├── main.c      # programa principal, algoritmos y manejo de archivos avs
├── main.h      # includes, tipos y declaraciones de funciones
└── README.md
```
 
| Archivo | Contenido |
|---|---|
| `main.h` | Includes, tipos `COLOR`, `POINT`, `LINE`, `ALGORITHM` y prototipos |
| `main.c` | `main`, `draw_scene`, `plot_nothing`/`plot_something`, `color_line`, `line_bresenham`, `draw_bresenham`, funciones de PRO1 (stubs) y lectura/escritura `.avs` |
 
## 4. Tipos y estructuras de datos
 
- `COLOR`: componentes `r`, `g`, `b` en `double` (0.0 a 1.0).
- `POINT`: coordenadas enteras `x`, `y`.
- `LINE`: par de puntos `A`, `B`.
- `ALGORITHM`: enum de algoritmos de línea heredado de la Tarea 0.
- Framebuffer: `COLOR **buffer` de `RES x RES`, inicializado en negro.
- Textura: `COLOR **texBuffer` de `height x width`, cargada desde un `.avs` (separado del framebuffer).
## 5. Los datos
 
- Fuente de los puntos de las provincias: <completar>
- Formato del archivo de datos: <completar>
- Sistema de coordenadas universales: <completar>
- Cantidad aproximada de vértices: <completar>
- Isla del Coco: <incluida / no incluida>
- Ventana inicial (universal): <xmin, ymin, xmax, ymax>
- Resolución del framebuffer: <ancho x alto> (misma proporción que la ventana)
## 6. Modos de despliegue
 
| Modo | Función | Estado |
|---|---|---|
| Sin colorear (solo bordes, modo inicial) | `SimplePolygon` | Pendiente |
| Coloreado (un color por provincia) | `PaintPolygon` | Pendiente |
| Texturas (`.avs` por provincia) | `TexturePolygon` | Pendiente |
 
Tecla de cambio de modo: <completar>
 
## 7. Controles
 
Cada operación tendrá tres velocidades: normal, lenta y rápida.
 
| Operación | Función | Normal | Lento | Rápido | Estado |
|---|---|---|---|---|---|
| Zoom in / out | `Zoom` | <tecla> | <tecla> | <tecla> | Pendiente |
| Pan (izq/der/arriba/abajo) | `Pan` | <tecla> | <tecla> | <tecla> | Pendiente |
| Rotación (horario/antihorario, en radianes) | `Rotate` | <tecla> | <tecla> | <tecla> | Pendiente |
| Reiniciar vista | `Reset` | <tecla> | — | — | Pendiente |
| Terminar | `Finish` | <tecla> | — | — | Pendiente |
 
Lectura de teclado: `startKeyboardReading` / `keyPressed` (pendientes).
 
## 8. Algoritmos y estado de implementación
 
| Componente | Función | Estado |
|---|---|---|
| Línea de Bresenham (punto medio, 8 octantes, solo enteros) | `line_bresenham` | Implementado |
| Pintado de píxel en framebuffer y pantalla | `plot_something` (con `plot_nothing` para medir tiempos) | Implementado |
| Color por algoritmo | `color_line` | Implementado |
| Refresco de la ventana | `draw_scene` | Implementado |
| Lectura de `.avs` a `texBuffer` | `cargarImgBuffer` | Implementado |
| Escritura de `texBuffer` a `.avs` | `guardarImgBuffer` | Implementado |
| Dibujo de polígono por aristas | `DrawPolygon` | Pendiente |
| Relleno con color sólido | `PaintPolygon` | Pendiente |
| Relleno con textura | `TexturePolygon` | Pendiente |
| Zoom / Pan / Rotación | `Zoom`, `Pan`, `Rotate` | Pendiente |
| Clipping de líneas y polígonos | — | Pendiente |
 
Notas de uso de las funciones `.avs`: `cargarImgBuffer` lee desde el archivo global `fptr`, que debe abrirse antes con `fopen(ruta, "rb")`. Las dimensiones quedan en las globales `width` y `height`.
 
## 9. Decisiones de diseño y limitaciones
 
- `plot` es un puntero a función que permite alternar entre `plot_nothing` y `plot_something`.
- `plot_something` descarta píxeles fuera del framebuffer para evitar accesos inválidos.
- <completar decisiones adicionales>
## 10. Checklist previo a la revisión
 
- [ ] Todo el código está en C (no C++)
- [ ] Compila y corre en Linux nativo (no VM, no disco externo)
- [ ] Sin prints de depuración (ej. `Width:`/`Height:` en las funciones `.avs`)
- [ ] Argumentos heredados de la Tarea 0 (`<# lineas>`, `<# veces>`) eliminados o ajustados
- [ ] Sin Segmentation Fault en ninguna circunstancia (zoom extremo, pan fuera del mapa, rotaciones, cambios de modo)
- [ ] Todos los módulos integrados
- [ ] Directorio nombrado con los dos apellidos, empaquetado con `tar` en `.tgz`
- [ ] Enviado a `torresrojas.cursos.05@gmail.com` antes de la hora de inicio, con el subject `[CG] Proyecto 1 - <Fulano>-<Mengano>-etc`
## 11. Créditos
 
- Lectura de `.avs` basada en `xtoraw.c` de Paul Bourke (2001) y en el código de la Tarea 0.
- Fuente de datos geográficos: <completar>
