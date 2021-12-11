/* main.c
   Originally "mipslabmain.c"

   This file written 2015 by Axel Isaksson,
   modified 2015, 2017 by F Lundevall
   modified 2021 by I Einberg

   For copyright and licensing, see file COPYING */

#include <stdint.h>
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "graphics.h"
#include "image_arrays.h"

int main(void) {
    /*
      This will set the peripheral bus clock to the same frequency
      as the sysclock. That means 80 MHz, when the microcontroller
      is running at 80 MHz. Changed 2017, as recommended by Axel.
    */
    SYSKEY = 0xAA996655; /* Unlock OSCCON, step 1 */
    SYSKEY = 0x556699AA; /* Unlock OSCCON, step 2 */
    while (OSCCON & (1 << 21));                  /* Wait until PBDIV ready */
    OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
    while (OSCCON & (1 << 21));          /* Wait until PBDIV ready */
    SYSKEY = 0x0; /* Lock OSCCON */

    /* Set up output pins */
    AD1PCFG = 0xFFFF;
    ODCE = 0x0;
    TRISECLR = 0xFF;
    PORTE = 0x0;

    /* Output pins for display signals */
    PORTF = 0xFFFF;
    PORTG = (1 << 9);
    ODCF = 0x0;
    ODCG = 0x0;
    TRISFCLR = 0x70;
    TRISGCLR = 0x200;

    /* Set up input pins */
    TRISDSET = (1 << 8);
    TRISFSET = (1 << 1);

    /* Set up SPI as master */
    SPI2CON = 0;
    SPI2BRG = 4;
    /* SPI2STAT bit SPIROV = 0; */
    SPI2STATCLR = 0x40;
    /* SPI2CON bit CKP = 1; */
    SPI2CONSET = 0x40;
    /* SPI2CON bit MSTEN = 1; */
    SPI2CONSET = 0x20;
    /* SPI2CON bit ON = 1; */
    SPI2CONSET = 0x8000;


    // Button 1
    TRISFSET = 0b10;
    // Buttons 2 & 3 & 4
    TRISDSET = 0b11100000;
    // LEDs
    TRISE &= ~0xFF;
    PORTE &= ~0xFF;

    display_init();
    refresh_screen(&stage1_screen);
    draw_entire_display(&stage1_screen);
    // loop image
    yeet: while (stage1_screen.current_scroll_amount <= stage1_screen.entire_image_width - 128) {
        if (PORTF & 0b10 && player_jumpflag == 0){
            player.vel.y = -5;
            player_jumpflag = 1;
        }

        if ((PORTD & 0b1000000) && player.pos.x > 0){ // move left
            player.vel.x = -1;
        } else if ((PORTD & 0b100000) && player.pos.x < 127){ // move right
            player.vel.x = 1;
        } else{
            player.vel.x = 0;
        }

//         För att kolla om programmet kraschar:
         if (PORTE == 1){
             PORTE = 0;
         } else{
             PORTE = 1;
         }
        refresh_screen(&stage1_screen);
        move_background(&stage1_screen, 1);
        move_object(&stage1_screen, &player);
        add_object_to_screen(&player, &stage1_screen);
        draw_entire_display(&stage1_screen);

        quicksleep(130000);
    }
   stage1_screen.current_scroll_amount = 0;
    goto yeet;


    return 0;
}
