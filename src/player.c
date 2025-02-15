#include <string.h>
#include <zos_errors.h>
#include <zvb_gfx.h>
#include <zvb_sprite.h>

#include "player.h"
#include "main.h"
#include "world.h"

player_t player;

error player_init(void)
{
    player.score = 0;
    player.point.x = SCREEN_WIDTH / 2;
    player.point.y = SCREEN_HEIGHT / 2;

    gfx_sprite sprite = {
        .tile = 58,
        .flags = SPRITE_NONE,
    };
    player.sprite_body = sprites_register(sprite);

    sprite.tile = 42;
    player.sprite_head = sprites_register(sprite);

    return GFX_SUCCESS;
}

error player_deinit(void)
{
    return ERR_SUCCESS;
}

Edge player_move(void)
{
    uint16_t x = player.point.x;
    uint16_t y = player.point.y;
    x          += (player.direction.x * PLAYER_SPEED);
    y          += (player.direction.y * PLAYER_SPEED);

    switch (player.direction.x) {
        case DIRECTION_LEFT: {
            player.sprite_head->flags = SPRITE_FLIP_X;
            player.sprite_body->flags = SPRITE_FLIP_X;
        } break;
        case DIRECTION_RIGHT: {
            player.sprite_head->flags = SPRITE_NONE;
            player.sprite_body->flags = SPRITE_NONE;
        } break;
    }

    uint8_t tx, ty;
    tx = (x >> 4) - 1;
    if(player.direction.x > 0) tx++;
    ty = (y >> 4) - 1;
    if(player.direction.y > 0) ty++;

    TileType type = screen_get_tile(tx, ty);

    // type
    sprintf(buffer, "%03d", type);
    nprint_string(&vctx, buffer, 3, 0, HEIGHT-3);

    // // player x,y
    // sprintf(buffer, "%03d %03d", player.point.x, player.point.y);
    // nprint_string(&vctx, buffer, 7, 0, HEIGHT-2);

    // screen x,y
    sprintf(buffer, "%03d %03d", tx, ty);
    nprint_string(&vctx, buffer, 7, 0, HEIGHT-1);

    switch(type) {
        case TILE_UNKNOWN: // fall thru
        case TILE_WATER: // fall thru
        case TILE_WALL:
            // obstruction
            return EdgeNone;
        case TILE_CHEST:
            player.score++;
            world_set_tile(tx, ty, 0); // make it a ground tile

            // player score
            sprintf(buffer, "%03d", player.score);
            nprint_string(&vctx, buffer, 3, WIDTH-3, HEIGHT-1);
            break;
    }


    player_set_xy(x, y);

    if(x < SPRITE_WIDTH) return EdgeLeft;
    else if(x > SCREEN_WIDTH) return EdgeRight;
    if(y < SPRITE_HEIGHT) return EdgeTop;
    else if(y > SCREEN_HEIGHT) return EdgeBottom;
    return EdgeNone;
}

void player_set_xy(uint16_t x, uint16_t y) {
    player.point.x = x;
    player.point.y = y;
    player.sprite_body->x = x;
    player.sprite_head->x = x;
    player.sprite_body->y = y;
    player.sprite_head->y = y - SPRITE_HEIGHT;
}
