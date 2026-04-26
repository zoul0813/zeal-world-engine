#include "ui_map.h"

#include <string.h>

#include "main.h"
#include "world.h"

#define UI_MAP_TILE_SIZE 16
#define UI_MAP_ROOM_SIZE 2
#define UI_MAP_TILE_BYTES (UI_MAP_TILE_SIZE * UI_MAP_TILE_SIZE)
#define UI_MAP_TILE_COUNT 2
#define UI_MAP_BUFFER_SIZE (UI_MAP_TILE_BYTES * UI_MAP_TILE_COUNT)
#define UI_MAP_VISITED_SIZE ((WORLD_WIDTH * WORLD_HEIGHT) / 8)
#define UI_MAP_SCREEN_X (WIDTH - UI_MAP_TILE_COUNT)
#define UI_MAP_SCREEN_Y 0

static uint8_t ui_map_tiles[UI_MAP_BUFFER_SIZE];
static uint8_t ui_map_visited[UI_MAP_VISITED_SIZE];
static uint8_t ui_map_unopened_chest[UI_MAP_VISITED_SIZE];
static Vector2_u8 ui_map_position;

static uint8_t ui_map_room_index(uint8_t x, uint8_t y) {
    return (y * WORLD_WIDTH) + x;
}

static uint8_t ui_map_room_visited(uint8_t x, uint8_t y) {
    uint8_t room = ui_map_room_index(x, y);
    return ui_map_visited[room >> 3] & (1 << (room & 7));
}

static uint8_t ui_map_room_has_unopened_chest(uint8_t x, uint8_t y) {
    uint8_t room = ui_map_room_index(x, y);
    return ui_map_unopened_chest[room >> 3] & (1 << (room & 7));
}

static void ui_map_set_visited(uint8_t x, uint8_t y) {
    uint8_t room = ui_map_room_index(x, y);
    ui_map_visited[room >> 3] |= (1 << (room & 7));
}

static void ui_map_set_unopened_chest(uint8_t x, uint8_t y, uint8_t has_unopened_chest) {
    uint8_t room = ui_map_room_index(x, y);
    uint8_t bit = 1 << (room & 7);

    if (has_unopened_chest) {
        ui_map_unopened_chest[room >> 3] |= bit;
    } else {
        ui_map_unopened_chest[room >> 3] &= ~bit;
    }
}

static void ui_map_draw_room(uint8_t x, uint8_t y, uint8_t color) {
    uint8_t px = x * UI_MAP_ROOM_SIZE;
    uint8_t py = y * UI_MAP_ROOM_SIZE;
    uint16_t offset = ((uint16_t)(px / UI_MAP_TILE_SIZE) * UI_MAP_TILE_BYTES) +
                      ((uint16_t)py * UI_MAP_TILE_SIZE) + (px % UI_MAP_TILE_SIZE);

    ui_map_tiles[offset] = color;
    ui_map_tiles[offset + 1] = color;
    ui_map_tiles[offset + UI_MAP_TILE_SIZE] = color;
    ui_map_tiles[offset + UI_MAP_TILE_SIZE + 1] = color;
}

static gfx_error ui_map_upload(gfx_context* ctx) {
    gfx_tileset_options options = {
        .compression = TILESET_COMP_NONE,
        .from_byte = ((uint16_t)UI_MAP_TILE_LEFT * UI_MAP_TILE_BYTES),
    };

    return gfx_tileset_load(ctx, ui_map_tiles, UI_MAP_BUFFER_SIZE, &options);
}

static gfx_error ui_map_place(gfx_context* ctx) {
    gfx_error err = gfx_tilemap_place(ctx, UI_MAP_TILE_LEFT, UI_LAYER, UI_MAP_SCREEN_X, UI_MAP_SCREEN_Y);
    if (err != GFX_SUCCESS) return err;

    return gfx_tilemap_place(ctx, UI_MAP_TILE_RIGHT, UI_LAYER, UI_MAP_SCREEN_X + 1, UI_MAP_SCREEN_Y);
}

static gfx_error ui_map_render(gfx_context* ctx) {
    memset(ui_map_tiles, UI_MAP_COLOR_BACKGROUND, UI_MAP_BUFFER_SIZE);

    for (uint8_t y = 0; y < WORLD_HEIGHT; y++) {
        for (uint8_t x = 0; x < WORLD_WIDTH; x++) {
            if (ui_map_room_visited(x, y)) {
                uint8_t color = UI_MAP_COLOR_VISITED;
                if (ui_map_room_has_unopened_chest(x, y)) {
                    color = UI_MAP_COLOR_UNOPENED_CHEST;
                }
                ui_map_draw_room(x, y, color);
            }
        }
    }

    ui_map_draw_room(ui_map_position.x, ui_map_position.y, UI_MAP_COLOR_CURRENT);
    return ui_map_upload(ctx);
}

gfx_error ui_map_init(gfx_context* ctx, Vector2_u8 position) {
    memset(ui_map_visited, 0, UI_MAP_VISITED_SIZE);
    memset(ui_map_unopened_chest, 0, UI_MAP_VISITED_SIZE);
    ui_map_position = position;
    ui_map_set_visited(position.x, position.y);

    gfx_error err = ui_map_render(ctx);
    if (err != GFX_SUCCESS) return err;

    return ui_map_place(ctx);
}

gfx_error ui_map_set_position(gfx_context* ctx, Vector2_u8 position, uint8_t previous_has_unopened_chest) {
    ui_map_set_visited(ui_map_position.x, ui_map_position.y);
    ui_map_set_unopened_chest(ui_map_position.x, ui_map_position.y, previous_has_unopened_chest);
    ui_map_position = position;
    ui_map_set_visited(position.x, position.y);
    return ui_map_render(ctx);
}
