#include "minimap.h"

#include <string.h>

#include "world.h"

#define MINIMAP_TILE_SIZE 16
#define MINIMAP_ROOM_SIZE 2
#define MINIMAP_TILE_BYTES (MINIMAP_TILE_SIZE * MINIMAP_TILE_SIZE)
#define MINIMAP_TILE_COUNT 2
#define MINIMAP_BUFFER_SIZE (MINIMAP_TILE_BYTES * MINIMAP_TILE_COUNT)
#define MINIMAP_VISITED_SIZE ((WORLD_WIDTH * WORLD_HEIGHT) / 8)
#define MINIMAP_SCREEN_X (WIDTH - MINIMAP_TILE_COUNT)
#define MINIMAP_SCREEN_Y 0

static uint8_t minimap_tiles[MINIMAP_BUFFER_SIZE];
static uint8_t minimap_visited[MINIMAP_VISITED_SIZE];
static uint8_t minimap_special[MINIMAP_VISITED_SIZE];
static Vector2_u8 minimap_position;

static uint8_t minimap_room_index(uint8_t x, uint8_t y) {
    return (y * WORLD_WIDTH) + x;
}

static uint8_t minimap_room_visited(uint8_t x, uint8_t y) {
    uint8_t room = minimap_room_index(x, y);
    return minimap_visited[room >> 3] & (1 << (room & 7));
}

static uint8_t minimap_room_is_special(uint8_t x, uint8_t y) {
    uint8_t room = minimap_room_index(x, y);
    return minimap_special[room >> 3] & (1 << (room & 7));
}

static void minimap_set_visited(uint8_t x, uint8_t y) {
    uint8_t room = minimap_room_index(x, y);
    minimap_visited[room >> 3] |= (1 << (room & 7));
}

static void minimap_set_special(uint8_t x, uint8_t y, uint8_t is_special) {
    uint8_t room = minimap_room_index(x, y);
    uint8_t bit = 1 << (room & 7);

    if (is_special) {
        minimap_special[room >> 3] |= bit;
    } else {
        minimap_special[room >> 3] &= ~bit;
    }
}

static void minimap_draw_room(uint8_t x, uint8_t y, uint8_t color) {
    uint8_t px = x * MINIMAP_ROOM_SIZE;
    uint8_t py = y * MINIMAP_ROOM_SIZE;
    uint16_t offset = ((uint16_t)(px / MINIMAP_TILE_SIZE) * MINIMAP_TILE_BYTES) +
                      ((uint16_t)py * MINIMAP_TILE_SIZE) + (px % MINIMAP_TILE_SIZE);

    minimap_tiles[offset] = color;
    minimap_tiles[offset + 1] = color;
    minimap_tiles[offset + MINIMAP_TILE_SIZE] = color;
    minimap_tiles[offset + MINIMAP_TILE_SIZE + 1] = color;
}

static gfx_error minimap_upload(gfx_context* ctx) {
    gfx_tileset_options options = {
        .compression = TILESET_COMP_NONE,
        .from_byte = ((uint16_t)MINIMAP_TILE_LEFT * MINIMAP_TILE_BYTES),
    };

    return gfx_tileset_load(ctx, minimap_tiles, MINIMAP_BUFFER_SIZE, &options);
}

static gfx_error minimap_place(gfx_context* ctx) {
    gfx_error err = gfx_tilemap_place(ctx, MINIMAP_TILE_LEFT, UI_LAYER, MINIMAP_SCREEN_X, MINIMAP_SCREEN_Y);
    if (err != GFX_SUCCESS) return err;

    return gfx_tilemap_place(ctx, MINIMAP_TILE_RIGHT, UI_LAYER, MINIMAP_SCREEN_X + 1, MINIMAP_SCREEN_Y);
}

static gfx_error minimap_render(gfx_context* ctx) {
    memset(minimap_tiles, MINIMAP_COLOR_UNVISITED, MINIMAP_BUFFER_SIZE);

    for (uint8_t y = 0; y < WORLD_HEIGHT; y++) {
        for (uint8_t x = 0; x < WORLD_WIDTH; x++) {
            if (minimap_room_visited(x, y)) {
                uint8_t color = MINIMAP_COLOR_VISITED;
                if (minimap_room_is_special(x, y)) {
                    color = MINIMAP_COLOR_SPECIAL;
                }
                minimap_draw_room(x, y, color);
            }
        }
    }

    minimap_draw_room(minimap_position.x, minimap_position.y, MINIMAP_COLOR_CURRENT);
    return minimap_upload(ctx);
}

gfx_error minimap_init(gfx_context* ctx, Vector2_u8 position) {
    memset(minimap_visited, 0, MINIMAP_VISITED_SIZE);
    memset(minimap_special, 0, MINIMAP_VISITED_SIZE);
    minimap_position = position;
    minimap_set_visited(position.x, position.y);

    gfx_error err = minimap_render(ctx);
    if (err != GFX_SUCCESS) return err;

    return minimap_place(ctx);
}

gfx_error minimap_set_position(gfx_context* ctx, Vector2_u8 position, uint8_t previous_is_special) {
    minimap_set_visited(minimap_position.x, minimap_position.y);
    minimap_set_special(minimap_position.x, minimap_position.y, previous_is_special);
    minimap_position = position;
    minimap_set_visited(position.x, position.y);
    return minimap_render(ctx);
}
