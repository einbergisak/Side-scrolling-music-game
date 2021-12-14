#include "songs.h"
#include "sound.h"
#include "waves.h"
#include <pic32mx.h> /* Declarations of system-specific addresses etc */

char *cur_song;
char cur_tones;
int song_length;
unsigned int song_pos;
int cur_tempo;
char playing = 0;

void song_init(char song_index) {
  // Ensure sound is paused
  playing = 0;

  // Set values to start position
  cur_tones = 0;
  song_pos = 0;

  // Set cur_song and get its length
  cur_song = songs[song_index];
  song_length = song_lengths[song_index];
  cur_tempo = song_tempos[song_index];

  // Resume playback
  playing = 1;
}

void song_start() { playing = 1; }

void song_stop() { playing = 0; }

int song_int_count = 0;
void song_isr() {
  // If not paused or past song end, set cur_tones and increment song_pos
  if (playing) {
    song_int_count++;
    if (song_int_count >= cur_tempo && playing && song_pos < song_length) {
      song_int_count = 0;
      cur_tones = cur_song[song_pos];
      song_pos++;
    }
  }
}

void audio_init() {
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

  // Interrupt initialization
  asm("ei");
}

unsigned int audio_counter = 0;
void update_audio() {
  // Check all bits in cur_tones and sum the waves of those that should be
  // played
  int out = 0;
  int n;
  for (n = 0; n < 8; n++) {
    if (cur_tones & 1 << n) {
      out += tone_at_count(audio_counter, n);
    }
  }

  PORTE = 128 + out;
  audio_counter++;
}

void audio_isr() {
  // Reset time-out flag
  IFSCLR(0) = 0x00000100;
  if (playing) {
    update_audio();
  }
}
