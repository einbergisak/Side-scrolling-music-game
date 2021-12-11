/* graphics.c

   This file written 2021 by I Einberg

   Some parts are original code written by Axel Isaksson & F Lundevall as "mipslabfunc.c"

   For copyright and licensing, see file COPYING */

#include <stdint.h>
#include <math.h>
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

/// Sets state.current_image to the correct (scrolled) view of state.entire_image
void refresh_screen(screenstate * state) {
    // page = display memory page number, col = column in display memory page
    int page, col;

    // There are 4 display pages, each of which corresponds to 1/4 of the screen, horizontally divided.
    for (page = 0; page < 4; page++) // For each display memory page
    {
        // Fill the current page with its corresponding bytes from the entire image
        for (col = 0; col < 128; col++) // 128 bytes per page (screen width)
        {
            uint8_t byte_to_draw;
            byte_to_draw = state->entire_image[
                    page * state->entire_image_width // y-coordinates (page)
                    + col + state->current_scroll_amount // x-coordinates (column)
            ];
            state->current_image[page * 128 + col] = byte_to_draw;
        }
    }
}


/// Draws the object at object.pos, on top of what's currently in state.current_image
void add_object_to_screen(object *obj, screenstate *state) {
    int page, col;
    uint8_t obj_width = obj->size.x;
    int8_t obj_x = obj->pos.x;
    int8_t obj_y = obj->pos.y;
    uint32_t temp;
    for (col = 0; col < obj_width; col++) {
        if (obj_y < 0) { // out of bounds invisibility
            temp = obj->image[col] >> (obj_y * -1);
        } else {
            temp = obj->image[col] << obj_y;
        }
        for (page = 0; page < 4; page++) {
            state->current_image[page * 128 + obj_x + col] |= 0xFF & (temp >> 8 * page);
        }
    }
}

void move_background(screenstate * state, uint8_t amount) {
    state->current_scroll_amount += amount;
    if (check_wall_collision(state, &player)) {
//        game_over();
        show_highscore_screen();
    }
}

//void game_over() {
//    current_screen = game_over_screen;
//    put_string(1, "Game over");
//    display_textbuffer();
//    quicksleep(10000000);
//    put_string(1, "Game over");
//    put_string(3, "bitch.");
//    display_textbuffer();
//    while (1);
//    return;
//}

void show_highscore_screen() {
    uint8_t pointer = 0; // value is which letter is being selected
    uint8_t btnflag = 0; // if button was pressed
    char arr[4] = {'A', 'A', 'A', 'A'};

    selectarrow.pos.y = 24;
    while (1) {
        if (!btnflag) {
            if ((PORTD & 0b100000)) { // if btn2
                if (pointer < 3) {
                    pointer += 1;
                }
                btnflag = 1;
            } else if ((PORTD & 0b1000000)) { // if btn3
                if (pointer > 0) {
                    pointer -= 1;
                }
                btnflag = 1;
            } else if ((PORTD & 0b10000000)) { // if btn4
                if (arr[pointer] == 'Z') {
                    arr[pointer] = 'A';
                } else {
                    arr[pointer]++;
                }
                btnflag = 1;
            } else if ((PORTF & 0b10)) { // if btn1

                btnflag = 1;
            }
        } else if (!(PORTD & 0b11100000)) { // btns 234 are not pressed
            btnflag = 0;
            refresh_screen(&game_over_screen);
            selectarrow.pos.x = pointer*8;
            add_object_to_screen(&selectarrow, &game_over_screen);

            put_string(0, "New highscore!");
            put_string(1, "Enter your name:");
            put_string(2, arr); // arr skickar med ett frågetecken av nån anledning. nullbyte ?

            add_textbuffer_to_screen(&game_over_screen);

            draw_entire_display(&game_over_screen);
        }
        quicksleep(140000);
    }

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

// Puts string in textbuffer, show with display_textbuffer()
void put_string(int line, char *s) {
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

// Renders content from textbuffer
void add_textbuffer_to_screen(screenstate * state) {
    int page, letter_pos, column;
    int letter;
    for (page = 0; page < 4; page++) {
        for (letter_pos = 0; letter_pos < 16; letter_pos++) {
            letter= textbuffer[page][letter_pos];
            if (letter >= 0x80 || letter < 0x20)
                continue;

            for (column = 0; column < 8; column++) {
                state->current_image[page * 128 + letter_pos * 8 + column] |= font[letter * 8 + column];
            }
        }
    }
}


/*
 * itoa
 *
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 */
#define ITOA_BUFSIZ ( 24 )

char *itoaconv(int num) {
    register int i, sign;
    static char itoa_buffer[ITOA_BUFSIZ];
    static const char maxneg[] = "-2147483648";

    itoa_buffer[ITOA_BUFSIZ - 1] = 0;   /* Insert the end-of-string marker. */
    sign = num;                           /* Save sign. */
    if (num < 0 && num - 1 > 0)          /* Check for most negative integer */
    {
        for (i = 0; i < sizeof(maxneg); i += 1)
            itoa_buffer[i + 1] = maxneg[i];
        i = 0;
    } else {
        if (num < 0) num = -num;           /* Make number positive. */
        i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
        do {
            itoa_buffer[i] = num % 10 + '0';/* Insert next digit. */
            num = num / 10;                   /* Remove digit from number. */
            i -= 1;                           /* Move index to next empty position. */
        } while (num > 0);
        if (sign < 0) {
            itoa_buffer[i] = '-';
            i -= 1;
        }
    }
    /* Since the loop always sets the index i to the next empty position,
     * we must add 1 in order to return a pointer to the first occupied position. */
    return (&itoa_buffer[i + 1]);
}