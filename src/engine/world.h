#include <zgdk.h>
#include <zgdk/tilemap.h>
#include <zos_errors.h>

#include "screen.h"

#ifndef WORLD_H
#define WORLD_H

#define WORLD_WIDTH 16
#define WORLD_HEIGHT 8
#define ROOM_WIDTH WIDTH
#define ROOM_HEIGHT HEIGHT
#define ROOM_OFFSET_X 0
#define ROOM_OFFSET_Y 0

typedef void (*transition_callback_t)(uint16_t x, uint16_t y);
/* Load and draw a room at the requested tilemap screen, and populate the
 * registered ZGDK Tilemap with its logical tiles and ROOM_HAS_UNOPENED_CHEST. */
typedef zos_err_t (*room_loader_t)(gfx_context* ctx, uint8_t x, uint8_t y, uint8_t map_x, uint8_t map_y);

extern Vector2_u8 world_position;
extern uint8_t ROOM_HAS_UNOPENED_CHEST;

/* The caller owns ctx and keeps it alive while the world is in use. */
zos_err_t world_init(gfx_context* ctx, Vector2_u8 start, room_loader_t loader);
uint8_t world_chest_open(uint8_t x, uint8_t y);
void world_open_chest(void);

zos_err_t screen_load(const Vector2_u8 world, const Edge edge, const Point point, transition_callback_t const callback);
/* Returns 1 after a transition, 0 at a world boundary, or a negated load error. */
int16_t screen_transition(const Edge edge, const Point point, const transition_callback_t callback);

#endif
