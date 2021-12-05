/* mipslab.h

   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Modified by I Einberg 2021

   For copyright and licensing, see file COPYING */

/* Declare display-related functions from display.c */

#include <stdint.h>
#include "object.h"

typedef struct screenstate {
    uint32_t current_scroll_amount; // Screen x-position relative to entire source image
    uint32_t entire_image_width; // Width of source image
    uint8_t current_image[512]; // The part of the entire image that is currently displayed
    uint8_t entire_image[]; // Entire source image as flexible array member
} screenstate;


void add_object_to_screen(object * obj, screenstate * state);

void draw_entire_display(screenstate *state);

void display_init(void);

void display_string(int line, char *s);

void display_update(void);

uint8_t display_send_byte(uint8_t data);

/* Declare bitmap array containing font */
extern const uint8_t const font[128 * 8];
/* Declare bitmap array containing icon */
extern const uint8_t const icon[128];
/* Declare text buffer for display output */
extern char textbuffer[4][16];

