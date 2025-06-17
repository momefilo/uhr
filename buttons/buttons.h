#ifndef my_buttons_h
#define my_buttons_h 1

#include "pico/stdlib.h"

#define BUTTON_U 21
#define BUTTON_D 20
#define BUTTON_L 19
#define BUTTON_R 18
#define BUTTON_M 16

/* Initialisiert gpio */
void buttons_init();

/* Gibt den aktuell gedrueckten Button zurück oder den Wert 100 */
uint8_t get_Button();

#endif
