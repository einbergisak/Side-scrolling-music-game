#include "graphics.h"
#include "object.h"
#include "image_arrays.h"
#include "highscore.h"
#include <pic32mx.h> /* Declarations of system-specific addresses etc */

int ingame = 0;
screenstate *current_level_screen;

/// Resets game variables and starts the game by setting ingame = 1
/// The game loop will then run in the sound ISR
void play_level(int level)
{
    player_jumpflag = 1;
    player.vel.x = 0;
    player.vel.y = 0;
    player.pos.x = 4;
    player.pos.y = 0;
    if (level == 0)
    {
        current_level_screen = &stage1_screen;
    }
    else if (level == 1)
    {
        current_level_screen = &stage2_screen;
    }
    current_level_screen->current_scroll_amount = 0;
    ingame = 1; // enables game logic in song_isr
    while (ingame)
        ; // loop until game over;
}


/// Ends the game
void game_over()
{
    ingame = 0;
    if (1 /*score > current_highscore*/)
    {
        show_enter_highscore_screen();
    }
    return;
}

/// Moves the level and the player and updates game graphics.
void update_game()
{
    // If the game hasn't progressed to its end
    if (current_level_screen->current_scroll_amount <= current_level_screen->entire_image_width - 128)
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
        current_highscore = (current_level_screen->current_scroll_amount * 100) / (current_level_screen->entire_image_width - 128);
    }
    else
    {
        game_over();
        refresh_screen(current_level_screen);
        put_string(2, "YOU WON!");
        add_textbuffer_to_screen(current_level_screen);
        draw_entire_display(current_level_screen);
        quicksleep(3500000);
    }
}

