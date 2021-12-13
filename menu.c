#include "image_arrays.h"
#include "graphics.h"
#include "highscore.h"
#include "game.h"
#include <pic32mx.h> /* Declarations of system-specific addresses etc */

void level_selection()
{
    uint8_t selected = 0;
    selectarrow.pos.y = 23;
    while (1)
    {
        if (!btn_dwn_flag)
        {
            if (PORTF & 0b10)
            { // if btn1
                btn_dwn_flag = 1;
                play_level(selected);
            }

            refresh_screen(&level_selection_screen);
            if (PORTD & 0b100000)
            { // if btn2
                selected = 1;
            }
            else if (PORTD & 0b1000000)
            { // if btn3
                selected = 0;
            }else if (PORTD & 0b10000000){ //if btn 4
                return;
            }
            if (selected)
            {
                selectarrow.pos.x = 92;
            }
            else
            {
                selectarrow.pos.x = 25;
            }
            add_object_to_screen(&selectarrow, &level_selection_screen);
            draw_entire_display(&level_selection_screen);
        }
        else if (!(PORTF & 0b10 || PORTD & 0b11100000))
        {
            btn_dwn_flag = 0;
        }
    }
}

void main_menu()
{
    uint8_t selected = 0;
    selectarrow.pos.y = 28;
    while (1)
    {

        if (!btn_dwn_flag)
        {
            if (PORTF & 0b10)
            { // if btn1
                btn_dwn_flag = 1;
                if (selected)
                {
                    show_highscore_screen();
                }
                else
                {
                    level_selection();
                }
            }

            refresh_screen(&main_menu_screen);
            if (PORTD & 0b100000)
            { // if btn2
                selected = 1;
            }
            else if (PORTD & 0b1000000)
            { // if btn3
                selected = 0;
            }
            if (selected)
            {
                selectarrow.pos.x = 98;
            }
            else
            {
                selectarrow.pos.x = 28;
            }
            add_object_to_screen(&selectarrow, &main_menu_screen);
            draw_entire_display(&main_menu_screen);
        }
        else if (!(PORTF & 0b10 || PORTD & 0b11100000))
        {
            btn_dwn_flag = 0;
        }
    }
}
