[README.md](https://github.com/user-attachments/files/32723712/README.md)
# CG-PR01# Proyecto 1: Manejo de Polígonos en 2D — Mapa de Costa Rica

**Curso:** Computer Graphics — Escuela de Computación, ITCR
**Integrantes:** José Emilio Alvarado Méndez - 2022163260
Josué Santiago Hidalgo Sandoval - 2024800128
Ian Jafeth Lopez Zamora - 2024222219
**Fecha de entrega:** Jueves 8 de Octubre



---

## 1. Descripción

<Resumen breve, con sus palabras: qué hace el programa y qué se muestra.>

## 2. Requisitos y compilación

- Sistema operativo: Linux (ejecución nativa, sin máquina virtual)
- Lenguaje: C
- Bibliotecas usadas (solo para ventana, píxeles e interacción): <MESA / GLUT / otra>
- Dependencias a instalar: `<comando, ej. sudo apt install ...>`

**Compilar:**

```bash
<comando de compilación o make>
```

**Ejecutar:**

```bash
<comando de ejecución>
```

## 3. Estructura del proyecto

```
<apellido1-apellido2>/
├── <archivos .c / .h>
├── <Makefile>
├── datos/        # <puntos de provincias>
├── texturas/     # <archivos .avs por provincia>
└── README.md
```

| Archivo | Responsabilidad |
|---|---|
| `<archivo>` | <qué contiene> |

## 4. Los datos

- Fuente de los puntos de las provincias: <origen>
- Formato del archivo de datos: <describir>
- Sistema de coordenadas universales usado: <describir>
- Cantidad aproximada de vértices: <n>
- Isla del Coco: <incluida / no incluida>
- Ventana inicial (coordenadas universales): <xmin, ymin, xmax, ymax>
- Resolución del framebuffer: <ancho x alto> (misma proporción que la ventana)

## 5. Modos de despliegue

| Modo | Descripción | Tecla |
|---|---|---|
| Sin colorear | Solo bordes (Bresenham). Modo inicial. | `<tecla>` |
| Coloreado | Un color distinto por provincia. | `<tecla>` |
| Texturas | Texeles de un `.avs` distinto por provincia. | `<tecla>` |

## 6. Controles

Cada operación tiene tres velocidades: normal, lenta y rápida.

| Operación | Normal | Lento | Rápido |
|---|---|---|---|
| Zoom in | `<tecla>` | `<tecla>` | `<tecla>` |
| Zoom out | `<tecla>` | `<tecla>` | `<tecla>` |
| Pan izquierda | `<tecla>` | `<tecla>` | `<tecla>` |
| Pan derecha | `<tecla>` | `<tecla>` | `<tecla>` |
| Pan arriba | `<tecla>` | `<tecla>` | `<tecla>` |
| Pan abajo | `<tecla>` | `<tecla>` | `<tecla>` |
| Rotar horario | `<tecla>` | `<tecla>` | `<tecla>` |
| Rotar antihorario | `<tecla>` | `<tecla>` | `<tecla>` |
| Reiniciar vista | `<tecla>` | — | — |
| Terminar | `<tecla>` | — | — |

Tasas usadas: <ej. zoom normal = x%, lento = x%, rápido = x%>

## 7. Algoritmos implementados

Todos desarrollados por el grupo.

- **Líneas:** <Bresenham — archivo/función>
- **Clipping de líneas:** <algoritmo — archivo/función>
- **Clipping de polígonos:** <algoritmo — archivo/función>
- **Relleno de polígonos:** <algoritmo — archivo/función>
- **Mapeo de texturas:** <mapeo simple a texeles — archivo/función>
- **Transformaciones (zoom, pan, rotación):** <cómo se aplican a la ventana/vértices>
- **Lectura de archivos `.avs`:** <archivo/función>

## 8. Decisiones de diseño y limitaciones

- <Decisión 1>
- <Limitación conocida 1>

## 9. Checklist previo a la revisión

- [ ] Todo el código está en C (no C++)
- [ ] Compila y corre en Linux nativo (no VM, no disco externo)
- [ ] Sin `printf` de depuración
- [ ] Sin Segmentation Fault en ninguna circunstancia (zoom extremo, pan fuera del mapa, rotaciones, cambios de modo)
- [ ] Todos los módulos están integrados
- [ ] Directorio nombrado con los dos apellidos, empaquetado con `tar` en `.tgz`
- [ ] Enviado a `torresrojas.cursos.05@gmail.com` antes de la hora de inicio, con el subject `[CG] Proyecto 1 - <Fulano>-<Mengano>-etc`

## 10. Créditos

- Fuente de datos geográficos: <fuente>
- Referencias: <notas de clase, documentación de la biblioteca, etc.>
