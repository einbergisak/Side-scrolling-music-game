#ifndef SCREENSTATE_H
#define SCREENSTATE_H
#include <stdint.h>
int btn_dwn_flag;

typedef struct screenstate {
    uint32_t current_scroll_amount; // Screen x-position relative to entire source image
    uint32_t entire_image_width; // Width of source image
    uint8_t current_image[512]; // The part of the entire image that is currently displayed
    uint8_t entire_image[]; // Entire source image as flexible array member
} screenstate;

screenstate * current_level_screen;
#endif