/* mipslab.h

   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Modified by I Einberg 2021

   For copyright and licensing, see file COPYING */

/* Declare display-related functions from display.c */

#include <stdint.h>
#include "screenstate.h"
#include "object.h"


void add_object_to_screen(object * obj, screenstate * state);

void draw_entire_display(screenstate *state);

void display_init(void);
void display_isr();

void display_update(void);
void move_background(screenstate  * state, uint8_t amount);

uint8_t display_send_byte(uint8_t data);
void add_textbuffer_to_screen(screenstate * state);
void put_string(int line, char * s);
char * itoaconv(int num);

/* Declare bitmap array containing font */
extern const uint8_t const font[128 * 8];
/* Declare bitmap array containing icon */
extern const uint8_t const icon[128];
/* Declare text buffer for display output */
extern char textbuffer[4][16];

