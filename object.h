//
// Created by Isak on 2021-12-05.
//

#include <stdint.h>

typedef struct vec2 {
    uint32_t x;
    uint32_t y;
} vec2;

typedef struct object {
    vec2 size;
    vec2 pos;
    uint32_t image[]; // flexible array member
} object;

object player;