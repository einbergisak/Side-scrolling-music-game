#include "songs.h"
#include "sound.h"
#include "waves.h"
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "game.h"
#include "screenstate.h"
#include "object.h"
#include "highscore.h"

char cur_song;
char cur_tones = 0;
unsigned int song_pos = 0;

void song_init(char song) { cur_song = song; }

int loopcounter = 0;
void song_isr()
{
  IFSCLR(0) = 0x00001000;
  if (loopcounter == 10)
  {
    loopcounter = 0;
    switch (cur_song)
    {
    case 0:
      if (song_pos < twinkleLength)
      {
        cur_tones = twinkle[song_pos];
      }
      break;
    case 1:
      if (song_pos < chordsLength)
      {
        cur_tones = chords[song_pos];
      }
      break;
    }
    song_pos++;
  }
  loopcounter += 1;
  if (ingame)
  {
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
}

void audio_init()
{
  // Set up output ports for speaker control
  TRISECLR = 0xFF;
  PORTE |= 0x0;

  // Audio timer initialization
  T2CON = 0x0;            // Set timer off and prescale to 1:1
  TMR2 = 0x0;             // Reset timer count
  PR2 = ticks_per_sample; // Set period
  IFSCLR(0) = 0x00000100; // Make sure the time-out flag is cleared
  T2CONSET = 0x8000;      // Turn the timer on again
  IPCSET(2) = 0x1f;       // Set priority to max
  IECSET(0) = 0x100;      // Enable interupts for Timer 2

  // Song timer initialization
  T3CON = 0x0070;         // Set timer off and prescale to 1:256
  TMR3 = 0x0;             // Reset timer count
  PR3 = 6250;            // Set period to get total of 0.04 sec
  IFSCLR(0) = 0x00001000; // Make sure the time-out flag is cleared
  T3CONSET = 0x8000;      // Turn the timer on again
  IPCSET(3) = 0x1f;       // Set priority to max
  IECSET(0) = 0x1000;     // Enable interupts for Timer 2

  // Interrupt initialization
  asm("ei");
}

unsigned int audio_counter = 0;
void update_audio()
{
  int out = 0;
  if (cur_tones & 0x1)
  {
    out += tone_at_count(audio_counter, 0);
  }
  if (cur_tones & 0x2)
  {
    out += tone_at_count(audio_counter, 1);
  }
  if (cur_tones & 0x4)
  {
    out += tone_at_count(audio_counter, 2);
  }
  if (cur_tones & 0x8)
  {
    out += tone_at_count(audio_counter, 3);
  }
  if (cur_tones & 0x10)
  {
    out += tone_at_count(audio_counter, 4);
  }
  if (cur_tones & 0x20)
  {
    out += tone_at_count(audio_counter, 5);
  }
  PORTE = 128 + out;
  audio_counter++;
}

void audio_isr()
{
  // Reset time-out flag
  IFSCLR(0) = 0x00000100;
  update_audio();
}
