#include "player.h"

#include <zos_errors.h>
#include <zvb_gfx.h>
#include <zvb_sprite.h>

#include "main.h"
#include "assets.h"
#include "world.h"

player_t player;

error player_init(void) {
    player.score = 0;
    player.point.x = WORLD_PLAYER_X;
    player.point.y = WORLD_PLAYER_Y;

    gfx_sprite sprite = {
        .tile = 42,
        .flags = SPRITE_NONE,
        .options = SPRITE_OPTION_32PX,
    };
    player.sprite = sprites_register(sprite);

    return GFX_SUCCESS;
}

error player_deinit(void) {
    return ERR_SUCCESS;
}

Edge player_move(void) {
    uint16_t x = player.point.x;
    uint16_t y = player.point.y;
    x += (player.direction.x * PLAYER_SPEED);
    y += (player.direction.y * PLAYER_SPEED);
    Edge edge = EdgeNone;

    switch (player.direction.x) {
        case DIRECTION_LEFT: {
            player.sprite->flags = SPRITE_FLIP_X;
        } break;
        case DIRECTION_RIGHT: {
            player.sprite->flags = SPRITE_NONE;
        } break;
    }

    // TODO: better tile x/y coordinate system?
    uint8_t tx, ty;
    tx = (x >> 4) - 1;
    if (player.direction.x > 0) tx++;
    ty = (y >> 4) - 1;
    if (player.direction.y > 0) ty++;

    if ((player.direction.x < 0) && (x < SPRITE_WIDTH)) {
        x = SPRITE_WIDTH;
        edge = EdgeLeft;
    } else if ((player.direction.x > 0) && (x >= SCREEN_WIDTH)) {
        x = SCREEN_WIDTH;
        edge = EdgeRight;
    }
    if ((player.direction.y < 0) && (y < SPRITE_HEIGHT)) {
        y = SPRITE_HEIGHT;
        edge = EdgeTop;
    } else if ((player.direction.y > 0) && (y >= SCREEN_HEIGHT)) {
        y = SCREEN_HEIGHT;
        edge = EdgeBottom;
    }

    if (edge != EdgeNone) {
        player_set_xy(x, y);
        return edge;
    }

    TileType type = screen_get_tile(tx, ty);

#if DEBUG
    // type
    format_u16_3(buffer, type);
    nprint_string(&vctx, buffer, 3, 0, HEIGHT - 3);

    // player x,y
    format_u16_3_pair(buffer, player.point.x, player.point.y);
    nprint_string(&vctx, buffer, 7, 0, HEIGHT - 2);

    // screen x,y
    format_u16_3_pair(buffer, tx, ty);
    nprint_string(&vctx, buffer, 7, 0, HEIGHT - 1);
#endif

    switch (type) {
        case TILE_UNKNOWN:     // fall thru
        case TILE_WATER:       // fall thru
        case TILE_CHEST_OPEN:  // fall thru
        case TILE_WALL:
            // TODO: only block movement on the direction that has the block obstruction
            return EdgeNone;
        case TILE_CHEST:
            player.score++;
            world_set_tile(tx, ty, TILE_ID_CHEST_OPEN);  // make it a ground tile

            // player score
            format_u16_3(buffer, player.score);
            nprint_string(&vctx, buffer, 3, WIDTH - 3, HEIGHT - 1);
            break;
    }

    player_set_xy(x, y);

    return EdgeNone;
}

void player_set_xy(uint16_t x, uint16_t y) {
    player.point.x = x;
    player.point.y = y;
    player.sprite->x = x;
    player.sprite->y = y - SPRITE_HEIGHT;
}
