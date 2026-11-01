#include <zgdk.h>
#include <zgdk/tilemap.h>
#include <zos_errors.h>

#include "layout.h"

#ifndef WORLD_H
#define WORLD_H

/* Load and draw a room at the requested tilemap screen, and populate the
 * registered ZGDK Tilemap with its logical tiles. */
typedef zos_err_t (*room_loader_t)(gfx_context* ctx, uint8_t x, uint8_t y, uint8_t map_x, uint8_t map_y);

extern Vector2_u8 world_position;

/* The caller owns ctx and keeps it alive while the world is in use. */
zos_err_t world_init(gfx_context* ctx, Vector2_u8 start, room_loader_t loader);

#endif
