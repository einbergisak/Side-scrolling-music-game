#include "graphics.h"
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "image_arrays.h"
#include "highscore.h"
#include <math.h>

char highscore_list[3][8] = {"        ", "        ", "        "};
int current_score = 0;
int current_highscore = 0;

/// Shows the screen with the highscore input interface
void show_enter_highscore_screen()
{
    uint8_t pointer = 0; // value is which letter is being selected
    char arr[4] = {'A', 'A', 'A', 'A'};

    selectarrow.pos.y = 24;
    while (1)
    {
        if (!(PORTF & 0b10 || PORTD & 0b11100000))
        { // btns 1234 are not pressed
            btn_dwn_flag = 0;
            refresh_screen(&enter_highscore_screen);
            selectarrow.pos.x = pointer * 8;
            add_object_to_screen(&selectarrow, &enter_highscore_screen);

            put_string(0, "New highscore!");
            put_string(1, "Enter your name:");
            put_string(2, "         ");
            put_string(3, "         ");
            put_short_string(2, arr, 4);

            add_textbuffer_to_screen(&enter_highscore_screen);

            draw_entire_display(&enter_highscore_screen);
        }
        else if (!btn_dwn_flag)
        {
            if ((PORTD & 0b100000))
            { // if btn2
                if (pointer < 3)
                {
                    pointer += 1;
                }
                btn_dwn_flag = 1;
            }
            else if ((PORTD & 0b1000000))
            { // if btn3
                if (pointer > 0)
                {
                    pointer -= 1;
                }
                btn_dwn_flag = 1;
            }
            else if ((PORTD & 0b10000000))
            { // if btn4
                if (arr[pointer] == 'Z')
                {
                    arr[pointer] = 'A';
                }
                else
                {
                    arr[pointer]++;
                }
                btn_dwn_flag = 1;
            }
            else if ((PORTF & 0b10))
            { // if btn1
                int i;
                for (i = 0; i < 8; i++)
                {
                    *(*(highscore_list + 2) + i) = *(*(highscore_list + 1) + i);
                }
                for (i = 0; i < 8; i++)
                {
                    *(*(highscore_list + 1) + i) = *(*(highscore_list) + i);
                }
                for (i = 0; i < 4; i++)
                {
                    *(*(highscore_list) + i) = arr[i];
                }
                *(*(highscore_list) + 4) = ' ';
                char *score = itoaconv(current_score);
                *(*(highscore_list) + 5) = *score;


                int num_digits;
                if (current_score < 10)
                {
                    num_digits = 1;
                }
                else if (current_score < 100)
                {
                    num_digits = 2;
                    *(*(highscore_list) + 6) = *(score + 1);
                }
                else
                {
                    *(*(highscore_list) + 6) = *(score + 1);
                    *(*(highscore_list) + 7) = *(score + 2);
                    num_digits = 3;
                }
                *(*(highscore_list) + 5 + num_digits) = '%';
                btn_dwn_flag = 1;
                return;
            }
        }
    }
}

/// Displays the screen with the highscore list
void show_highscore_screen()
{
    while (1)
    {
        if (!btn_dwn_flag)
        {
            if ((PORTD & 0b10000000) || (PORTF & 0b10))
            {
                btn_dwn_flag = 1;
                return;
            }
        }
        else if (!(PORTF & 0b10 || PORTD & 0b11100000))
        {
            btn_dwn_flag = 0;
        }

        refresh_screen(&highscore_screen);
        put_string(0, "Players");
        put_short_string(1, highscore_list[0], 9);
        put_short_string(2, highscore_list[1], 9);
        put_short_string(3, highscore_list[2], 9);
        add_textbuffer_to_screen(&highscore_screen);
        draw_entire_display(&highscore_screen);
    }
}

/// Puts given string in textbuffer
void put_short_string(int line, char *s, int num)
{
    int i;
    if (line < 0 || line >= 4)
        return;
    if (!s)
        return;

    for (i = 0; i < num; i++)
        if (*s)
        {
            textbuffer[line][i] = *s;
            s++;
        }
        else
            textbuffer[line][i] = ' ';
    for (; i < 16; i++)
    {
        textbuffer[line][i] = ' ';
    }
}