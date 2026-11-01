#include "render.h"
#include "state.h"
#include "../world/layout.h"

#include <string.h>

#define MINIMAP_TILE_SIZE 16
#define MINIMAP_ROOM_SIZE 2
#define MINIMAP_TILE_BYTES (MINIMAP_TILE_SIZE * MINIMAP_TILE_SIZE)
#define MINIMAP_TILE_COUNT 2
#define MINIMAP_BUFFER_SIZE (MINIMAP_TILE_BYTES * MINIMAP_TILE_COUNT)
#define MINIMAP_SCREEN_X (WIDTH - MINIMAP_TILE_COUNT)
#define MINIMAP_SCREEN_Y 0

static uint8_t minimap_tiles[MINIMAP_BUFFER_SIZE];

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

gfx_error minimap_place(gfx_context* ctx) {
    if (ctx == NULL) return GFX_INVALID_ARG;
    gfx_error err = gfx_tilemap_place(ctx, MINIMAP_TILE_LEFT, UI_LAYER, MINIMAP_SCREEN_X, MINIMAP_SCREEN_Y);
    if (err != GFX_SUCCESS) return err;

    return gfx_tilemap_place(ctx, MINIMAP_TILE_RIGHT, UI_LAYER, MINIMAP_SCREEN_X + 1, MINIMAP_SCREEN_Y);
}

gfx_error minimap_render(gfx_context* ctx) {
    if (ctx == NULL) return GFX_INVALID_ARG;
    Vector2_u8 position;
    position = minimap_get_position();
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

    minimap_draw_room(position.x, position.y, MINIMAP_COLOR_CURRENT);
    return minimap_upload(ctx);
}

