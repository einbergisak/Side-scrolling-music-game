//
// Created by Isak on 2021-12-05.
//

#include <stdint.h>
#include "screenstate.h"

typedef struct vec2 {
    int32_t x;
    int32_t y;
} vec2;

typedef struct object {
    vec2 size;
    vec2 pos;
    vec2 vel;
    uint32_t image[]; // flexible array member
} object;

void move_object(screenstate * state, object * obj);
uint8_t check_vertical_collision(screenstate * state, object * obj);
void check_player_collision();
uint8_t check_wall_collision(screenstate * state, object * obj);
uint8_t player_jumpflag;

object player;
object selectarrow;