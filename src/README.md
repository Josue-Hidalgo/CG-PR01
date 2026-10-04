# CG-PR01 - Mapa de Costa Rica (base del proyecto)

Compilar y ejecutar (desde la raiz del proyecto):

    make run

Requiere: gcc, nasm, freeglut3-dev, libxmu-dev. Copiar `Bresenham.s` a `src/`
y los archivos `cr.json` y `Ejemplo.avs` a `assets/`.

## Reparto de archivos (para evitar conflictos en git)

| Persona | Archivos | Responsabilidad |
|---|---|---|
| 1. Despliegue | `render.c/.h`, `Bresenham.s` | Bresenham, bordes, relleno, textura |
| 2. Geometria | `geometry.c/.h` | Zoom, pan, rotacion, clipping |
| 3. Datos y GUI | `data.c/.h`, `GUI.c/.h` | Cargar mapa/textura, menu, panel |
| Compartido (avisar antes de tocar) | `main.c/.h`, `makefile` | Tipos comunes e integracion |

## Fases sugeridas
0. Base compila; linea de prueba y menu funcionan (hecho en esta base).
1. Cargar cr.json y dibujar bordes (Persona 3 -> Persona 1).
2. Transformaciones + clipping (Persona 2).
3. Relleno de color y textura (Persona 1).
4. Panel fijo de botones a la derecha (Persona 3).

## Contratos entre modulos
- `data.h`: `MAP map` con `POLYGON`s en coordenadas de mundo (normalizadas a 0..HRES, 0..VRES).
- `geometry.h`: `geometry_matrix()` / `geometry_apply()`; el estado vive en `CAMERA camera`.
- `render.h`: `polygon_to_screen()` entrega los vertices ya transformados; usarlo en bordes, relleno y textura.
- `plot(x, y)` solo escribe en el framebuffer; `draw_scene()` lo muestra una vez por cuadro.
- Teclas: `+ -` zoom, `r/R` rotar, flechas pan, `m` modo, `0` reiniciar, ESC salir.
