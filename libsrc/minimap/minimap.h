#ifndef MINIMAP_H
#define MINIMAP_H

#include "state.h"
#include "render.h"

/* Convenience API combining state and rendering; configs are copied. */
gfx_error minimap_init(gfx_context* ctx, const minimap_state_config_t* state,
    const minimap_render_config_t* render, Vector2_u8 position);
gfx_error minimap_set_position(gfx_context* ctx, Vector2_u8 position, uint8_t previous_is_special);

#endif
