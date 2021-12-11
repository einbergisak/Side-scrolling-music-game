//
// Created by Isak on 2021-12-05.
//

#include "object.h"
#include "image_arrays.h"

object player = {
    .image = {0b111110, 0b100011, 0b100101, 0b100101, 0b100011, 0b111110}, // 6x6 cube,
    .size = {.x = 6, .y = 6},
    .pos = {.x = 0, .y = 0},
    .vel = {.x = 0, .y = 0}};

uint8_t player_jumpflag = 0;

void move_object(object *obj)
{
        int i, xdir, ydir;

        // if (obj->vel.x < 0)
        // {
        //         xdir = -1;
        // }
        // else
        // {
        //         xdir = 1;
        // }
        // for (i = 0; i < obj->vel.x * xdir; i++)
        // {
        //         obj->pos.x += 1 * xdir;
        //         // TODO: check sideways collision elr nått
        // }
        if (obj->vel.y < 0)
        {
                ydir = -1;
        }
        else
        {
                ydir = 1;
        }
        for (i = 0; i < obj->vel.y * ydir; i++)
        {
                obj->pos.y += 1 * ydir;



                // If touching the ground, stop descending.
                if (obj->pos.y >= 0 && check_vertical_collision(obj))
                {
                        break;
                }
        }

        obj->vel.y += 1;
}

uint8_t check_vertical_collision(object *obj)
{
        uint8_t col, page;
        uint32_t temp, bottom_index, top_pixel, bottom_pixel;
        for (col = 0; col < obj->size.x; col++)
        {
                temp = obj->image[col] << obj->pos.y;          // shift image into position
                bottom_index = (obj->pos.y + obj->size.y - 1); // calculate y-position of obj bottom.
                top_pixel = temp & (0x1 << obj->pos.y);
                bottom_pixel = temp & (0x00000001 << bottom_index);                   // mask out the bottom pixel
                for (page = 0; page < 4; page++)
                {
                        // temp = obj->image[i] + (obj->image[obj_width+i] << 8) + (obj->image[2*obj_width+i] << 16) + (obj->image[3*obj_width+i] << 24);
                        if (current_screen.current_image[page * 128 + obj->pos.x + col] & bottom_pixel >> 8*page)
                        {
                                obj->vel.y = 0;
                                obj->pos.y -= 1;
                                player_jumpflag = 0;
                                return 1;
                        }
                        if (current_screen.current_image[page * 128 + obj->pos.x + col] & top_pixel >> 8*page){
                            obj->vel.y *= -1;
                            obj->pos.y += 1;
                            return 1;
                        }
                }
        }
    return 0;
}

uint8_t check_wall_collision(object * obj){
        int page;

        if (obj->pos.y < 0){ // wrap invincibility
            return 0;
        }

        uint32_t temp = obj->image[obj->size.x-1] << obj->pos.y;   // shift rightmost column into position
//        temp &= ~(1 << (obj->pos.y+obj->size.y-1)); // Mask out bit representing bottom pixel in column.
        for (page = 0; page < 4; page++){
                if (current_screen.current_image[page*128 + obj->pos.x + obj->size.x - 1] & temp >> page*8){
                        return 1;
                }

        }
        return 0;

}

void check_player_collision()
{
}