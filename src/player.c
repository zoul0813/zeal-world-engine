#include "player.h"

#include <zos_errors.h>
#include <zvb_gfx.h>
#include <zvb_sprite.h>

#include "main.h"
#include "assets.h"
#include "tile.h"
#include "zgdk/rooms.h"

player_t player;

static uint8_t player_tile_x(uint16_t x) {
    return (uint8_t)((x >> 4) - 1);
}

static uint8_t player_tile_y(uint16_t y) {
    return (uint8_t)((y >> 4) - 1);
}

static uint8_t player_type_blocks(int16_t type) {
    if (type < 0) return 1;
    switch (type) {
        case TILE_UNKNOWN:     // fall thru
        case TILE_WATER:       // fall thru
        case TILE_CHEST_OPEN:  // fall thru
        case TILE_WALL:
            return 1;
    }

    return 0;
}

static uint8_t player_body_blocks(uint16_t x, uint16_t y) {
    uint8_t tx0 = player_tile_x(x);
    uint8_t ty0 = player_tile_y(y);
    uint8_t tx1 = player_tile_x(x + SPRITE_WIDTH - 1);
    uint8_t ty1 = player_tile_y(y + SPRITE_HEIGHT - 1);

    if (player_type_blocks(tilemap_get_xy(tx0, ty0))) return 1;
    if (player_type_blocks(tilemap_get_xy(tx1, ty0))) return 1;
    if (player_type_blocks(tilemap_get_xy(tx0, ty1))) return 1;
    if (player_type_blocks(tilemap_get_xy(tx1, ty1))) return 1;

    return 0;
}

static uint8_t player_open_chest(uint8_t tx, uint8_t ty) {
    if (tilemap_get_xy(tx, ty) != TILE_CHEST) return 0;

    player.score++;
    room_set_tile(&vctx, tx, ty, TILE_ID_CHEST_OPEN);

    // player score
    format_u16_3(buffer, player.score);
    nprint_string(&vctx, buffer, 3, WIDTH - 3, HEIGHT - 1);

    return 1;
}

static void player_open_chest_at_body(uint16_t x, uint16_t y) {
    uint8_t tx0 = player_tile_x(x);
    uint8_t ty0 = player_tile_y(y);
    uint8_t tx1 = player_tile_x(x + SPRITE_WIDTH - 1);
    uint8_t ty1 = player_tile_y(y + SPRITE_HEIGHT - 1);

    if (player_open_chest(tx0, ty0)) return;
    if (player_open_chest(tx1, ty0)) return;
    if (player_open_chest(tx0, ty1)) return;
    player_open_chest(tx1, ty1);
}

error player_init(void) {
    player.score = 0;
    player.point.x = WORLD_PLAYER_X;
    player.point.y = WORLD_PLAYER_Y;

    gfx_sprite sprite = {
        .tile = TILE_ID_PLAYER_HEAD,
        .flags = SPRITE_NONE,
        .options = SPRITE_OPTION_32PX,
    };
    player.sprite = sprites_register(sprite);
    if (player.sprite == NULL) return ERR_NO_MORE_MEMORY;

    return GFX_SUCCESS;
}

error player_deinit(void) {
    return ERR_SUCCESS;
}

Edge player_move(void) {
    uint16_t x = player.point.x;
    uint16_t y = player.point.y;
    uint16_t nx = x + (player.direction.x * PLAYER_SPEED);
    uint16_t ny = y + (player.direction.y * PLAYER_SPEED);
    Edge edge = EdgeNone;

    switch (player.direction.x) {
        case DIRECTION_LEFT: {
            player.sprite->flags = SPRITE_FLIP_X;
        } break;
        case DIRECTION_RIGHT: {
            player.sprite->flags = SPRITE_NONE;
        } break;
    }

    if ((player.direction.x < 0) && (nx < SPRITE_WIDTH)) {
        x = SPRITE_WIDTH;
        edge = EdgeLeft;
    } else if ((player.direction.x > 0) && (nx >= SCREEN_WIDTH)) {
        x = SCREEN_WIDTH;
        edge = EdgeRight;
    }
    if ((player.direction.y < 0) && (ny < SPRITE_HEIGHT)) {
        y = SPRITE_HEIGHT;
        edge = EdgeTop;
    } else if ((player.direction.y > 0) && (ny >= SCREEN_HEIGHT)) {
        y = SCREEN_HEIGHT;
        edge = EdgeBottom;
    }

    if (edge != EdgeNone) {
        player_set_xy(x, y);
        return edge;
    }

#if DEBUG
    uint8_t tx = player_tile_x(nx);
    uint8_t ty = player_tile_y(ny);

    // type
    format_u16_3(buffer, tilemap_get_xy(tx, ty));
    nprint_string(&vctx, buffer, 3, 0, HEIGHT - 3);

    // player x,y
    format_u16_3_pair(buffer, player.point.x, player.point.y);
    nprint_string(&vctx, buffer, 7, 0, HEIGHT - 2);

    // screen x,y
    format_u16_3_pair(buffer, tx, ty);
    nprint_string(&vctx, buffer, 7, 0, HEIGHT - 1);
#endif

    if ((player.direction.x == 0) || !player_body_blocks(nx, y)) x = nx;
    if ((player.direction.y == 0) || !player_body_blocks(x, ny)) y = ny;

    player_open_chest_at_body(x, y);
    player_set_xy(x, y);

    return EdgeNone;
}

void player_set_xy(uint16_t x, uint16_t y) {
    player.point.x = x;
    player.point.y = y;
    player.sprite->x = x;
    player.sprite->y = y - SPRITE_HEIGHT;
}
