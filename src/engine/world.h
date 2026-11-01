#include <zgdk.h>
#include <zos_errors.h>

#include "screen.h"
#include "tile.h"

#ifndef WORLD_H
#define WORLD_H

#define WORLD_WIDTH 16
#define WORLD_HEIGHT 8
#define ROOM_WIDTH WIDTH
#define ROOM_HEIGHT HEIGHT
#define ROOM_OFFSET_X 0
#define ROOM_OFFSET_Y 0
#define TILEMAP_OFFSET 0

typedef void (*transition_callback_t)(uint16_t x, uint16_t y);

extern Vector2_u8 world_position;
extern uint8_t ROOM_HAS_UNOPENED_CHEST;

extern TileType SCREEN[ROOM_HEIGHT][ROOM_WIDTH];

/* The caller owns ctx and keeps it alive while the world is in use. */
zos_err_t world_init(gfx_context* ctx, Vector2_u8 start);
uint8_t world_get_tile(const uint8_t world_x, const uint8_t world_y, const uint8_t tile);
void world_set_tile(const uint8_t x, const uint8_t y, const uint8_t tile);

void screen_load(const Vector2_u8 world, const Edge edge, const Point point, transition_callback_t const callback);
uint8_t screen_transition(const Edge edge, const Point point, const transition_callback_t callback);
TileType screen_get_tile(const uint8_t x, const uint8_t y);

#endif
