#include "graphics.h"
#include "object.h"
#include "image_arrays.h"
#include "highscore.h"
#include <pic32mx.h> /* Declarations of system-specific addresses etc */

int ingame = 0;
screenstate * current_level_screen;
void play_level(int level){
    player_jumpflag = 1;
    player.vel.x = 0;
    player.vel.y = 0;
    player.pos.x = 4;
    player.pos.y = 0;
    if (level == 0){
        current_level_screen = &stage1_screen;
    } else if (level == 1) {
        current_level_screen = &stage2_screen;
    }
    current_level_screen->current_scroll_amount = 0;
    ingame = 1; // enables game logic in song_isr
    while (ingame); // loop until game over;
}

void game_over() {
    ingame = 0;
    if (1/*score > current_highscore*/){
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
