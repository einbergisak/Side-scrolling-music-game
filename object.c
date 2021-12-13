//
// Created by Isak on 2021-12-05.
//

#include "object.h"
#include "image_arrays.h"

object player = {
        .image = {0b111110, 0b100011, 0b100101, 0b100101, 0b100011, 0b111110}, // 6x6 cube,
        .size = {.x = 6, .y = 6},
        .pos = {.x = 4, .y = 3},
        .vel = {.x = 0, .y = 0}};

object selectarrow = {
        .image = {0,0b00001100, 0b00000110, 0xFF, 0xFF, 0b00000110, 0b00001100, 0},
        .size = {.x = 8, .y = 8}
};

uint8_t player_jumpflag = 0;

// if 1 then the game is over.
int move_object(screenstate * state, object *obj) {
    int i, xdir, ydir;

    if (obj->vel.x < 0) {
        xdir = -1;
    } else {
        xdir = 1;
    }

    // easy but dumb way to check if the object is the player
        if (check_wall_collision(state, obj) && obj->pos.x == player.pos.x && obj->pos.y == player.pos.y) {
            game_over();
            return 1;
        }

    // for each movement in x direction.
    for (i = 0; i < obj->vel.x * xdir; i++) {
        obj->pos.x += 1 * xdir;

        // easy but dumb way to check if the object is the player
        if (check_wall_collision(state, obj) && obj->pos.x == player.pos.x && obj->pos.y == player.pos.y) {
            game_over();
            return 1;
        }
    }

    // for each movement in y direction
    if (obj->vel.y < 0) {
        ydir = -1;
    } else {
        ydir = 1;
    }
    for (i = 0; i < obj->vel.y * ydir; i++) {
        obj->pos.y += 1 * ydir;

        // If touching the ground, stop descending.
        if (obj->pos.y >= obj->size.y && check_vertical_collision(state, obj)) {
            break;
        } else if (obj->pos.y > 32 - obj->size.y){
            game_over();
        }
    }

    obj->vel.y += 1;
    return 0;
}

uint8_t check_vertical_collision(screenstate * state, object *obj) {
    uint8_t col, page, has_contact = 0;
    uint32_t temp, bottom_index, top_pixel, bottom_pixel;
    for (col = 0; col < obj->size.x; col++) {
        temp = obj->image[col] << obj->pos.y;          // shift image into position
        bottom_index = (obj->pos.y + obj->size.y - 1); // calculate y-position of obj bottom.
        top_pixel = temp & (0x1 << obj->pos.y);
        bottom_pixel = temp & (0x00000001 << bottom_index);                   // mask out the bottom pixel
        for (page = 0; page < 4; page++) {
            // ground collision (player stops ascent)
            if (state->current_image[page * 128 + obj->pos.x + col] & bottom_pixel >> 8 * page) {

                if (!has_contact) { // if th
                    has_contact = 1;
                    continue;
                }
                obj->vel.y = 0;
                obj->pos.y -= 1;
                player_jumpflag = 0;
                return 1;
            }

            // top collision (player bounces back)
            if (state->current_image[page * 128 + obj->pos.x + col] & top_pixel >> 8 * page) {
                obj->vel.y *= -1;
                obj->pos.y += 1;
                return 1;
            }
        }
    }
    if (has_contact) {

    }
    return 0;
}

uint8_t check_wall_collision(screenstate * state, object *obj) {
    int page;

    if (obj->pos.y < 0) { // wrap invincibility
        return 0;
    }

    uint32_t temp = obj->image[obj->size.x - 1] << obj->pos.y;   // shift rightmost column into position
//        temp &= ~(1 << (obj->pos.y+obj->size.y-1)); // Mask out bit representing bottom pixel in column.
    for (page = 0; page < 4; page++) {
        if (state->current_image[page * 128 + obj->pos.x + obj->size.x - 1] & temp >> page * 8) {
            return 1;
        }

    }
    return 0;

}

void check_player_collision() {
}