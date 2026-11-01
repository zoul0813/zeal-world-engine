#ifndef MINIMAP_H
#define MINIMAP_H

#include <stdint.h>
#include <zvb_gfx.h>
#include <zgdk.h>

#define MINIMAP_COLOR_CURRENT 16
#define MINIMAP_COLOR_VISITED 27
#define MINIMAP_COLOR_SPECIAL 36
#define MINIMAP_COLOR_UNVISITED 23

#define MINIMAP_TILE_LEFT 0xFD
#define MINIMAP_TILE_RIGHT 0xFE

gfx_error minimap_init(gfx_context* ctx, Vector2_u8 position);
/* Store the previous room's special flag, then highlight the new current room.
 * The game decides what makes a visited room special. */
gfx_error minimap_set_position(gfx_context* ctx, Vector2_u8 position, uint8_t previous_is_special);

#endif
