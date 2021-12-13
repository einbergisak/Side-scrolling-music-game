#include "graphics.h"
#include "object.h"
#include "image_arrays.h"
#include "highscore.h"
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
void play_level(int level)
{
    player_jumpflag = 1;
    player.vel.x = 0;
    player.vel.y = 0;
    player.pos.x = 4;
    player.pos.y = 0;

    screenstate *current_level_screen;
    if (level == 1)
    {
        current_level_screen = &stage1_screen;
    }
    else if (level == 2)
    {
        current_level_screen = &stage2_screen;
    }
    current_level_screen->current_scroll_amount = 0;
    while (current_level_screen->current_scroll_amount <= current_level_screen->entire_image_width - 128)
    {
        if (!btn_dwn_flag)
        {
            if (PORTF & 0b10 && player_jumpflag == 0)
            {
                player.vel.y = -5;
                player_jumpflag = 1;
                btn_dwn_flag = 1;
            }

            if ((PORTD & 0b1000000) && player.pos.x > 0)
            { // move left
                player.vel.x = -1;
                btn_dwn_flag = 1;
            }
            else if ((PORTD & 0b100000) && player.pos.x < 127)
            { // move right
                player.vel.x = 1;
                btn_dwn_flag = 1;
            }
        }
        else if (!(PORTF & 0b10 || PORTD & 0b11100000))
        {
            btn_dwn_flag = 0;
            player.vel.x = 0;
        }

        //         För att kolla om programmet kraschar:
        //  if (PORTE == 1){
        //      PORTE = 0;
        //  } else{
        //      PORTE = 1;
        //  }
        refresh_screen(current_level_screen);
        move_background(current_level_screen, 1);
        if (move_object(current_level_screen, &player))
        { // if game is over
            btn_dwn_flag = 1;
            return;
        }
        add_object_to_screen(&player, current_level_screen);
        draw_entire_display(current_level_screen);
        // Highscore is percentage of level completed
        current_highscore = current_level_screen->current_scroll_amount / (current_level_screen->entire_image_width - 128);
        quicksleep(130000);
    }
    game_over();
    refresh_screen(current_level_screen);
    put_string(2, "YOU WON!");
    add_textbuffer_to_screen(current_level_screen);
    draw_entire_display(current_level_screen);
    quicksleep(3500000);
}

void game_over()
{
    if (1 /*score > current_highscore*/)
    {
        show_enter_highscore_screen();
    }
    //    put_string(1, "Game over");
    //    display_textbuffer();
    //    quicksleep(10000000);
    //    put_string(1, "Game over");
    //    put_string(3, "bitch.");
    //    display_textbuffer();
    return;
}
