#ifndef MINIMAP_H
#define MINIMAP_H

#include <stdint.h>
#include <zvb_gfx.h>
#include "state.h"
#include "render.h"

/* Convenience functions combining state updates with rendering. */
gfx_error minimap_init(gfx_context* ctx, Vector2_u8 position);
/* Store the previous room's special flag, then highlight the new current room.
 * The game decides what makes a visited room special. */
gfx_error minimap_set_position(gfx_context* ctx, Vector2_u8 position, uint8_t previous_is_special);

#endif
