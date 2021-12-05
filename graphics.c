/* graphics.c

   This file written 2021 by I Einberg

   Some parts are original code written by Axel Isaksson & F Lundevall as "mipslabfunc.c"

   For copyright and licensing, see file COPYING */

#include <stdint.h>
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "graphics.h" // Declarations for this file
#include "image_arrays.h"

/* Declare a helper function which is local to this file */
static void num32asc(char *s, int);

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
    int i;
    for (i = cyc; i > 0; i--);
}

uint8_t display_send_byte(uint8_t data) {
    // Wait for transmitter
    while (!(SPI2STAT & 0x08));

    // Write transmit byte
    SPI2BUF = data;

    // Wait for recieve byte
    while (!(SPI2STAT & 1));

    return SPI2BUF;
}

// Reference: https://digilent.com/reference/_media/chipkit_shield_basic_io_shield:chipkit_basic_io_shield_rm.pdf
void draw_entire_display(screenstate *state) {
    // page = display memory page number, col = column in display memory page
    int page, col;

    // There are 4 display pages, each of which corresponds to 1/4 of the screen, horizontally divided.
    for (page = 0; page < 4; page++) // For each display memory page
    {
        DISPLAY_CHANGE_TO_COMMAND_MODE;

        display_send_byte(0x22); // "Set page" command
        display_send_byte(page); // Set page number

        display_send_byte(0);    // Set low nybble of column
        display_send_byte(0x10); // Set high nybble of column

        DISPLAY_CHANGE_TO_DATA_MODE;

        // Fill the current page with its corresponding bytes
        for (col = 0; col < 128; col++) // 128 bytes per page (screen width)
        {
            display_send_byte(
                    state->current_image[page * 128 + col]
            );
        }
    }
}

/// Sets current_screen.current_image to the correct (scrolled) view of current_screen.entire_image
void update_current_screen() {
    // page = display memory page number, col = column in display memory page
    int page, col;

    // There are 4 display pages, each of which corresponds to 1/4 of the screen, horizontally divided.
    for (page = 0; page < 4; page++) // For each display memory page
    {
        // Fill the current page with its corresponding bytes from the entire image
        for (col = 0; col < 128; col++) // 128 bytes per page (screen width)
        {
            uint8_t byte_to_draw;
            byte_to_draw = current_screen.entire_image[
                    page * current_screen.entire_image_width // y-coordinates (page)
                    + col + current_screen.current_scroll_amount // x-coordinates (column)
            ];
            current_screen.current_image[page * 128 + col] = byte_to_draw;
        }
    }
}


/// Draws the object at object.pos, on top of what's currently in current_screen.current_image
void add_object_to_screen(object * obj, screenstate * state) {
    int page, col, i, j;
    uint8_t obj_width = obj->size.x;
    uint8_t obj_x = obj->pos.x;
    uint8_t obj_y = obj->pos.y;
    uint8_t positioned_obj[512];
    for (col = 0; col < obj_width; col++){
//        temp = obj->image[i] + (obj->image[obj_width+i] << 8) + (obj->image[2*obj_width+i] << 16) + (obj->image[3*obj_width+i] << 24);
        uint32_t temp = obj->image[col] << obj_y;
        for (page = 0; page < 4; page++){
            state->current_image[page * 128 + obj_x + col] |= 0xFF & (temp >> 8*page);
        }
    }

//    for (page = 0; page < 4; page++) {
//        for (col = 0; col < obj_width; col++) {
//            state->current_image[page * 128 + obj_x + col] |= positioned_obj[page * obj_width + col];
//        }
//    }
}


void display_init(void) {
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    quicksleep(10);
    DISPLAY_ACTIVATE_VDD;
    quicksleep(1000000);

    display_send_byte(0xAE);
    DISPLAY_ACTIVATE_RESET;
    quicksleep(10);
    DISPLAY_DO_NOT_RESET;
    quicksleep(10);

    display_send_byte(0x8D);
    display_send_byte(0x14);

    display_send_byte(0xD9);
    display_send_byte(0xF1);

    DISPLAY_ACTIVATE_VBAT;
    quicksleep(10000000);

    display_send_byte(0xA1);
    display_send_byte(0xC8);

    display_send_byte(0xDA);
    display_send_byte(0x20);

    display_send_byte(0xAF);
}

void display_string(int line, char *s) {
    int i;
    if (line < 0 || line >= 4)
        return;
    if (!s)
        return;

    for (i = 0; i < 16; i++)
        if (*s) {
            textbuffer[line][i] = *s;
            s++;
        } else
            textbuffer[line][i] = ' ';
}

void display_update(void) {
    int i, j, k;
    int c;
    for (i = 0; i < 4; i++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
        display_send_byte(0x22);
        display_send_byte(i);

        display_send_byte(0x0);
        display_send_byte(0x10);

        DISPLAY_CHANGE_TO_DATA_MODE;

        for (j = 0; j < 16; j++) {
            c = textbuffer[i][j];
            if (c & 0x80)
                continue;

            for (k = 0; k < 8; k++)
                display_send_byte(font[c * 8 + k]);
        }
    }
}
