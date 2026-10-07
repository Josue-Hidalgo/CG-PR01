#ifndef GUI_H
#define GUI_H

/*
 * GUI.h - [Persona 3: Datos y GUI]
 * Menu, teclado y (a futuro) panel fijo de botones a la derecha.
 */

#include "main.h"

void gui_init(void); /* registra menu y callbacks de teclado (llamar despues de crear la ventana) */
void gui_quit(void); /* libera todo y termina el programa */

// dibuja el panel a la derecha del mapa (llamar en draw_scene)
void gui_draw_panel(void);

#endif /* GUI_H */
